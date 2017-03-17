#ifndef LSPP_HPP
#define LSPP_HPP

#include <unordered_map>
#include <string.h>

/*
FILENAMES = {
              "Makefile":   [u"", TXT],
              "README":     [u"\uf128", TXT],
              "readme":     [u"\uf128", TXT],
              "LICENSE":    [u"", TXT],
              "license":    [u"", TXT],
              ".gitignore": [u"", TXT],
              ".git":       [u"", TXT],
              "tags":       [u"\uf02c", TXT],
}
*/

#define ESC "\033["
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

enum FormatList {EXT_LIST, FILE_LIST};

#define FILE_FMT 0
#define DIR_FMT  1
#define DOT_FMT  2

enum generalFormatIndex : int {fileIndex = 0, dirIndex = 1, dotfileIndex = 2};
std::string generalFormat[][3] = {
  {":FILE",     "",      FILE_C    },
  {":DIRECTORY","",      DIR_C     },
  {":DOTFILE",  "",      DOTFILE   }
};

std::string nameFormat[][3] = {
  {".git",      "",      TXT       },
  {".gitignore","",      TXT       },
  {"LICENSE",   "",      TXT       },
  {"Makefile",  "",      TXT       },
  {"README",    "",      TXT       },
  {"license",   "",      TXT       },
  {"readme",    "",      TXT       },
  {"tags",      "",      TXT       }
};

enum format : int {key = 0, icon = 1, color = 2};

