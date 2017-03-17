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
  _haveStats(false)
  {}

fileEnt::~fileEnt(){}

std::string fileEnt::getName(){ return _name; }
unsigned char fileEnt::getType(){ return _type; }

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

