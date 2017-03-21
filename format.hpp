#ifndef FORMAT_HPP
#define FORMAT_HPP

#define ESC         "\033["
#define COLOR_ESC(c) ESC"38;5;" c "m"

#define BLUE    "159"
#define GREEN   "85"
#define YELLOW  "229"
#define LTGREY  "252"
#define DKGREY  "244"
#define INDIGO  "105"
#define ORANGE  "216"
#define RED     "160"

#define FILE_C    COLOR_ESC(ORANGE)
#define DIR_C     COLOR_ESC(BLUE)
#define DOTFILE   COLOR_ESC(INDIGO)
#define SRC       COLOR_ESC(GREEN)
#define EXE       COLOR_ESC(GREEN)
#define COMPRESS  COLOR_ESC(YELLOW)
#define COMPILED  COLOR_ESC(GREEN)
#define IMG       COLOR_ESC(LTGREY)
#define TXT       COLOR_ESC(INDIGO)
#define TMP       COLOR_ESC(DKGREY)
#define AUDIO     COLOR_ESC(LTGREY)
#define VIDEO     COLOR_ESC(LTGREY)

#define BOLD      ESC"1m"
#define NO_EMPH   ESC"0m"

#define LINK_ICON     "\uf0c1"
#define VISIBLE_ICON  "\uf06e"

enum generalFormatIndex : int {fileIndex = 0, dirIndex = 1, dotfileIndex = 2, devIndex = 3, sockIndex = 4, fifoIndex = 5};
enum format : int {key = 0, icon = 1, color = 2};

extern std::string generalFormat[][3];
extern std::string generalFormat[][3];
extern std::string nameFormat[][3];
extern std::string emphasis[];

#endif /* FORMAT_HPP */
