#ifndef FORMATTAB_HPP
#define FORMATTAB_HPP

std::string generalFormat[][3] = {
  {":FILE",     "",      FILE_C    },
  {":DIRECTORY","",      DIR_C     },
  {":DOTFILE",  "",      DOTFILE   },
  {":DEV",      "\uf0a0", DIR_C     },
  {":SOCK",     "\uf0ee", DIR_C     },
  {":FIFO",     "\uf0ec", DIR_C     },
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

  {"asm",       "",      SRC       },
  {"S",         "",      SRC       },
  {"s",         "",      SRC       },
  {"bash",      "",      SRC       },
  {"bat",       "",      SRC       },
  {"c",         "",      SRC       },
  {"h",         "",      SRC       },
  {"cc",        "",      SRC       },
  {"c++",       "",      SRC       },
  {"cpp",       "",      SRC       },
  {"hh",        "",      SRC       },
  {"hpp",       "",      SRC       },
  {"inl",       "",      SRC       },
  {"clj",       "",      SRC       },
  {"cljc",      "",      SRC       },
  {"cljs",      "",      SRC       },
  {"coffee",    "",      SRC       },
  {"cp",        "",      SRC       },
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
  {"hbs",       "",      SRC       },
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
  {"sml",        "λ",     SRC       },
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

#endif /* FORMATTAB_HPP */
