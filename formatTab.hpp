#ifndef FORMATTAB_HPP
#define FORMATTAB_HPP

/* fileType hiererchy */
const fileType file =         {"file", NULL};
  const fileType srcType =      {"src",     &file};
    const fileType webDevType =   {"webdev", &srcType};
  const fileType exeType =      {"exe",     &file};
  const fileType txtType =      {"txt",     &file};
  const fileType archiveType =  {"arch",    &file};
  const fileType imgType =      {"img",     &file};
  const fileType audioType =    {"audio",   &file};
  const fileType compiledType = {"comp",    &file};
  const fileType tmpType =      {"tmp",     &file};
  const fileType rcType =       {"rc", &file};
  const fileType dir =          {"dir", NULL};

/* general format entries accessed by index from enum */
const fileFmt generalFormat[] = {
  {":FILE",     "",      FILE_C    , &file},
  {":DIRECTORY","",      DIR_C     , &dir},
  {":DOTFILE",  "",      DOTFILE   , &file},
  {":BLK_DEV",  "\uf0a0", DEV_C     , &file},
  {":CHR_DEV",  "\uf1e4", DEV_C     , &file},
  {":SOCK",     "\uf0ee", SOCK_C    , &file},
  {":FIFO",     "\uf0ec", FIFO_C    , &file},
};

/* filename format entries must match full filename */
const fileNameFmt nameFormat[] = {
  {".git",      "",      TXT, &file, false},
  {".gitignore","",      TXT, &file, false},
  {"LICENSE",   "",      TXT, &file, false},
  {"Makefile",  "",      TXT, &file, false},
  {"README",    "",      TXT, &file, false},
  {"license",   "",      TXT, &file, false},
  {"readme",    "",      TXT, &file, false},
  {"tags",      "",      TXT, &file, false},
  {"\\..*rc",   "",      TXT, &rcType, true }
};

