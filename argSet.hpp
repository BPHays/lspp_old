#ifndef ARGSET_HPP
#define ARGSET_HPP

#include <bitset>
#include <getopt.h>

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
      sortAccTime = 21,     // sort by access time
      quote       = 22,     // quote strings
      nFlags      = 64
    };

  private:
    enum longOptIndex : short {
      opt_ft = 128, 
      opt_type = 129, 
      opt_author = 130, 
      opt_noFmt = 131, 
      opt_color = 132, 
      opt_icon = 133, 
      opt_tree = 134, 
      opt_help = 135, 
      opt_perm = 136
    };

  // members
  private: 
    std::bitset<nFlags> _flagBits;
    std::string         _lsdir;
    std::string         _listType;

  //methods
  private:
  public:
    argSet(int argc, char * const * argv);

    // getters
    inline       bool          getFlag(flags flag) const { return _flagBits.test(flag); };
    inline const std::string & getLsDir()          const { return _lsdir; }
    inline const std::string & getListType()       const { return _listType; }

    // setters
    inline void setFlag(flags flag, bool val = true) { _flagBits.set(flag, val); }
    inline void setLsDir(std::string lsdir)          { _lsdir = lsdir; }
    inline void setListType(std::string listType)    { _listType = listType; }
};

#endif /* ARGSET_HPP */
