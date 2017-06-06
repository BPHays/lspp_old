#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <sstream>
#include <regex>
#include <chrono>
#include <functional>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <assert.h>

#include <time.h>

#include "lspp.hpp"
#include "format.hpp"
#include "fileEnt.hpp"
#include "usage.hpp"

#include <stdio.h>

argSet * args;

/**
 * @brief perform format lookup by filename
 *
 * @param f file entry to look up
 *
 * @return true if the format was set
 */
bool lookupByFilename(fileEnt & f) {
  static std::unordered_map<const fileNameFmt *, std::regex> regMap;
  std::string name = f.getName(); 
  for(std::size_t i = 0; i < sizeof(nameFormat)/sizeof(*nameFormat); i++) {
    const fileNameFmt * entry = &nameFormat[i];
    if (entry->reg) {
      // Handle if the filename format is a regex
      std::regex re;

      // Use a cache to avoid recomputing regexes
      auto it = regMap.find(entry);
      if (it == regMap.end()) {
        regMap[&nameFormat[i]] = std::regex(entry->name);
      }
      if(regex_match(name, regMap[&nameFormat[i]])) {
        f.setFmt(entry);
        return true;
      }
    } else {
      // Handle if the filename format is simple string
      if (name == entry->name) {
        f.setFmt(entry);
        return true;
      }
    }
  }
  return false;
}

/**
 * @brief perform format lookup by extension
 *
 * @param f file entry to look up
 * @param baseName file's basename
 * @param extension file's extension
 *
 * @return true if the format was set
 */
bool lookupByExtension(fileEnt & f, std::string baseName, std::string extension) {
  static std::unordered_map<std::string, const fileFmt *> extCache;
  // First try to find extension in cache
  auto it = extCache.find(extension);
  if (it != extCache.end()) {
    f.setFmt(it->second);
    return true;
  } else {
    // Find file extension
    for(std::size_t i = 0; i < sizeof(extFormat)/sizeof(*extFormat); i++) {
      const fileFmt * entry = &extFormat[i];
      if (extension == entry->name) {
        f.setFmt(entry);
        extCache[extension] = &extFormat[i];
        return true;
      }
    }
  }

  if (baseName[0] == '.') {
    // Check if file is a dotfile
    f.setFmt(&generalFormat[dotfileIndex]);
    return true;
  }

  // Use the default if extension not found
  f.setFmt(&generalFormat[fileIndex]);
  extCache[extension] = &generalFormat[fileIndex];
  return true;
}

/**
 * @brief Check if the child's filetype or any of it's ancestors are typeName
 *
 * @param fType the file's type
 * @param typeName the name of the type to search for
 *
 * @return true if the file or one of its ancestors is of type typeName
 */
bool isChildType(const fileType * fType, std::string typeName) {
  for(;fType != NULL; fType = fType->parent) {
    if (!fType->typeName.compare(typeName)) {
      return true;
    }
  }
  return false;
}

/**
 * @brief Print the program's usage message
 */
void usage() {
  //std::cout << "Usage: lspp [al] [directory]" << std::endl;
  std::cout << usageMsg << std::endl;
  exit(0);
}

/**
 * @brief open dir and read in the list of files or the file is dir is a file
 *
 * @param dir the directory to open, may be a regular file as well
 * @param filenames a list to populate with the filenames
 */
void getFiles(const std::string lsdir, std::vector<fileEnt> & filenames) {
  // Check if a directory or a file
  struct stat stats;
  if (stat(lsdir.c_str(), &stats) < 0) {
    perror("stat: ");
    exit(-1);
  }

  if (S_ISDIR(stats.st_mode)) {
    struct dirent * dent;
    DIR * dir = opendir(lsdir.c_str());
    if (dir == NULL) {
      perror("opendir: ");
      exit(-1);
    }

    // Read out all of the files
    while((dent = readdir(dir))) {
      if (dent->d_name[0] != '.' || 
          args->getFlag(argSet::flags::all) || args->getFlag(argSet::flags::almostAll)) {
        // Check for -A almost all
        if (args->getFlag(argSet::flags::almostAll) && 
           (!strcmp(dent->d_name, ".") || !strcmp(dent->d_name, ".."))) {
          continue;
        }
        filenames.push_back(fileEnt(lsdir, dent->d_name, dent->d_type));
      }
    }
    
  } else {
    // TODO either need to read the directory above the file, or need
    // to have a way to not need to use the dirent data
    // currently lists the file and doesn't crash but metadata is garbage
    std::string name, dir;
    std::size_t index = lsdir.find_last_of("/");
    if (index == std::string::npos) {
      name = lsdir;
      dir = ".";
    } else {
      dir = lsdir.substr(index);
      name = lsdir.substr(index + 1);
    }
    filenames.push_back(fileEnt(dir, name));
  }
}

void getFormatStyle(std::vector<fileEnt> & filenames) {
  // Find the correct formatting settings
  for(fileEnt & f : filenames) {
    std::string name = f.getName();
    std::string ext;
    std::string base;

    // Extract basename and extension from the filename
    std::size_t index = name.find_last_of(".");
    if (index == std::string::npos || index == 0) {
      base = name;
      ext = "";
    } else {
      base = name.substr(0, index);
      ext = name.substr(index + 1);
    }

    // Look up the format to use for each file
    switch(f.getStat().st_mode & S_IFMT) {
      case S_IFDIR:
        f.setFmt(&generalFormat[dirIndex]);
        break;
      case S_IFCHR:
        f.setFmt(&generalFormat[chrDevIndex]);
        break;
      case S_IFBLK:
        f.setFmt(&generalFormat[blkDevIndex]);
        break;
      case S_IFSOCK:
        f.setFmt(&generalFormat[sockIndex]);
        break;
      case S_IFIFO:
        f.setFmt(&generalFormat[fifoIndex]);
        break;
      case S_IFREG:
        /* fallthrough */
      default:
        // Handle reserved Filenames
        if (lookupByFilename(f)) break;

        // Handle files by extension
        if (lookupByExtension(f, base, ext)) break;
        
        // Should never get here
        assert(0);
    }
  }
}

