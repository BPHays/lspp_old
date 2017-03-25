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
  nFlags    = ft + 1
};
std::bitset<nFlags> flagSet;

bool lookupByFilename(fileEnt & f);
bool lookupByExtension(fileEnt & f);
void printColumns(std::vector<fileEnt> & filenames);
void printList(fileEnt & f);
void printList(std::vector<fileEnt> & filenames, unsigned char linkWidth);
void usage();
void setoption(struct option & op, const char * name, int has_arg, int * flag, int val);

#endif /* LSPP_HPP */
