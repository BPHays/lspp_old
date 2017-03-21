#ifndef LSPP_HPP
#define LSPP_HPP

#include "fileEnt.hpp"

bool lookupByFilename(fileEnt & f);
bool lookupByExtension(fileEnt & f);
void printColumns(std::vector<fileEnt> & filenames);
void printList(fileEnt & f);
void printList(std::vector<fileEnt> & filenames, unsigned char linkWidth);
void usage();
void setoption(struct option & op, const char * name, int has_arg, int * flag, int val);

#endif /* LSPP_HPP */