// File extension descriptions.
// Format: "EXTENSION": [u"ICON","COLOR CODE"]
std::string extFormat[][3] = {
  {"",          "",      EXE       },
  {"exe",       "",      EXE       },
  {"out",       "",      EXE       },

  {"7z",        "",      COMPRESS  },
  {"bz",        "",      COMPRESS  },
  {"bz2",       "",      COMPRESS  },
  {"gz",        "",      COMPRESS  },
  {"tar",       "",      COMPRESS  },
  {"xz",        "",      COMPRESS  },
  {"zip",       "",      COMPRESS  },

  {"ai",        "",      IMG       },
  {"bmp",       "",      IMG       },
  {"gif",       "",      IMG       },
  {"ico",       "",      IMG       },
  {"jpeg",      "",      IMG       },
  {"jpg",       "",      IMG       },
  {"png",       "",      IMG       },
  {"psb",       "",      IMG       },
  {"psd",       "",      IMG       },
  {"ts",        "",      IMG       },

  {"mp3",       "",      AUDIO     },
  {"wav",       "",      AUDIO     },
  {"mkv",       "",      VIDEO     },

  {"doc",       "",      TXT       },
  {"docx",      "",      TXT       },
  {"odp",       "",      TXT       },
  {"ods",       "",      TXT       },
  {"odt",       "",      TXT       },
  {"pdf",       "",      TXT       },
  {"ppt",       "",      TXT       },
  {"pptx",      "",      TXT       },
  {"ttf",       "",      TXT       },
  {"xls",       "",      TXT       },
  {"xlsx",      "",      TXT       },
  {"swp",       "",      TMP       },
  {"tmp",       "",      TMP       },
  {"conf",      "",      TXT       },
  {"csv",       "",      TXT       },
  {"dump",      "",      TXT       },
  {"log",       "",      TXT       },
  {"markdown",  "",      TXT       },
  {"md",        "",      TXT       },
  {"rss",       "",      TXT       },
  {"t",         "",      TXT       },
  {"txt",       "",      TXT       },

  {"class",     "",      COMPILED  },
  {"o",         "",      COMPILED  },

  {"S",         "",      SRC       },
  {"asm",       "",      SRC       },
  {"bash",      "",      SRC       },
  {"bat",       "",      SRC       },
  {"c",         "",      SRC       },
  {"c++",       "",      SRC       },
  {"cc",        "",      SRC       },
  {"clj",       "",      SRC       },
  {"cljc",      "",      SRC       },
  {"cljs",      "",      SRC       },
  {"coffee",    "",      SRC       },
  {"cp",        "",      SRC       },
  {"cpp",       "",      SRC       },
  {"csh",       "",      SRC       },
  {"css",       "",      SRC       },
  {"cxx",       "",      SRC       },
  {"d",         "",      SRC       },
  {"dart",      "",      SRC       },
  {"db",        "",      SRC       },
  {"diff",      "",      SRC       },
  {"edn",       "",      SRC       },
  {"ejs",       "",      SRC       },
  {"erl",       "",      SRC       },
  {"f#",        "",      SRC       },
  {"fish",      "",      SRC       },
  {"fs",        "",      SRC       },
  {"fsi",       "",      SRC       },
  {"fsscript",  "",      SRC       },
  {"fsx",       "",      SRC       },
  {"go",        "",      SRC       },
  {"h",         "",      SRC       },
  {"hbs",       "",      SRC       },
  {"hh",        "",      SRC       },
  {"hpp",       "",      SRC       },
  {"hrl",       "",      SRC       },
  {"hs",        "",      SRC       },
  {"htm",       "",      SRC       },
  {"html",      "",      SRC       },
  {"ini",       "",      SRC       },
  {"java",      "",      SRC       },
  {"jl",        "",      SRC       },
  {"js",        "",      SRC       },
  {"json",      "",      SRC       },
  {"jsx",       "",      SRC       },
  {"less",      "",      SRC       },
  {"lhs",       "",      SRC       },
  {"lua",       "",      SRC       },
  {"ml",        "λ",      SRC       },
  {"mli",       "λ",      SRC       },
  {"mustache",  "",      SRC       },
  {"php",       "",      SRC       },
  {"pl",        "",      SRC       },
  {"pm",        "",      SRC       },
  {"py",        "",      SRC       },
  {"pyc",       "",      SRC       },
  {"pyd",       "",      SRC       },
  {"pyo",       "",      SRC       },
  {"rb",        "",      SRC       },
  {"rlib",      "",      SRC       },
  {"rs",        "",      SRC       },
  {"s",         "",      SRC       },
  {"scala",     "",      SRC       },
  {"scm",       "λ",      SRC       },
  {"scss",      "",      SRC       },
  {"sh",        "",      SRC       },
  {"slim",      "",      SRC       },
  {"sln",       "",      SRC       },
  {"sql",       "",      SRC       },
  {"styl",      "",      SRC       },
  {"suo",       "",      SRC       },
  {"tex",       "",      SRC       },
  {"twig",      "",      SRC       },
  {"vim",       "",      SRC       },
  {"xul",       "",      SRC       },
  {"yml",       "",      SRC       },
  {"zsh",       "",      SRC       }
};

class fileEnt {
  private:
    std::string   _path;
    std::string   _name;
    unsigned char _type;
    const std::string * _fmt;
    struct stat   _stat;
    bool          _haveStats;

  private:
    static std::unordered_map<uid_t, std::string> userNames;
    static std::unordered_map<gid_t, std::string> groupNames;

  public: 
    fileEnt(std::string dir, std::string name, unsigned char type);
    virtual ~fileEnt();

    // Getters
    std::string getName();
    unsigned char getType();

    // Setters
    void setFmt(std::string * fmt);

    // Methods
    std::string   formatted(size_t length);
    const std::string & getColor();
    const std::string & getIcon();
    std::string   getPermissionString();
    std::string & getOwnerName();
    std::string & getGroupName();
    std::string   getSize();
    std::string   getTimestamp();

    inline const struct stat & getStat();

    static std::string & pad(std::string & str, size_t length);
    static void padGroupNames(size_t length);
    static void padUserNames(size_t length);

  private:
    void statFile();

  public:
    friend bool operator<(fileEnt x, fileEnt y) {
      const char * xc = x._name.c_str();
      const char * yc = y._name.c_str();
      if (*xc == '.') ++xc;
      if (*yc == '.') ++yc;
      return strcasecmp(xc, yc) < 0;
    }
};


#endif /* LSPP_HPP */
