#ifndef FORMAT_HPP
#define FORMAT_HPP

#define ESC         "\033["
#define COLOR_ESC(c) ESC"38;5;" c "m"

#define RED     "09"
#define GREEN   "10"
#define YELLOW  "11"
#define BLUE    "12"
#define PURPLE  "13"
#define CYAN    "14"
#define WHITE   "7"

#define LTGREY  "252"
#define DKGREY  "244"
#define INDIGO  "105"
#define ORANGE  "216"

// File permission colors
const std::string RWX_PERM = COLOR_ESC(WHITE);
const std::string W_PERM   = COLOR_ESC(YELLOW);
const std::string RW_PERM  = COLOR_ESC(GREEN);
const std::string R_PERM   = COLOR_ESC(BLUE);
const std::string RX_PERM  = COLOR_ESC(PURPLE);
const std::string X_PERM   = COLOR_ESC(RED);
const std::string WX_PERM  = COLOR_ESC(ORANGE);
const std::string NO_PERM  = COLOR_ESC(DKGREY);

// Generic
#define FILE_C    COLOR_ESC(ORANGE)
#define DIR_C     COLOR_ESC(GREEN)
#define DEV_C     COLOR_ESC(PURPLE)
#define SOCK_C    COLOR_ESC(PURPLE)
#define FIFO_C    COLOR_ESC(PURPLE)
#define DOTFILE   COLOR_ESC(INDIGO)

// Bins
#define EXE       COLOR_ESC(RED)

// Code
#define SRC       COLOR_ESC(BLUE)
#define COMPILED  COLOR_ESC(PURPLE)

// Basic Text
#define TXT       COLOR_ESC(CYAN)

// Archives
#define COMPRESS  COLOR_ESC(YELLOW)

// Temp
#define TMP       COLOR_ESC(DKGREY)

// Media
#define IMG       COLOR_ESC(LTGREY)
#define AUDIO     COLOR_ESC(LTGREY)
#define VIDEO     COLOR_ESC(LTGREY)

#define BOLD      ESC"1m"
#define NO_EMPH   ESC"0m"

#define LINK_ICON     "\uf0c1"
#define VISIBLE_ICON  "\uf06e"

// Tree characters
const std::string TREE_COLOR        = DIR_C;
const std::string TREE_ELBOW        = "\u2514";
const std::string TREE_BRANCH_RIGHT = "\u251c";
const std::string TREE_BRANCH_DOWN  = "\u252c";
const std::string TREE_VERTICAL     = "\u2502";
const std::string TREE_HORIZONTAL   = "\u2500";
const std::string TREE_EMPTY_SPACE  = " ";

enum generalFormatIndex : int {
  fileIndex = 0, 
  dirIndex = 1, 
  dotfileIndex = 2, 
  blkDevIndex = 3, 
  chrDevIndex = 4, 
  sockIndex = 5, 
  fifoIndex = 6
};

// TODO generalize fileType and fileFmt structs to use format
// this allows setting a fileType default format to use, to reduce
// duplication
/*
struct format {
  std::string icon;
  std::string fmt;
};
*/

struct fileType /*: format*/ {
  std::string typeName;
  const fileType   *parent;
  /*
  const format * format;
  fileType(std::string name, const fileType * parent, std::string icon = "", std::string fmt = "") : format(icon, fmt), name(name), parent(parent){};
  */
};

struct fileFmt /*: format*/ {
  std::string  name;
  std::string  icon;
  std::string  fmt;
  const fileType    *parent;
  fileFmt(std::string name, std::string icon, std::string fmt, const fileType *parent) : name(name), icon(icon), fmt(fmt), parent(parent){};
  /*
  const format * format;
  fileFmt(std::string name, std::string icon, std::string fmt, const fileType *parent) : format(icon, fmt), name(name), icon(icon), fmt(fmt), parent(parent){};
  */
};

struct fileNameFmt : fileFmt {
  bool reg;
  fileNameFmt(std::string name, std::string icon, std::string fmt, const fileType *parent, bool reg) : fileFmt(name, icon, fmt, parent), reg(reg){};
};

extern const std::string emphasis[];

#include "formatTab.hpp"

#endif /* FORMAT_HPP */
