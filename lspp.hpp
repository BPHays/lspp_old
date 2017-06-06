#ifndef LSPP_HPP
#define LSPP_HPP

#include "fileEnt.hpp"
#include "argSet.hpp"

class listTree {
  fileEnt              node;
  std::vector<fileEnt> children;
};

extern argSet * args;

void usage();
void parseArgs(int argc, char * const * argv);
void getFiles(const std::string lsdir, std::vector<fileEnt> & filenames);
void getFormatStyle(std::vector<fileEnt> & filenames);
void fileterFiles(std::vector<fileEnt> & filenames);
void sortFiles(std::vector<fileEnt> & filenames);
void printFiles(std::vector<fileEnt> & filenames);
void printByType(std::vector<fileEnt> & filenames);

void listDirectory(std::string lsdir);

// Helper functions for finding the file format and type
bool lookupByFilename(fileEnt & f);
bool lookupByExtension(fileEnt & f);

#endif /* LSPP_HPP */
