#include <vector>
#include <functional>
#include <map>
#include <iostream>
#include <algorithm>

#include <unistd.h>
#include <sys/ioctl.h>

#include "fileEnt.hpp"
#include "printFiles.hpp"
#include "argSet.hpp"
#include "lspp.hpp"

static void printPadding(size_t len);

/**
 * @brief lambda to short print a file
 *
 * @param f the file to print
 */
const std::function<void(fileEnt const &)> printShortFormat = 
           [](auto & f) { 
            if (args->getFlag(argSet::flags::color)) {
              std::cout << f.getColor();
            }
            if (args->getFlag(argSet::flags::icon)) {
              std::cout << f.getIcon() << " ";
            }
            if (args->getFlag(argSet::flags::quote)) {
              std::cout << "\"";
            }
            std::cout << f.getName();
            if (args->getFlag(argSet::flags::quote)) {
              std::cout << "\"";
            }
            std::cout << f.getSuffixIcons();
           };

const std::function<void(fileEnt const &)> printShortLine =
           [](auto & f) {
            printShortFormat(f);
            std::cout << std::endl;};

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
static bool fitsInNRows(
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
      suffixLen = 2 * ent.getNSuffixIcons();
      colWidth = std::max(colWidth, ent.getName().length() + suffixLen + padding);
    }
    colWidths.push_back(colWidth);
    totalSize += colWidth;
  }
  return totalSize <= width;
}

/**
 * @brief generate file list by filetype and print them separately
 *
 * @param filenames list containing the filenames to print
 */
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

ssize_t getShortLength(fileEnt & f) {
  size_t suffixLen = 2 * f.getNSuffixIcons();
  return f.getName().length() + suffixLen;
}

/**
 * @brief print the file padded to fit its column's width
 *
 * @param f file to print
 * @param length width to pad to
 */
static void printFormatColumn(fileEnt & f, size_t length) {
  ssize_t padLen = length - getShortLength(f);
  printShortFormat(f);
  if (padLen >= 0) {
    printPadding(padLen);
  }
}

/*
void rightSizeColumns(std::vector<size_t> lengths, std::vector<size_t> & colWidths) {
  // TODO right size the columns of elements each of length lenthgs[i]. fill in the 
  // widths of the columns in the colWidths vector.
}
*/

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
          printFormatColumn(filenames[col * rows + row], colWidths[col] - 2);
        } else {
          printFormatColumn(filenames[col * rows + row], 0);
        }
    }
    std::cout << std::endl;
  }
}

/**
 * @brief create a lambda to print the files in long format
 *
 * @param linksMax the width of the hard links column in characters
 *
 * @return lambda taking a file to print
 */
const std::function<void(fileEnt & f)> printLongFormat(size_t linksMax) {
  return [linksMax](auto & f) { 
              if (!args->getFlag(argSet::flags::noFmt) &&
                  args->getFlag(argSet::flags::color)) {
                std::cout << f.getEmphasis();
                if (args->getFlag(argSet::flags::perm)) {
                  std::cout << f.getPermColor();
                } else {
                  std::cout << f.getColor();
                }
              }
              std::cout << f.getPermissionString() << " ";
              std::cout << f.getRefCnt(linksMax) << " ";
              if (!args->getFlag(argSet::flags::noOwner)) {
                std::cout << f.getOwnerName() << " ";
              }
              if (!args->getFlag(argSet::flags::noGroup)) {
                std::cout << f.getGroupName() << " ";
              }
              if (args->getFlag(argSet::flags::author)) {
                std::cout << f.getOwnerName() << " ";
              }
              std::cout << f.getSizeStr() << " ";
              std::cout << f.getTimestampStr() << " ";
              if (args->getFlag(argSet::flags::icon)) {
                std::cout << f.getIcon() << " ";
              }
              if (args->getFlag(argSet::flags::quote)) {
                std::cout << "\"";
              }
              std::cout << f.getName() ;
              if (args->getFlag(argSet::flags::quote)) {
                std::cout << "\"";
              }
              std::cout << f.getSuffixIcons();
              if (f.isLink()) {
                std::cout << " " << f.getTarget();
              }
              std::cout << std::endl; };
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
  for_each(filenames.begin(), filenames.end(), printLongFormat(linksMax));
} 

static void printPadding(size_t len) {
  static std::string pad(256, ' ');
  pad.resize(len, ' ');
  std::cout << pad;
}

/**
 * @brief simply print each file on its own line
 *
 * @param filenames the list of files to print
 */
void printList(std::vector<fileEnt> & filenames) {
  for_each(filenames.begin(), filenames.end(), printShortLine);
}

/**
 * @brief print the files as a tree recursing on subdirectories
 *
 * @param filenames list of top level filenames to print
 */
void printTree(std::vector<fileEnt> & filenames) {
  // Keep track of if each directory traversed has more elements
  static std::vector<bool> indent = { true };

  for (auto f = filenames.begin(); f != filenames.end(); f++) {
    std::cout << DIR_C;
    for (auto b = indent.begin(); b != indent.end(); b++) {
      if (*b) { 
        if (b == indent.end() - 1) {
          if (f == filenames.end() - 1) {
            std::cout << TREE_ELBOW;
          } else {
            std::cout << TREE_BRANCH_RIGHT;
          }
        } else {
          std::cout << TREE_VERTICAL; 
        }
      } else { 
        std::cout << TREE_EMPTY_SPACE; 
      }
    }
    if (args->getFlag(argSet::flags::tree)) {
      if (f->isDir()) {
        std::cout << TREE_BRANCH_DOWN;
      } else {
        std::cout << TREE_HORIZONTAL;
      }
    }
    printShortLine(*f);
    if (f->isDir()) {
      if (f == filenames.end() - 1) {
        indent.pop_back();
        indent.push_back(false);
      }
      indent.push_back(true);
      listDirectory(f->getPath());
      indent.pop_back();
    }
  }
}

/**
 * @brief print the list of files according to the command line flags
 *
 * @param filenames the list of files to be printed
 */
void printFiles(std::vector<fileEnt> & filenames) {
  if (args->getFlag(argSet::flags::longList) || 
      args->getFlag(argSet::flags::noGroup) || 
      args->getFlag(argSet::flags::noOwner)) {
    // Print in long list format
    printLongList(filenames);
  } else {
    if (isatty(1) && !( args->getFlag(argSet::flags::filePerLine) ||
        args->getFlag(argSet::flags::tree))) {
      // Print in compact columnar format
      printColumns(filenames);
    } else {
      if (args->getFlag(argSet::flags::tree)) {
        printTree(filenames);
      } else {
        printList(filenames);
      }
    }
  }
}

