#ifndef LSPP_HPP
#define LSPP_HPP

#include <bitset>

#include "fileEnt.hpp"

class argSet {
  public: 
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
      noFmt       = 15,     // Dont' color format the output
      filePerLine = 15,     // list a single file per line
      color       = 16,     // colorize the output
      // TODO icon flag should turn off suffix icons as well
      icon        = 17,     // display icons with the file
      recursive   = 18,     // recursively print subdirectories
      tree        = 19,     
      perm        = 20,     // color the files by the user's file permissions   
      nFlags      = 64
    };

  // members
  private: 
    std::bitset<nFlags> _flagBits;
    std::string         _lsdir;

  //methods
  private:
  public:
    /*
    flagSet();
    ~flagSet();
    */

    // getters
    inline       bool          getFlag(flags flag) const { return _flagBits.test(flag); };
    inline const std::string & getLsDir()          const { return _lsdir; }

    // setters
    inline void setFlag(flags flag, bool val = true) { _flagBits.set(flag, val); }
    inline void setLsDir(std::string lsdir)          { _lsdir = lsdir; }
};

class listTree {
  fileEnt              node;
  std::vector<fileEnt> children;
};

argSet args;

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

// Helper methods for printing
void printColumns(std::vector<fileEnt> & filenames);
void printLongList(fileEnt & f, unsigned char linkWidth);
void printLongList(std::vector<fileEnt> & filenames);
void printList(std::vector<fileEnt> & filenames);

#endif /* LSPP_HPP */
