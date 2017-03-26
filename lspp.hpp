#ifndef LSPP_HPP
#define LSPP_HPP

#include <bitset>

#include "fileEnt.hpp"

enum flags : int {
  help        =  0,     // print usage 
  all         =  1,     // show hidden files too
  longList    =  2,     // list with file metadata
  almostAll   =  3,     // show hidden files except ., ..
  noOwner     =  4,     // show group, but not owner
  noGroup     =  5,     // show owner, but not group
  ft          =  6,     // filter by fileType
  type        =  7,     // list grouped by fileType
  sortTime    =  8,     // sort by mod time
  sortSize    =  9,     // sort by size
  sortInDir   = 10,     // sort by order in directory
  sortExt     = 11,     // sort by extension
  reverse     = 12,     // reverse the sorting
  author      = 13,     // show auther (same as owner on linux)
  noFmt       = 14,     // Dont' color format the output
  filePerLine = 15,     // list a single file per line
  nFlags      = 64
};
std::bitset<nFlags> flagSet;

void usage();
std::string parseArgs(int argc, char * const * argv);
void getFiles(const std::string lsdir, std::vector<fileEnt> & filenames);
void getFormatStyle(std::vector<fileEnt> & filenames);
void fileterFiles(std::vector<fileEnt> & filenames);
void sortFiles(std::vector<fileEnt> & filenames);
void printFiles(std::vector<fileEnt> & filenames);
void printByType(std::vector<fileEnt> & filenames);

// Helper functions for finding the file format and type
bool lookupByFilename(fileEnt & f);
bool lookupByExtension(fileEnt & f);

// Helper methods for printing
void printColumns(std::vector<fileEnt> & filenames);
void printLongList(fileEnt & f, unsigned char linkWidth);
void printLongList(std::vector<fileEnt> & filenames);
void printList(std::vector<fileEnt> & filenames);

#endif /* LSPP_HPP */