/* extension format entries must only match the file extension */
const fileFmt extFormat[] = {
  {"",          "",      EXE,       &exeType},
  {"exe",       "",      EXE,       &exeType},
  {"out",       "",      EXE,       &exeType},
  {"jar",       "",      EXE,       &exeType},

  {"7z",        "",      COMPRESS,  &archiveType},
  {"bz",        "",      COMPRESS,  &archiveType},
  {"bz2",       "",      COMPRESS,  &archiveType},
  {"gz",        "",      COMPRESS,  &archiveType},
  {"tar",       "",      COMPRESS,  &archiveType},
  {"xz",        "",      COMPRESS,  &archiveType},
  {"zip",       "",      COMPRESS,  &archiveType},

  {"ai",        "",      IMG,       &imgType},
  {"bmp",       "",      IMG,       &imgType},
  {"gif",       "",      IMG,       &imgType},
  {"ico",       "",      IMG,       &imgType},
  {"jpeg",      "",      IMG,       &imgType},
  {"jpg",       "",      IMG,       &imgType},
  {"png",       "",      IMG,       &imgType},
  {"psb",       "",      IMG,       &imgType},
  {"psd",       "",      IMG,       &imgType},
  {"ts",        "",      IMG,       &imgType},

  {"mp3",       "",      AUDIO,     &audioType},
  {"wav",       "",      AUDIO,     &audioType},
  {"mkv",       "",      VIDEO,     &audioType},

  {"doc",       "",      TXT,       &txtType},
  {"docx",      "",      TXT,       &txtType},
  {"odp",       "",      TXT,       &txtType},
  {"ods",       "",      TXT,       &txtType},
  {"odt",       "",      TXT,       &txtType},
  {"pdf",       "",      TXT,       &txtType},
  {"ppt",       "",      TXT,       &txtType},
  {"pptx",      "",      TXT,       &txtType},
  {"ttf",       "",      TXT,       &txtType},
  {"xls",       "",      TXT,       &txtType},
  {"xlsx",      "",      TXT,       &txtType},
  {"csv",       "",      TXT,       &txtType},
  {"dump",      "",      TXT,       &txtType},
  {"log",       "",      TXT,       &txtType},
  {"markdown",  "",      TXT,       &txtType},
  {"md",        "",      TXT,       &txtType},
  {"rss",       "",      TXT,       &txtType},
  {"t",         "",      TXT,       &txtType},
  {"txt",       "",      TXT,       &txtType},
  {"xml",       "",      TXT,       &txtType},
  {"yaml",      "",      TXT,       &txtType},
  
  {"conf",      "",      TXT,       &rcType},
  {"pro",       "",      TXT,       &rcType},

  {"lock",      "\uf023", TMP,       &tmpType},
  {"swp",       "",      TMP,       &tmpType},
  {"tmp",       "",      TMP,       &tmpType},

  {"class",     "",      COMPILED,  &compiledType},
  {"o",         "",      COMPILED,  &compiledType},

  {"asm",       "",      SRC,       &srcType},
  {"S",         "",      SRC,       &srcType},
  {"s",         "",      SRC,       &srcType},
  {"bash",      "",      SRC,       &srcType},
  {"bat",       "",      SRC,       &srcType},
  {"c",         "",      SRC,       &srcType},
  {"h",         "",      SRC,       &srcType},
  {"cc",        "",      SRC,       &srcType},
  {"c++",       "",      SRC,       &srcType},
  {"cpp",       "",      SRC,       &srcType},
  {"hh",        "",      SRC,       &srcType},
  {"hpp",       "",      SRC,       &srcType},
  {"inl",       "",      SRC,       &srcType},
  {"clj",       "",      SRC,       &srcType},
  {"cljc",      "",      SRC,       &srcType},
  {"cljs",      "",      SRC,       &srcType},
  {"coffee",    "",      SRC,       &srcType},
  {"cp",        "",      SRC,       &srcType},
  {"csh",       "",      SRC,       &srcType},
  {"css",       "",      SRC,       &srcType},
  {"cxx",       "",      SRC,       &srcType},
  {"d",         "",      SRC,       &srcType},
  {"dart",      "",      SRC,       &srcType},
  {"db",        "",      SRC,       &srcType},
  {"diff",      "",      SRC,       &srcType},
  {"edn",       "",      SRC,       &srcType},
  {"ejs",       "",      SRC,       &srcType},
  {"erl",       "",      SRC,       &srcType},
  {"f#",        "",      SRC,       &srcType},
  {"fish",      "",      SRC,       &srcType},
  {"fs",        "",      SRC,       &srcType},
  {"fsi",       "",      SRC,       &srcType},
  {"fsscript",  "",      SRC,       &srcType},
  {"fsx",       "",      SRC,       &srcType},
  {"go",        "",      SRC,       &srcType},
  {"hbs",       "",      SRC,       &srcType},
  {"hrl",       "",      SRC,       &srcType},
  {"hs",        "",      SRC,       &srcType},
  {"htm",       "",      SRC,       &srcType},
  {"html",      "",      SRC,       &srcType},
  {"ini",       "",      SRC,       &srcType},
  {"java",      "",      SRC,       &srcType},
  {"jl",        "",      SRC,       &srcType},
  {"js",        "",      SRC,       &webDevType},
  {"json",      "",      SRC,       &srcType},
  {"jsx",       "",      SRC,       &srcType},
  {"less",      "",      SRC,       &srcType},
  {"lhs",       "",      SRC,       &srcType},
  {"lua",       "",      SRC,       &srcType},
  {"ml",        "λ",      SRC,       &srcType},
  {"mli",       "λ",      SRC,       &srcType},
  {"sml",        "λ",     SRC,       &srcType},
  {"mustache",  "",      SRC,       &srcType},
  {"php",       "",      SRC,       &srcType},
  {"pl",        "",      SRC,       &srcType},
  {"pm",        "",      SRC,       &srcType},
  {"py",        "",      SRC,       &srcType},
  {"pyc",       "",      SRC,       &srcType},
  {"pyd",       "",      SRC,       &srcType},
  {"pyo",       "",      SRC,       &srcType},
  {"rb",        "",      SRC,       &srcType},
  {"rlib",      "",      SRC,       &srcType},
  {"rs",        "",      SRC,       &srcType},
  {"scala",     "",      SRC,       &srcType},
  {"scm",       "λ",      SRC,       &srcType},
  {"scss",      "",      SRC,       &srcType},
  {"sh",        "",      SRC,       &srcType},
  {"slim",      "",      SRC,       &srcType},
  {"sln",       "",      SRC,       &srcType},
  {"sql",       "",      SRC,       &srcType},
  {"styl",      "",      SRC,       &srcType},
  {"suo",       "",      SRC,       &srcType},
  {"tex",       "",      SRC,       &srcType},
  {"twig",      "",      SRC,       &srcType},
  {"vim",       "",      SRC,       &srcType},
  {"xul",       "",      SRC,       &srcType},
  {"yml",       "",      SRC,       &srcType},
  {"zsh",       "",      SRC,       &srcType}
};
#endif /* FORMATTAB_HPP */
