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
    printFiles(t.second);
    std::cout << std::endl;
  }
}

static void printFormatColumn(fileEnt & f, size_t length) {
  std::string padding = "";
  size_t suffixLen = f.getNSuffixIcons() > 0 ? 2 * f.getNSuffixIcons() : 0;
  ssize_t padLen = length - f.getName().length() - suffixLen;
  if (padLen >= 0) {
    padding.resize(padLen, ' ');
  }
  if (args.getFlag(argSet::flags::color)) {
    std::cout << f.getColor();
  }
  if (args.getFlag(argSet::flags::icon)) {
    std::cout << f.getIcon() << " ";
  }
  std::cout << f.getName() << f.getSuffixIcons() << padding;
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
          //std::cout << filenames[col * rows + row].formatted(colWidths[col] - 2);
          printFormatColumn(filenames[col * rows + row], colWidths[col] - 2);
        } else {
          //std::cout << filenames[col * rows + row].formatted(0);
          printFormatColumn(filenames[col * rows + row], 0);
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
void printLongList(fileEnt & f, unsigned char linkWidth) {
  // print formatting here
  // TODO implement usual ls flags to control columns
  if (!args.getFlag(argSet::flags::noFmt)) {
    std::cout << f.getEmphasis();
    std::cout << f.getColor();
  }
  std::cout << f.getPermissionString() << " ";
  std::cout << f.getRefCnt(linkWidth) << " ";
  if (!args.getFlag(argSet::flags::noOwner)) {
    std::cout << f.getOwnerName() << " ";
  }
  if (!args.getFlag(argSet::flags::noGroup)) {
    std::cout << f.getGroupName() << " ";
  }
  if (args.getFlag(argSet::flags::author)) {
    std::cout << f.getOwnerName() << " ";
  }
  std::cout << f.getSizeStr() << " ";
  std::cout << f.getTimestampStr() << " ";
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
void printLongList(std::vector<fileEnt> & filenames) {
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
  for_each(filenames.begin(), filenames.end(),
            [linksMax](auto & f) { 
              if (!args.getFlag(argSet::flags::noFmt) &&
                  args.getFlag(argSet::flags::color)) {
                std::cout << f.getEmphasis();
                std::cout << f.getColor();
              }
              std::cout << f.getPermissionString() << " ";
              std::cout << f.getRefCnt(linksMax) << " ";
              if (!args.getFlag(argSet::flags::noOwner)) {
                std::cout << f.getOwnerName() << " ";
              }
              if (!args.getFlag(argSet::flags::noGroup)) {
                std::cout << f.getGroupName() << " ";
              }
              if (args.getFlag(argSet::flags::author)) {
                std::cout << f.getOwnerName() << " ";
              }
              std::cout << f.getSizeStr() << " ";
              std::cout << f.getTimestampStr() << " ";
              if (args.getFlag(argSet::flags::icon)) {
                std::cout << f.getIcon() << " ";
              }
              std::cout << f.getName() << f.getSuffixIcons();
              if (f.isLink()) {
                std::cout << " " << f.getTarget();
              }
              std::cout << std::endl; });
} 

/**
 * @brief simply print each file on its own line
 *
 * @param filenames the list of files to print
 */
void printList(std::vector<fileEnt> & filenames) {
  for_each(filenames.begin(), filenames.end(), 
           [](auto & f) { 
            if (args.getFlag(argSet::flags::color)) {
              std::cout << f.getColor();
            }
            if (args.getFlag(argSet::flags::icon)) {
              std::cout << f.getIcon() << " ";
            }
            std::cout << f.getName() << std::endl;});
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
  std::cout << "Usage: lspp [al] [directory]" << std::endl;
  exit(0);
}

// Tell getopt not to print error messages. This allows cleaner handling of
// a getopt failure where /bin/ls is simply called
extern int opterr = 0;

std::string listType;

/**
 * @brief Update the flagSet to match the provided flags and store any params
 *
 * @param argc number of command line argumnets
 * @param argv array of the command line arguments
 *
 * @return std::string the directory to list
 */
void parseArgs(int argc, char * const * argv) {
  short c;
  int  option_index = 0;

  // gnu style long options 
  // start switch indices after ascii to avoid collisions
  enum longOptIndex : short {
    ft = 128, type = 129, author = 130, noFmt = 131, color = 132, 
    icon = 133, tree = 134};
  struct option longopts[] = {
    {"all",             0, NULL, 'a'    },
    {"allmost-aLl",     0, NULL, 'A'    },
    {"author",          0, NULL, author },
    {"help",            0, NULL, 'h'    },
    {"reverse",         0, NULL, 'r'    },
    {"ft",              1, NULL, ft     },
    {"noFmt",           0, NULL, noFmt  },
    {"type",            0, NULL, type   },
    {"color",           2, NULL, color  },
    {"icon",            2, NULL, icon   },
    {"recursive",       0, NULL, 'R'},
    {"tree",            0, NULL, tree},
    {NULL,              0, NULL, 0      }
  };

  // parse the args
  while((c = getopt_long(argc, argv, "aAghlorRStUX1", longopts, &option_index)) != -1) {
    switch (c) {
      // Handle long only args
      case ft:
        args.setFlag(argSet::flags::ft);
        listType = std::string(optarg);
        break;
      case color:   
        if (optarg == NULL) {
          // default to "always"
          args.setFlag(argSet::flags::color);  
        } else {
          std::string colorArg = std::string(optarg);
          if (!colorArg.compare("auto") && isatty(1)) {
            // only color the output when stdout is a tty
            args.setFlag(argSet::flags::color);
          } else if (!colorArg.compare("always")) { 
            // always color the output
            args.setFlag(argSet::flags::color);
          } else if (!colorArg.compare("never")) {
            // never color the output
            args.setFlag(argSet::flags::color, false);
          }
        }
        break;
      case icon:   
        if (optarg == NULL) {
          // default to "always"
          args.setFlag(argSet::flags::icon);  
        } else {
          std::string iconArg = std::string(optarg);
          if (!iconArg.compare("auto") && isatty(1)) {
            // only color the output when stdout is a tty
            args.setFlag(argSet::flags::icon);
          } else if (!iconArg.compare("always")) { 
            // always color the output
            args.setFlag(argSet::flags::icon);
          } else if (!iconArg.compare("never")) {
            // never color the output
            args.setFlag(argSet::flags::icon, false);
          }
        }
        break;
      case type:    args.setFlag(argSet::flags::type);   break;
      case author:  args.setFlag(argSet::flags::author); break;
      case noFmt:   args.setFlag(argSet::flags::noFmt);  break;
      case tree:    args.setFlag(argSet::flags::tree);   break;

      // Handle short args, argumnets with both a long and short argument
      // have their long argument routed to the same location as the short arg
      case 'a': args.setFlag(argSet::flags::all);        break;
      case 'A': args.setFlag(argSet::flags::almostAll);  break;
      case 'g': args.setFlag(argSet::flags::noOwner);    break;
      case 'h': args.setFlag(argSet::flags::help);       break;
      case 'l': args.setFlag(argSet::flags::longList);   break;
      case 'o': args.setFlag(argSet::flags::noGroup);    break;
      case 'r': args.setFlag(argSet::flags::reverse);    break;
      case 'R': args.setFlag(argSet::flags::recursive);  break;
      case 'S': args.setFlag(argSet::flags::sortSize);   break;
      case 't': args.setFlag(argSet::flags::sortTime);   break;
      case 'U': args.setFlag(argSet::flags::sortInDir);  break;
      case 'X': args.setFlag(argSet::flags::sortExt);    break;
      case '1': args.setFlag(argSet::flags::filePerLine);break;
      default:  execvp("ls", argv);
    }
  }

  // Get directory to list or use "." if none provided
  if (optind < argc) {
    args.setLsDir(std::string(argv[optind]));
  } else {
    args.setLsDir(".");
  }
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
          args.getFlag(argSet::flags::all) || args.getFlag(argSet::flags::almostAll)) {
        // Check for -A almost all
        if (args.getFlag(argSet::flags::almostAll) && 
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
      { return !isChildType(f.getFileType(), listType); });
  filenames.erase(it, filenames.end());
}

/**
 * @brief sort the files alphabetically or according to the pased flags
 *
 * @param filenames the list of files to sort
 */
void sortFiles(std::vector<fileEnt> & filenames) {
  std::function<int(fileEnt const &, fileEnt const &)> sortBy;

  if (args.getFlag(argSet::flags::sortInDir)) {
    // Keep the files in the order they were in in the directory
    return;
  } else if (args.getFlag(argSet::flags::sortTime)) {
    // Sort by time
    sortBy = [](auto const & x, auto const & y) {
              return x.getModTS() < y.getModTS();};

  } else if (args.getFlag(argSet::flags::sortSize)) {
    // Sort by fileSize
    sortBy = [](auto const & x, auto const & y) {
              return x.getSize() < y.getSize();};

  } else if (args.getFlag(argSet::flags::sortExt)) {
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
 
  if (args.getFlag(argSet::flags::reverse)) {
    std::sort(filenames.rbegin(), filenames.rend(), sortBy);
  } else {
    std::sort(filenames.begin(), filenames.end(), sortBy);
  }
}

/**
 * @brief print the list of files according to the command line flags
 *
 * @param filenames the list of files to be printed
 */
void printFiles(std::vector<fileEnt> & filenames) {
  if (args.getFlag(argSet::flags::longList) || 
      args.getFlag(argSet::flags::noGroup) || 
      args.getFlag(argSet::flags::noOwner)) {
    // Print in long list format
    printLongList(filenames);
  } else {
    if (isatty(1) && !args.getFlag(argSet::flags::filePerLine)) {
      // Print in compact columnar format
      printColumns(filenames);
    } else {
      printList(filenames);
    }
  }
}

void listDirectory(std::string lsdir) {
  std::vector<fileEnt>  filenames;
  std::vector<fileEnt>  directories;

  if (args.getFlag(argSet::flags::recursive)) {
    std::cout << std::endl << "\033[0;m" << lsdir << ":" << std::endl;
  }

  // get all of the files in the directory
  getFiles(lsdir, filenames);

  // look up the correct format for each file
  getFormatStyle(filenames);

  directories = filenames;

  // Filter the filenames to only files with the specified fileType
  if (args.getFlag(argSet::flags::ft)) { filterFiles(filenames); }

  // Sort the files
  sortFiles(filenames);

  if (args.getFlag(argSet::flags::type)) { 
    // Print each typeType together either in long list or columnar format
    printByType(filenames); 
  } else {
    // Print the files normally
    printFiles(filenames);
  }

  // After listing the parent directory recursively list all child directories
  if (args.getFlag(argSet::flags::recursive)) {
    for_each(directories.begin(), directories.end(),
             [](fileEnt & f) { if (f.getType() == DT_DIR) listDirectory(f.getPath());});
  }
}

int main(int argc, char **argv) {
  std::string           lsdir;

  std::cout.sync_with_stdio(false);
  std::setvbuf(stdout, NULL, _IOFBF, 8192);

  // Parse the command line flags and get the directory to list
  parseArgs(argc, argv);

  // Print the usage message and exit if the help flag was set
  if (args.getFlag(argSet::flags::help)) { usage(); }

  listDirectory(args.getLsDir());
}
