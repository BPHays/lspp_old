#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <sstream>
#include <regex>
#include <chrono>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>
#include <dirent.h>
#include <assert.h>

#include <time.h>

#include "lspp.hpp"
#include "format.hpp"
#include "fileEnt.hpp"

#include <stdio.h>

/**
 * @brief perform format lookup by filename
 *
 * @param f file entry to look up
 *
 * @return true if the format was set
 */
bool lookupByFilename(fileEnt & f) {
  static std::unordered_map<const fileNameFmt *, std::regex> regMap;
  std::string name = f.getName(); for(std::size_t i = 0; i < sizeof(nameFormat)/sizeof(*nameFormat); i++) {
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
  if (baseName[0] == '.') {
    // Check if file is a dotfile
    f.setFmt(&generalFormat[dotfileIndex]);
    return true;
  } else {
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
  }

  // Use the default if extension not found
  f.setFmt(&generalFormat[fileIndex]);
  extCache[extension] = &generalFormat[fileIndex];
  return true;
}

/**
 * @brief check if files can be fit in witdh columns on the terminal
 *
 * @param filenames the list of files to pack
 * @param width the width of the terminal
 * @param rows the number of rows to try and pack into
 * @param colWidths a vector of each column (of files) width
 *
 * @return true if the files can be fit in rows rows
 */
static inline bool fitsInNRows(
    std::vector<fileEnt> & filenames, 
    unsigned short width, 
    size_t rows,
    std::vector<size_t> & colWidths
    ) 
{
  const size_t padding = 4;
  unsigned short totalSize = 0;
  size_t colWidth;
  size_t suffixLen;
  for (size_t col = 0; col < (filenames.size() + rows - 1) / rows; ++col) {
    colWidth = 0;
    for (size_t row = 0; row < rows; ++row) {
      if (col * rows + row + 1 > filenames.size()) { 
        break; 
      }
      fileEnt & ent = filenames[col * rows + row];
      suffixLen = ent.getNSuffixIcons() > 0 ? 2 * ent.getNSuffixIcons() : 0;
      colWidth = std::max(colWidth, ent.getName().length() + suffixLen + padding);
    }
    colWidths.push_back(colWidth);
    totalSize += colWidth;
  }
  return totalSize <= width;
}

void printByType(std::vector<fileEnt> & filenames) {
  std::map<const std::string, std::vector<fileEnt> > typeMap;

  // Generate list by types
  std::for_each(filenames.begin(), filenames.end(), 
    [&typeMap](auto & f){
      typeMap[f.getFileType()->typeName].push_back(f);});

  // Print by type
  for(auto & t : typeMap) {
    std::cout << ESC "0m" << t.first << std::endl;
    if (flagSet.test(flags::longList)) {
      printList(t.second);
    } else {
      printColumns(t.second);
    }
    std::cout << std::endl;
  }
}

/**
 * @brief print the list of file entries in right sized columns
 *
 * Binary seach for the number of rows needed to fit all of the files in
 * right sized columns and then print the files in their columns
 *
 * @param filenames list of file entries to print
 */
void printColumns(std::vector<fileEnt> & filenames) {
  std::vector<size_t> colWidths;
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  unsigned short width = w.ws_col;

  // Get rough estimate of number of rows required
  size_t rows = 1;
  for(rows = 1;; rows *= 2) {
    colWidths.clear();
    if (fitsInNRows(filenames, width, rows, colWidths)) {
      /* Kill loop without incrementing rows */
      break;
    }
  }

  // Binary search for the lowest number of columns that still can
  // fit all of the files
  size_t max = rows;
  size_t min = std::max((size_t) 2, rows / 2);
  size_t test = max;
  bool   lastFail = false;
  for(test = (max + min) / 2; max > min; test = (max + min) / 2, colWidths.clear()) {
    if (fitsInNRows(filenames, width, test, colWidths)) {
      max = test;
      lastFail = false;
    } else {
      min = test + 1;
      lastFail = true;
    }
  }

  rows = max;

  // If the last search was a failure, then we must regenerate the column list
  if (lastFail) {
    colWidths.clear(); fitsInNRows(filenames, width, rows, colWidths);
  }

  size_t maxCols = (filenames.size() + rows - 1) / rows;
  for (size_t row = 0; row < rows; ++row) {
    for (size_t col = 0; col < maxCols; ++col) {
        if (col * rows + row + 1 > filenames.size()) { 
          break; 
        }
        if ((col + 1) * rows + row < filenames.size()) {
          std::cout << filenames[col * rows + row].formatted(colWidths[col] - 2);
          //ss << filenames[col * rows + row].formatted(colWidths[col] - 2);
        } else {
          std::cout << filenames[col * rows + row].formatted(0);
          //ss << filenames[col * rows + row].formatted(0);
        }
    }
    std::cout << std::endl;
  }
}

/**
 * @brief print the entry in long list format
 *
 * @param f file entry to print
 */
void printList(fileEnt & f, unsigned char linkWidth) {
  // print formatting here
  // TODO implement usual ls flags to control columns
  std::cout << f.getEmphasis();
  std::cout << f.getColor();
  std::cout << f.getPermissionString() << " ";
  std::cout << f.getRefCnt(linkWidth) << " ";
  if (!flagSet.test(flags::g)) {
    std::cout << f.getOwnerName() << " ";
  }
  if (!flagSet.test(flags::o)) {
    std::cout << f.getGroupName() << " ";
  }
  std::cout << f.getSize() << " ";
  std::cout << f.getTimestamp() << " ";
  std::cout << f.getIcon() << " ";
  std::cout << f.getName() << f.getSuffixIcons();
  if (f.isLink()) {
    std::cout << " " << f.getTarget();
  }
  std::cout << std::endl;
}

/**
 * @brief print a list of entries in long list format
 *
 * @param filenames list of file entries to print
 */
void printList(std::vector<fileEnt> & filenames) {
  size_t userMax = 0, groupMax = 0, linksMax = 0;

  // Find correct widths
  for(fileEnt & f : filenames) {
    userMax  = std::max(f.getOwnerName().length(), userMax);
    groupMax = std::max(f.getGroupName().length(), groupMax);
    linksMax = std::max(f.getRefCnt().length(), linksMax);
  }

  // Pad strings
  fileEnt::padUserNames(userMax);
  fileEnt::padGroupNames(groupMax);
  //TODO pad size column (currently works up to a petabyte)

  // Print strings
  for(fileEnt & f : filenames) {
    printList(f, linksMax);
  }
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

void usage() {
  std::cout << "Usage: lspp [al] [directory]" << std::endl;
  exit(0);
}

// Tell getopt not to print error messages for me
extern int opterr = 0;

void setoption(struct option & op, const char * name, int has_arg, int * flag, int val) {
  op.name    = name;
  op.has_arg = has_arg;
  op.flag    = flag;
  op.val     = val;
}

std::string listType;

void parseArgs(int argc, char ** argv) {
  short c;
  int  option_index = 0;

  // gnu style long options 
  // start switch indices after ascii to avoid collisions
  enum longOptIndex : short {ft = 128, type = 129};
  struct option longopts[] = {
    {"all",             0, NULL, 'a'},
    {"allmost-aLl",     0, NULL, 'A'},
    {"help",            0, NULL, 'h'},
    {"ft",              1, NULL, ft },
    {"type",            0, NULL, type },
    {NULL,              0, NULL, 0  }
  };

  // parse the args
  while((c = getopt_long(argc, argv, "aAghlo", longopts, &option_index)) != -1) {
    switch (c) {
      // Handle long only args
      case ft:
        flagSet.set(flags::ft);
        listType = std::string(optarg);
        break;
      case type:
        flagSet.set(flags::type);
        break;
      // Handle short args, argumnets with both a long and short argument
      // have their long argument routed to the same location as the short arg
      case 'a': flagSet.set(flags::all);        break;
      case 'A': flagSet.set(flags::almostAll);  break;
      case 'g': flagSet.set(flags::g);          break;
      case 'h': flagSet.set(flags::help);       break;
      case 'l': flagSet.set(flags::longList);   break;
      case 'o': flagSet.set(flags::o);          break;
      default:  execvp("ls", argv);
    }
  }
}

int main(int argc, char **argv) {
  std::vector<fileEnt> filenames;
  std::string lsdir = ".";

  std::cout.sync_with_stdio(false);
  std::setvbuf(stdout, NULL, _IOFBF, 8192);

  parseArgs(argc, argv);

  if (flagSet.test(flags::help)) {
    usage();
  }

  if (optind < argc) {
    lsdir = std::string(argv[optind]); }

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
          flagSet.test(flags::all) || flagSet.test(flags::almostAll)) {
        // Check for -A almost all
        if (flagSet.test(flags::almostAll) && 
           (!strcmp(dent->d_name, ".") || !strcmp(dent->d_name, ".."))) {
          continue;
        }
        filenames.push_back(fileEnt(lsdir, dent->d_name, dent->d_type));
      }
    }
    
    // Sort the files
    std::sort(filenames.begin(), filenames.end());
  } else {
    // TODO add lsing single files
    std::cerr << "Currently cannot stat individual files" << std::endl;
    exit(-1);
  }

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

  if (flagSet.test(flags::ft)) {
    std::vector<fileEnt> filteredNames;
    for(auto it = filenames.begin(); it != filenames.end(); ++it) {
      // Remove any files which don't have the correct type
      if (isChildType(it->getFileType(), listType)) {
        filteredNames.push_back(*it);
      }
    }
    filenames = filteredNames;
  }

  if (flagSet.test(flags::type)) {
    printByType(filenames);
  } else if (flagSet.test(flags::longList)) {
    printList(filenames);
  } else {
    printColumns(filenames);
  }
}
