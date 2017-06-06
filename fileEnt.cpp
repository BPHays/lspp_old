#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <dirent.h>
#include <grp.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>

#include "fileEnt.hpp"
#include "format.hpp"

std::unordered_map<uid_t, std::string> fileEnt::userNames;
std::unordered_map<gid_t, std::string> fileEnt::groupNames;

fileEnt::fileEnt(std::string dir, std::string name, unsigned char type) :
  _path(dir + "/" + name),
  _name(name),
  _type(type),
  _nSuffixIcons(0)
  {
    if (stat(_path.c_str(), &_stat) < 0) {
      perror("fileEnt::fileEnt");
    };
    if (isLink()) {
      ++_nSuffixIcons;
    }
    if (isVisible()) {
      ++_nSuffixIcons;
    }
  }

fileEnt::~fileEnt(){}

/**
 * @brief name getter
 *
 * @return the file's name ex file.c
 */
std::string fileEnt::getName() const { return _name; }
std::string fileEnt::getPath() const { return _path; }


/**
 * @brief get the dirent type of the file
 *
 * @return type field from the file's dirent
 */
unsigned char fileEnt::getType() const { return _type; }


/**
 * @brief get the number of suffix icons for the file
 *
 * @return number of suffix icons to follow the filename
 */
const size_t & fileEnt::getNSuffixIcons() const { return _nSuffixIcons; }


/**
 * @brief get the suffix icons for the file
 *
 * @return a list of icons padded with a space afterward
 */
std::string fileEnt::getSuffixIcons() const {
  if (_nSuffixIcons == 0) {
    return "";
  }
  std::string icons = "";
  if (isLink()) {
    icons += " " LINK_ICON;
  }
  if (isVisible()) {
    icons += " " VISIBLE_ICON;
  }
  return icons;
}

/**
 * @brief set the format entry to use
 *
 * @param fmt the format entry to use
 */
void fileEnt::setFmt(const fileFmt *fmt) { 
  _fmt = fmt;
}

/**
 * @brief format string with colorized name and icon
 *
 * @param length length to pad formatted string to
 *
 * @return the formatted string
 */
std::string fileEnt::formatted(size_t length) const {
  std::string padding = "";
  size_t suffixLen = getNSuffixIcons() > 0 ? 2 * getNSuffixIcons() : 0;
  ssize_t padLen = length - _name.length() - suffixLen;
  if (padLen >= 0) {
    padding.resize(padLen, ' ');
  }
  return getEmphasis() + _fmt->fmt + _fmt->icon + " " + _name + getSuffixIcons() + padding;
}

/**
 * @brief get the entries color format field
 *
 * @return the entries color format field
 */
const std::string & fileEnt::getColor() const {
  return _fmt->fmt;
}


/**
 * @brief return the color format for the permissions bits
 *
 * @return the correct format for the file's permissions
 */
const std::string & fileEnt::getPermColor() const {
  // TODO get the current user's permissions so may need to check group/others
  switch ((_stat.st_mode >> 6) & 7) {
    case 7:
      return RWX_PERM;
    case 6:
      return RW_PERM;
    case 5:
      return RX_PERM;
    case 4:
      return R_PERM;
    case 3:
      return WX_PERM;
    case 2:
      return W_PERM;
    case 1:
      return X_PERM;
    case 0:
      return NO_PERM;
  }

  /* Should never get here */
  assert(0);
}

/**
 * @brief get the entries icon format field
 *
 * @return the entries icon format field
 */
const std::string & fileEnt::getIcon() const {
  return _fmt->icon;
}

/**
 * @brief get the fileType entry for the file
 *
 * @return a pointer to the fileType entry for the file
 */
const fileType * fileEnt::getFileType() const {
  if (_fmt == NULL) {
    return NULL;
  } else {
    return _fmt->parent;
  }
}

/**
 * @brief return the padded link icon if the file is a link
 *
 * @return the link icon or empty string depending on the filetype
 */
const char * fileEnt::getLink() const {
  if (isLink()) {
    return  " " LINK_ICON " ";
  } else {
    return "";
  }
}

/**
 * @brief check if the file is a link, some FSs may require statting the file
 *
 * @return true if the file is a link
 */
bool fileEnt::isLink() const {
  struct stat lstats;
  switch(_type) {
    case DT_UNKNOWN:
      // Only used for filesystems that don't support d_type in dirents
      // Requires an extra call to stat
      lstat(_path.c_str(), &lstats);
      return S_ISLNK(lstats.st_mode);
    case DT_LNK:
      return true;
    default:
      return false;
  }
}

