#ifndef LSPP_HPP
#define LSPP_HPP

#include <bitset>

#include "fileEnt.hpp"

enum flags : int {
  help      = 0, 
  all       = 1, 
  longList  = 2, 
  almostAll = 3, 
  g         = 4, 
  o         = 5, 
  ft        = 6, 
  type      = 7, 
  nFlags    = type + 1
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

bool lookupByFilename(fileEnt & f);
bool lookupByExtension(fileEnt & f);

void printColumns(std::vector<fileEnt> & filenames);
void printList(fileEnt & f, unsigned char linkWidth);
void printList(std::vector<fileEnt> & filenames);

#endif /* LSPP_HPP */
