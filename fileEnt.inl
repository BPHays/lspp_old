friend bool operator<(fileEnt x, fileEnt y) {
  const char * xc = x._name.c_str();
  const char * yc = y._name.c_str();
  if (*xc == '.') ++xc;
  if (*yc == '.') ++yc;
  return strcasecmp(xc, yc) < 0;
}

/**
 * @brief pad a string to length with space chars
 *
 * @param str string to pad
 * @param length lenght to pad to
 *
 * @return a reference to the padded string
 */
inline static std::string pad(std::string str, size_t length) {
  str.resize(length, ' ');
  return str;
}

/**
 * @brief pad all of the groups names to length
 *
 * @param length length to pad to
 */
inline static void padGroupNames(size_t length) {
  for (auto & elem : groupNames) {
    pad(elem.second, length);
  }
}

/**
 * @brief pad all of the users names to length
 *
 * @param length length to pad to
 */
inline static void padUserNames(size_t length) {
  for (auto & elem : userNames) {
    pad(elem.second, length);
  }
}

/**
 * @brief stat the file or return the cached copy of the file's stats
 *
 * @return the file entry's stat struct from the stat syscall
 */
inline const struct stat & getStat() const {
  return _stat;
}