bool fileEnt::isDir() const {
  struct stat lstats;
  switch(_type) {
    case DT_UNKNOWN:
      // Only used for filesystems that don't support d_type in dirents
      // Requires an extra call to stat
      lstat(_path.c_str(), &lstats);
      return S_ISDIR(lstats.st_mode);
    case DT_DIR:
      return true;
    default:
      return false;
  }
}

/**
 * @brief extract the permissions bits fromt the type and stat mode
 *
 * @return human readable permissions bits string
 */
std::string fileEnt::getPermissionString() const {
  struct stat lstats;
  std::string permStr = "";

  // Set the type char
  switch(_type) {
    case DT_LNK:
      permStr += 'l';
      break;
    case DT_DIR:
      permStr += 'd';
      break;
    case DT_BLK:
      permStr += 'b';
      break;
    case DT_CHR:
      permStr += 'c';
      break;
    case DT_SOCK:
      permStr += 's';
      break;
    case DT_FIFO:
      permStr += 'p';
      break;
    case DT_UNKNOWN:
      // lstat the file if no dirent data
      lstat(_path.c_str(), &lstats);
      switch(lstats.st_mode) {
        case S_IFLNK:
          permStr += 'l';
          break;
        case S_IFDIR:
          permStr += 'd';
          break;
        case S_IFBLK:
          permStr += 'b';
          break;
        case S_IFCHR:
          permStr += 'c';
          break;
        case S_IFSOCK:
          permStr += 's';
          break;
        case S_IFIFO:
          permStr += 'p';
          break;
        default:
          permStr += '-';
      }
      break;
    case DT_REG:
      /* fallthrough */
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
std::string & fileEnt::getOwnerName() const {
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
std::string & fileEnt::getGroupName() const {
  gid_t id = getStat().st_gid;
  if (groupNames.find(id) == groupNames.end()) {
    groupNames[id] = std::string(getgrgid(id)->gr_name);
  }
  return groupNames[id];
}

/**
 * @brief check if the file has any permissions in the others columns
 *
 * @return true if others have any permissions for the file
 */
bool fileEnt::isVisible() const {
  if ((getStat().st_mode & 0x7) != 0) {
    return true;
  } else {
    return false;
  }
}

/**
 * @brief get the empasis (bold, underline, etc) formatting for the file
 *
 * @return bold if the file is executable or nothing otherwise
 */
const char * fileEnt::getEmphasis() const {
  if (getStat().st_mode >> 6 & 0x1 && _type != DT_DIR) {
    return BOLD;
  } else {
    return NO_EMPH;
  }
}

/**
 * @brief get a human readable file size
 *
 * @return padded human readable file size
 */
std::string fileEnt::getSizeStr() const {
  const char *prefix[] = {"  B", "KiB", "MiB", "GiB", "TiB", "PiB", "XiB"};
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

off_t fileEnt::getSize() const {
  return getStat().st_size;
}

/**
 * @brief get the number of hard links to the file
 *
 * @param padding amount to pad the string to fit nicely in a column
 *
 * @return the number or hard links converted to a string
 */
std::string fileEnt::getRefCnt(int padding) const {
  std::string padStr = "";
  std::string refCnt = std::to_string(_stat.st_nlink);
  if (padding > 0) {
    return pad(padStr, padding - refCnt.length()) + refCnt;
  } else {
    return refCnt;
  }
}

/**
 * @brief get the path that a symlink points to
 *
 * @return a string containing the path to the symlink target
 */
std::string fileEnt::getTarget() const {
  const size_t size = 1024;
  char targBuf[size];
  ssize_t len;
  if((len = readlink(_path.c_str(), targBuf, size)) > 0) {
    targBuf[len] = '\0';
    return std::string(targBuf);
  } else {
    return "";
  }
}

/**
 * @brief convert the timestamp to a human readable format
 *
 * @return human readable timestamp
 */
std::string fileEnt::getTimestampStr() const {
  char timeBuff[32];
  time_t timeStamp = getStat().st_mtim.tv_sec; 
  if((time(0) - timeStamp) < 60 * 60 * 24 * 365) {
    // Print the time when under a year old
    strftime(timeBuff, 32, "%b %d %R", localtime(&timeStamp));
  } else {
    // Print the year when over a year old
    strftime(timeBuff, 32, "%b %d %Y ", localtime(&timeStamp));
  }
  return std::string(timeBuff);
}

time_t fileEnt::getModTS() const {
  return getStat().st_mtim.tv_sec;
}

time_t fileEnt::getAccTS() const {
  return getStat().st_atim.tv_sec;
}