void filterFiles(std::vector<fileEnt> & filenames) {
  auto it = remove_if(filenames.begin(), filenames.end(),
    [](auto f)
      { return !isChildType(f.getFileType(), args->getListType()); });
  filenames.erase(it, filenames.end());
}

/**
 * @brief sort the files alphabetically or according to the pased flags
 *
 * @param filenames the list of files to sort
 */
void sortFiles(std::vector<fileEnt> & filenames) {
  std::function<int(fileEnt const &, fileEnt const &)> sortBy;

  if (args->getFlag(argSet::flags::sortInDir)) {
    // Keep the files in the order they were in in the directory
    return;
  } else if (args->getFlag(argSet::flags::sortTime)) {
    // Sort by time
    sortBy = [](auto const & x, auto const & y) {
              return x.getModTS() < y.getModTS();};
  } else if (args->getFlag(argSet::flags::sortAccTime)) {
    // Sort by time
    sortBy = [](auto const & x, auto const & y) {
              return x.getAccTS() < y.getAccTS();};
  } else if (args->getFlag(argSet::flags::sortSize)) {
    // Sort by fileSize
    sortBy = [](auto const & x, auto const & y) {
              return x.getSize() < y.getSize();};

  } else if (args->getFlag(argSet::flags::sortExt)) {
    // Sort first by extension then by filename
    sortBy = [](auto const & x, auto const & y) {
              const std::string & xn = x.getName();
              const std::string & yn = y.getName();

              const size_t xpos = xn.find_last_of(".");
              const std::string & xext = xpos != std::string::npos ? xn.substr(xpos + 1) : "";
              const size_t ypos = yn.find_last_of(".");
              const std::string & yext = ypos != std::string::npos ? yn.substr(ypos + 1) : "";

              const int cmp = xext.compare(yext);
              if (cmp == 0) { return xn.compare(yn) < 0; } 
              else          { return cmp < 0; }};
  } else {
    // Sort by filename
    sortBy = [](auto const & x, auto const & y) {
              const char * xc = x.getName().c_str();
              const char * yc = y.getName().c_str();
              if (*xc == '.') ++xc;
              if (*yc == '.') ++yc;
              return strcasecmp(xc, yc) < 0;};
  };
 
  if (args->getFlag(argSet::flags::reverse)) {
    std::sort(filenames.rbegin(), filenames.rend(), sortBy);
  } else {
    std::sort(filenames.begin(), filenames.end(), sortBy);
  }
}

void listTree(std::string lsdir) {
    std::cout << lsdir << std::endl;
    listDirectory(lsdir);
}

void recursiveList(std::string lsdir) {
  std::vector<fileEnt>  filenames;
  std::vector<fileEnt>  directories;

  std::cout << std::endl << "\033[0;m" << lsdir << ":" << std::endl;

  // get all of the files in the directory
  getFiles(lsdir, filenames);

  // look up the correct format for each file
  getFormatStyle(filenames);

  directories = filenames;

  // Filter the filenames to only files with the specified fileType
  if (args->getFlag(argSet::flags::ft)) { filterFiles(filenames); }

  // Sort the files
  sortFiles(filenames);

  if (args->getFlag(argSet::flags::type)) { 
    // Print each typeType together either in long list or columnar format
    printByType(filenames); 
  } else {
    // Print the files normally
    printFiles(filenames);
  }

  // After listing the parent directory recursively list all child directories
  for_each(directories.begin(), directories.end(),
           [](fileEnt & f) { if (f.isDir()) recursiveList(f.getPath());});
}

void listDirectory(std::string lsdir) {
  std::vector<fileEnt>  filenames;
  std::vector<fileEnt>  directories;

  // get all of the files in the directory
  getFiles(lsdir, filenames);

  // look up the correct format for each file
  getFormatStyle(filenames);

  directories = filenames;

  // Filter the filenames to only files with the specified fileType
  if (args->getFlag(argSet::flags::ft)) { filterFiles(filenames); }

  // Sort the files
  sortFiles(filenames);

  if (args->getFlag(argSet::flags::type)) { 
    // Print each typeType together either in long list or columnar format
    printByType(filenames); 
  } else {
    // Print the files normally
    printFiles(filenames);
  }
}

int main(int argc, char **argv) {
  std::string           lsdir;

  std::cout.sync_with_stdio(false);
  std::setvbuf(stdout, NULL, _IOFBF, 8192);

  // Parse the command line flags and get the directory to list
  args = new argSet(argc, argv);

  // Print the usage message and exit if the help flag was set
  if (args->getFlag(argSet::flags::help)) { usage(); }

  if (args->getFlag(argSet::flags::tree)) {
    listTree(args->getLsDir()); 
  } else if (args->getFlag(argSet::flags::recursive)) {
    recursiveList(args->getLsDir());
  } else {
    listDirectory(args->getLsDir());
  }

  delete args;
}
