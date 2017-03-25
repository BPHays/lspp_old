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

#define LTGREY  "252"
#define DKGREY  "244"
#define INDIGO  "105"
#define ORANGE  "216"

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

enum generalFormatIndex : int {fileIndex = 0, dirIndex = 1, dotfileIndex = 2, blkDevIndex = 3, chrDevIndex = 4, sockIndex = 5, fifoIndex = 6};
enum format : int {key = 0, icon = 1, color = 2};

struct fileType {
  std::string typeName;
  const fileType   *parent;
};

struct fileFmt {
  std::string  name;
  std::string  icon;
  std::string  fmt;
  const fileType    *parent;
};

/*
extern const fileFmt generalFormat[];
extern const fileFmt generalFormat[];
extern const fileFmt nameFormat[];
*/

extern const std::string emphasis[];

#include "formatTab.hpp"

#endif /* FORMAT_HPP */
