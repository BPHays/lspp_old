#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>
#include <dirent.h>
#include <assert.h>

#include "lspp.hpp"
#include "format.hpp"
#include "formatTab.hpp"
#include "fileEnt.hpp"

/**
 * @brief perform format lookup by filename
 *
 * @param f file entry to look up
 *
 * @return true if the format was set
 */
bool lookupByFilename(fileEnt & f) {
  std::string name = f.getName();
  for(std::size_t i = 0; i < sizeof(nameFormat)/sizeof(*nameFormat); i++) {
    std::string * entry = nameFormat[i];
    if (name == entry[key]) {
      f.setFmt(entry);
      return true;
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
  if (baseName[0] == '.') {
    // Check if file is a dotfile
    f.setFmt(generalFormat[dotfileIndex]);
    return true;
  } else {
    // Find file extension
    for(std::size_t i = 0; i < sizeof(extFormat)/sizeof(*extFormat); i++) {
      std::string * entry = extFormat[i];
      if (extension == entry[key]) {
        f.setFmt(entry);
        return true;
      }
    }
  }
  // Use the default if extension not found
  f.setFmt(generalFormat[fileIndex]);
  return true;
}

/**
 * @brief print the list of file entries in right sized columns
 *
 * @param filenames list of file entries to print
 */
void printColumns(std::vector<fileEnt> & filenames) {
  std::vector<size_t> colWidths;
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  unsigned short width = w.ws_col;

  size_t rows = 0;
  size_t colWidth;
  int totalSize;
  do {
    ++rows;
    totalSize = 0;
    colWidths.clear();
    for (size_t col = 0; col < (filenames.size() + rows - 1) / rows; ++col) {
      colWidth = 0;
      for (size_t row = 0; row < rows; ++row) {
        if (col * rows + row + 1 > filenames.size()) { 
          break; 
        }
        colWidth = std::max(colWidth, filenames[col * rows + row].getName().length() + 4);
      }
      colWidths.push_back(colWidth);
      totalSize += colWidth;
    }
  } while(totalSize > width);

  std::string fmt;
  for (size_t row = 0; row < rows; ++row) {
    for (size_t col = 0; col < (filenames.size() + rows - 1) / rows; ++col) {
        if (col * rows + row + 1 > filenames.size()) { 
          break; 
        }
        //fmt = filenames[col * rows + row].formatted(col);
        //std::cout << fmt;
        std::cout << filenames[col * rows + row].formatted(colWidths[col] - 2);
    }
    std::cout << std::endl;
  }
}

/**
 * @brief print the entry in long list format
 *
 * @param f file entry to print
 */
void printList(fileEnt & f) {
  // print formatting here
  // TODO implement usual ls flags to control columns
  std::cout << f.getColor();
  std::cout << f.getPermissionString() << " ";
  std::cout << f.getOwnerName() << " ";
  std::cout << f.getGroupName() << " ";
  std::cout << f.getSize() << " ";
  std::cout << f.getTimestamp() << " ";
  std::cout << f.getIcon() << " ";
  std::cout << f.getName() << std::endl;
}

/**
 * @brief print a list of entries in long list format
 *
 * @param filenames list of file entries to print
 */
void printList(std::vector<fileEnt> & filenames) {
  size_t userMax = 0, groupMax = 0;

  // Find correct widths
  for(fileEnt & f : filenames) {
    userMax  = std::max(f.getOwnerName().length(), userMax);
    groupMax = std::max(f.getGroupName().length(), groupMax);
  }

  // Pad strings
  fileEnt::padUserNames(userMax);
  fileEnt::padGroupNames(groupMax);
  //TODO pad size column (currently works up to a petabyte)

  // Print strings
  for(fileEnt & f : filenames) {
    printList(f);
  }
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

int main(int argc, char **argv) {
  // Handle arguments
  int  allFlag = false;
  bool longFlag = false;
  int  helpFlag = false;
  std::string lsdir = ".";
  char c;
  struct option longopts[2];
  setoption(longopts[0], "all",  0, &allFlag,  (int) true);
  setoption(longopts[1], "help", 0, &helpFlag, (int) true);
  int option_index = 0;
  while((c = getopt_long(argc, argv, "ahl", longopts, &option_index)) != -1) {
    switch (c) {
      case 0:
        break;
      case 'a':
        allFlag = true;
        break;
      case 'h':
        break;
      case 'l':
        longFlag = true;
        break;
      default:
        execvp("ls", argv);
    }
  }

  if (helpFlag) {
    usage();
  }

  if (optind < argc) {
    lsdir = std::string(argv[optind]);
  }

  struct dirent * dent;
  DIR * dir = opendir(lsdir.c_str());
  std::vector<fileEnt> filenames;

  // Read out all of the files
  while((dent = readdir(dir))) {
    if (dent->d_name[0] != '.' || allFlag) {
      filenames.push_back(fileEnt(lsdir, dent->d_name, dent->d_type));
    }
  }

  // Sort the files
  std::sort(filenames.begin(), filenames.end());

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

    switch(f.getType()) {
      case DT_DIR:
        f.setFmt(generalFormat[dirIndex]);
        break;
      default:
        // Handle reserved Filenames
        if (lookupByFilename(f)) break;

        // Handle files by extension
        if (lookupByExtension(f, base, ext)) break;
        
        // Should never get here
        assert(0);
    }
  }

  if (longFlag) {
    printList(filenames);
  } else {
    printColumns(filenames);
  }
}
