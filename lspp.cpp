#include <iostream>
#include <iomanip>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>
#include <dirent.h>
#include <assert.h>

#include "lspp.hpp"

std::unordered_map<uid_t, std::string> fileEnt::userNames;
std::unordered_map<gid_t, std::string> fileEnt::groupNames;

fileEnt::fileEnt(std::string dir, std::string name, unsigned char type) :
  _path(dir + "/" + name),
  _name(name),
  _type(type),
  _haveStats(false)
  {}

fileEnt::~fileEnt(){}

std::string fileEnt::getName(){ return _name; }
unsigned char fileEnt::getType(){ return _type; }

/**
 * @brief stat the file or return the cached copy of the file's stats
 *
 * @return the file entry's stat struct from the stat syscall
 */
const struct stat & fileEnt::getStat() {
  if (!_haveStats) {
    stat(_path.c_str(), &_stat);
    _haveStats = true;
  }
  return _stat;
}

/**
 * @brief set the format entry to use
 *
 * @param fmt the format entry to use
 */
void fileEnt::setFmt(std::string * fmt) { 
  _fmt = fmt;
}

/**
 * @brief format string with colorized name and icon
 *
 * @param length length to pad formatted string to
 *
 * @return the formatted string
 */
std::string fileEnt::formatted(size_t length) {
  return _fmt[color] + _fmt[icon] + " " + pad(_name, length);
}

/**
 * @brief get the entries color format field
 *
 * @return the entries color format field
 */
const std::string & fileEnt::getColor() {
  return _fmt[color];
}

/**
 * @brief get the entries icon format field
 *
 * @return the entries icon format field
 */
const std::string & fileEnt::getIcon() {
  return _fmt[icon];
}

/**
 * @brief pad a string to length with space chars
 *
 * @param str string to pad
 * @param length lenght to pad to
 *
 * @return a reference to the padded string
 */
inline std::string & fileEnt::pad(std::string & str, size_t length) {
  str.resize(length, ' ');
  return str;
}

/**
 * @brief pad all of the groups names to length
 *
 * @param length length to pad to
 */
inline void fileEnt::padGroupNames(size_t length) {
  for (auto & elem : groupNames) {
    pad(elem.second, length);
  }
}

/**
 * @brief pad all of the users names to length
 *
 * @param length length to pad to
 */
inline void fileEnt::padUserNames(size_t length) {
  for (auto & elem : userNames) {
    pad(elem.second, length);
  }
} 

/**
 * @brief perform format lookup by filename
 *
 * @param f file entry to look up
 *
 * @return true if the format was set
 */
bool lookupByFilename(fileEnt & f) {
  std::string name = f.getName();
  for(std::size_t i = 0; i < sizeof(nameFormat)/sizeof(*nameFormat); i++) {
    std::string * entry = nameFormat[i];
    if (name == entry[key]) {
      f.setFmt(entry);
      return true;
    }
  }
  return false;
}

/**
 * @brief perform format lookup by extension
 *
 * @param f file entry to look up
 * @param baseName file's basename
 * @param extension file's extension
 *
 * @return true if the format was set
 */
bool lookupByExtension(fileEnt & f, std::string baseName, std::string extension) {
  if (baseName[0] == '.') {
    // Check if file is a dotfile
    f.setFmt(generalFormat[dotfileIndex]);
    return true;
  } else {
    // Find file extension
    for(std::size_t i = 0; i < sizeof(extFormat)/sizeof(*extFormat); i++) {
      std::string * entry = extFormat[i];
      if (extension == entry[key]) {
        f.setFmt(entry);
        return true;
      }
    }
  }
  // Use the default if extension not found
  f.setFmt(generalFormat[fileIndex]);
  return true;
}

/**
 * @brief print the list of file entries in right sized columns
 *
 * @param filenames list of file entries to print
 */
void printColumns(std::vector<fileEnt> & filenames) {
  std::vector<size_t> colWidths;
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  unsigned short width = w.ws_col;

  size_t rows = 0;
  size_t colWidth;
  int totalSize;
  do {
    ++rows;
    totalSize = 0;
    colWidths.clear();
    for (size_t col = 0; col < (filenames.size() + rows - 1) / rows; ++col) {
      colWidth = 0;
      for (size_t row = 0; row < rows; ++row) {
        if (col * rows + row + 1 > filenames.size()) { 
          break; 
        }
        colWidth = std::max(colWidth, filenames[col * rows + row].getName().length() + 4);
      }
      colWidths.push_back(colWidth);
      totalSize += colWidth;
    }
  } while(totalSize > width);

  std::string fmt;
  for (size_t row = 0; row < rows; ++row) {
    for (size_t col = 0; col < (filenames.size() + rows - 1) / rows; ++col) {
        if (col * rows + row + 1 > filenames.size()) { 
          break; 
        }
        //fmt = filenames[col * rows + row].formatted(col);
        //std::cout << fmt;
        std::cout << filenames[col * rows + row].formatted(colWidths[col] - 2);
    }
    std::cout << std::endl;
  }
}

/**
 * @brief extract the permissions bits fromt the type and stat mode
 *
 * @return human readable permissions bits string
 */
std::string fileEnt::getPermissionString() {
  std::string permStr = "";

  // Set the type char
  switch(_type) {
    case DT_LNK:
      permStr += 'l';
      break;
    case DT_DIR:
      permStr += 'd';
      break;
    case DT_REG:
      // fallthrough
    case DT_UNKNOWN:
      // fallthrough
    default:
      permStr += '-';
  }

  // Set the permissions chars
  char permChars[] = {'x', 'w', 'r'};
  mode_t st_mode = getStat().st_mode;
  for (int permSet = 2; permSet >= 0; --permSet) {
    for (int permBit = 2; permBit >= 0; --permBit) {
      if ((st_mode >> (permSet * 3 + permBit)) & 0x1) {
        permStr += permChars[permBit];
      } else {
        permStr +=  '-';
      }
    }
  }

  return permStr;
}

/**
 * @brief either look up the user in the static map or via syscall
 *
 * @return the file owner's name
 */
std::string & fileEnt::getOwnerName() {
  uid_t id = getStat().st_uid;
  if (userNames.find(id) == userNames.end()) {
    userNames[id] = std::string(getpwuid(getStat().st_uid)->pw_name);
  }
  return userNames[id];
}

/**
 * @brief either look up the group in the static map or via syscall
 *
 * @return the file's group name
 */
std::string & fileEnt::getGroupName() {
  gid_t id = getStat().st_gid;
  if (groupNames.find(id) == groupNames.end()) {
    groupNames[id] = std::string(getgrgid(id)->gr_name);
  }
  return groupNames[id];
}

/**
 * @brief get a human readable file size
 *
 * @return padded human readable file size
 */
std::string fileEnt::getSize() {
  const char *prefix[] = {"  B", "KiB", "MiB", "GiB", "TiB"};
  off_t size = getStat().st_size;
  size_t i = 0;
  while(size > 1024 && i < sizeof(prefix) - 1) {
    i++;
    size >>= 10;
  }
  std::string str = std::to_string(size);
  while(str.length() < 4) {
    str = " " + str;
  }
  return str + " " + prefix[i];
}

/**
 * @brief convert the timestamp to a human readable format
 *
 * @return human readable timestamp
 */
std::string fileEnt::getTimestamp() {
  char time[32];
  //TODO handle old files by listing year instead of hh::mm
  strftime(time, 32, "%b %d %R", localtime(&getStat().st_mtim.tv_sec));
  return std::string(time);
}

/**
 * @brief print the entry in long list format
 *
 * @param f file entry to print
 */
void printList(fileEnt & f) {
  // print formatting here
  // TODO implement usual ls flags to control columns
  std::cout << f.getColor();
  std::cout << f.getPermissionString() << " ";
  std::cout << f.getOwnerName() << " ";
  std::cout << f.getGroupName() << " ";
  std::cout << f.getSize() << " ";
  std::cout << f.getTimestamp() << " ";
  std::cout << f.getIcon() << " ";
  std::cout << f.getName() << std::endl;
}

/**
 * @brief print a list of entries in long list format
 *
 * @param filenames list of file entries to print
 */
void printList(std::vector<fileEnt> & filenames) {
  size_t userMax = 0, groupMax = 0;

  // Find correct widths
  for(fileEnt & f : filenames) {
    userMax  = std::max(f.getOwnerName().length(), userMax);
    groupMax = std::max(f.getGroupName().length(), groupMax);
  }

  // Pad strings
  fileEnt::padUserNames(userMax);
  fileEnt::padGroupNames(groupMax);
  //TODO pad size column (currently works up to a petabyte)

  // Print strings
  for(fileEnt & f : filenames) {
    printList(f);
  }
} 

void usage() {
  std::cout << "Usage: lspp [al] [directory]" << std::endl;
  exit(0);
}

// Tell getopt not to print error messages for me
extern int opterr = 0;

void setoption(struct option & op, const char * name, int has_arg, int * flag, int val) {
  op.name    = name;
  op.has_arg = has_arg;
  op.flag    = flag;
  op.val     = val;
}

int main(int argc, char **argv) {
  // Handle arguments
  int  allFlag = false;
  bool longFlag = false;
  int  helpFlag = false;
  std::string lsdir = ".";
  char c;
  struct option longopts[2];
  setoption(longopts[0], "all",  0, &allFlag,  (int) true);
  setoption(longopts[1], "help", 0, &helpFlag, (int) true);
  int option_index = 0;
  while((c = getopt_long(argc, argv, "ahl", longopts, &option_index)) != -1) {
    switch (c) {
      case 0:
        break;
      case 'a':
        allFlag = true;
        break;
      case 'h':
        break;
      case 'l':
        longFlag = true;
        break;
      default:
        execvp("ls", argv);
    }
  }

  if (helpFlag) {
    usage();
  }

  if (optind < argc) {
    lsdir = std::string(argv[optind]);
  }

  struct dirent * dent;
  DIR * dir = opendir(lsdir.c_str());
  std::vector<fileEnt> filenames;

  // Read out all of the files
  while((dent = readdir(dir))) {
    if (dent->d_name[0] != '.' || allFlag) {
      filenames.push_back(fileEnt(lsdir, dent->d_name, dent->d_type));
    }
  }

  // Sort the files
  std::sort(filenames.begin(), filenames.end());

  // Find the correct formatting settings
  for(fileEnt & f : filenames) {
    std::string name = f.getName();
    std::string ext;
    std::string base;

    // Extract basename and extension from the filename
    std::size_t index = name.find_last_of(".");
    if (index == std::string::npos || index == 0) {
      base = name;
      ext = "";
    } else {
      base = name.substr(0, index);
      ext = name.substr(index + 1);
    }

    switch(f.getType()) {
      case DT_DIR:
        f.setFmt(generalFormat[dirIndex]);
        break;
      default:
        // Handle reserved Filenames
        if (lookupByFilename(f)) break;

        // Handle files by extension
        if (lookupByExtension(f, base, ext)) break;
        
        // Should never get here
        assert(0);
    }
  }

  if (longFlag) {
    printList(filenames);
  } else {
    printColumns(filenames);
  }
}
