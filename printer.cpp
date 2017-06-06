#include "printer.hpp"

// Print in long format
const std::function<void(fileEnt & f)> printer::printLongFormat(size_t linksMax) {
  return [linksMax](auto & f) { 
              if (!args.getFlag(argSet::flags::noFmt) &&
                  args.getFlag(argSet::flags::color)) {
                std::cout << f.getEmphasis();
                if (args.getFlag(argSet::flags::perm)) {
                  std::cout << f.getPermColor();
                } else {
                  std::cout << f.getColor();
                }
              }
              std::cout << f.getPermissionString() << " ";
              std::cout << f.getRefCnt(linksMax) << " ";
              if (!args.getFlag(argSet::flags::noOwner)) {
                std::cout << f.getOwnerName() << " ";
              }
              if (!args.getFlag(argSet::flags::noGroup)) {
                std::cout << f.getGroupName() << " ";
              }
              if (args.getFlag(argSet::flags::author)) {
                std::cout << f.getOwnerName() << " ";
              }
              std::cout << f.getSizeStr() << " ";
              std::cout << f.getTimestampStr() << " ";
              if (args.getFlag(argSet::flags::icon)) {
                std::cout << f.getIcon() << " ";
              }
              if (args.getFlag(argSet::flags::quote)) {
                std::cout << "\"";
              }
              std::cout << f.getName() ;
              if (args.getFlag(argSet::flags::quote)) {
                std::cout << "\"";
              }
              std::cout << f.getSuffixIcons();
              if (f.isLink()) {
                std::cout << " " << f.getTarget();
              }
              std::cout << std::endl; };
}

// Print in short format
const std::function<void(fileEnt const &)> printer::printShortFormat = 
           [](auto & f) { 
            if (args.getFlag(argSet::flags::color)) {
              std::cout << f.getColor();
            }
            if (args.getFlag(argSet::flags::icon)) {
              std::cout << f.getIcon() << " ";
            }
            if (args.getFlag(argSet::flags::quote)) {
              std::cout << "\"";
            }
            std::cout << f.getName();
            if (args.getFlag(argSet::flags::quote)) {
              std::cout << "\"";
            }};

// Print short format on a line
const std::function<void(fileEnt const &)> printer::printShortLine =
           [](auto & f) {
            printShortFormat(f);
            std::cout << std::endl;};

// Print long format on a line
const std::function<void(fileEnt const &)> printer::printLongLine =
           [](auto & f) {
             printLongFormat(f);
             std::cout << std::endl;};

// Get the full length of the filename with suffix icons
const std::function<ssize_t(fileEnt const &)> printer::shortLength =
           [](auto & f) {
            size_t suffixLen = f.getNSuffixIcons() * 2;
            return f.getName().length() + suffixLen;};

// TODO implement actually look at the sizes of the oher fields and constant amounts of padding
const std::functino<ssize_t(fileEnt const&)> printer::longLength =
           [](auto & f) {
            size_t suffixLen = f.getNSuffixIcons() * 2;
            return f.getName().length() + suffixLen + 
                   f.getOwnerName().length() + f.getGroupName().length() + 
                   f.getRefCnt().length();};

// Basic printer contructor
printer::printer(std::vector<fileEnt> filenames,
                 std::function<void(fileEnt const &) printFile,
                 std::function<void(fileEnt const &) getLength) :
                 _filenames(filenames),
                 _printFile(printFile),
                 _getLength(getLength)
                 {}

// Actually do the printing
printer::print() {
  _printStyle();
}

/**
 * @brief print the list of files according to the command line flags
 */
static void printer::printFiles(std::vector<fileEnt> & filenames) {
  printer p;

  if (args.getFlag(argSet::flags::longList) || 
      args.getFlag(argSet::flags::noGroup) || 
      args.getFlag(argSet::flags::noOwner)) {
    // Print in long list format
    p = printer(filenames, printLongLine, longLength, printLongList);

  } else {
    if (isatty(1) && !( args.getFlag(argSet::flags::filePerLine) ||
        args.getFlag(argSet::flags::tree))) {
      // Print in compact columnar format
      p = printer(filenames, printShortFormat, shortLength, printColumns);
    } else {
      if (args.getFlag(argSet::flags::tree)) {
        // List recursively as a tree
        p = printer(filenames, printShortFormat, shortLength, printTree);
      } else {
        // List one file per line short formatted
        p = printer(filenames, printShortLine, shortLength, printList);
      }
    }
  }

  printer.print();
}

/**
 * @brief generate file list by filetype and print them separately
 */
void printer::printByType() {
  std::map<const std::string, std::vector<fileEnt> > typeMap;

  // Generate list by types
  std::for_each(filenames.begin(), filenames.end(), 
    [&typeMap](auto & f){
      typeMap[f.getFileType()->typeName].push_back(f);});

  // Print by type
  for(auto & t : typeMap) {
    std::cout << ESC "0m" << t.first << std::endl;
    printer p(t.second, this->_printFile, this->_getLength);
    p.printFiles();
    std::cout << std::endl;
  }
}

/**
 * @brief simply print each file on its own line
 */
void printer::printList() {
  for_each(filenames.begin(), filenames.end(), _printFile);
}

/**
 * @brief find the right size for the variable length columns
 *
 * @param linksMax longest number of hard links size
 */
void printer::getLongListSizes(size_t & linksMax) {
  size_t userMax = 0, groupMax = 0;

  // TODO replace with calls to accumulate
  // Find correct widths
  for(fileEnt & f : filenames) {
    userMax  = std::max(f.getOwnerName().length(), userMax);
    groupMax = std::max(f.getGroupName().length(), groupMax);
    linksMax = std::max(f.getRefCnt().length(), linksMax);
  }

  // Pad strings
  fileEnt::padUserNames(userMax);
  fileEnt::padGroupNames(groupMax);
  //TODO pad size column (currently works up to a petabyte)
}

/**
 * @brief print a list of entries in long list format
 */
void printer::printLongList() {
  size_t linksMax = 0;
  getLongListSizes(linksMax);

  // Print strings
  for_each(filenames.begin(), filenames.end(), _printFile);
}

/**
 * @brief print the file padded to fit its column's width
 *
 * @param f file to print
 * @param length width to pad to
 */
void printer::printFormatColumn(fileEnt & f, size_t length) {
  ssize_t padLen = length - _getLength(f);
  _printFile(f);
  if (padLen >= 0) {
    printPadding(padLen);
  }
}

/**
 * @brief print the files as a tree recursing on subdirectories
 */
void printTree() {
  // Keep track of if each directory traversed has more elements
  // TODO this may not work in the object may need to be moved into a static
  // class variable
  static std::vector<bool> indent = { true };

  for (auto f = filenames.begin(); f != filenames.end(); f++) {
    std::cout << DIR_C;
    // Print characters for the previous and current direcotries of the tree
    for (auto b = indent.begin(); b != indent.end(); b++) {
      if (*b) { 
        if (b == indent.end() - 1) {
          if (f == filenames.end() - 1) {
            std::cout << TREE_ELBOW;
          } else {
            std::cout << TREE_BRANCH_RIGHT;
          }
        } else {
          std::cout << TREE_VERTICAL; 
        }
      } else { 
        std::cout << TREE_EMPTY_SPACE; 
      }
    }

    // Handle a new branch sprouting if the file is a dir
    if (args.getFlag(argSet::flags::tree)) {
      if (f->isDir()) {
        std::cout << TREE_BRANCH_DOWN;
      } else {
        std::cout << TREE_HORIZONTAL;
      }
    }

    // List the file
    _printFile(*f);

    // If a directory recurse on the directory
    if (f->isDir()) {
      if (f == filenames.end() - 1) {
        indent.pop_back();
        indent.push_back(false);
      }
      indent.push_back(true);
      listDirectory(f->getPath());
      indent.pop_back();
    }
  }
}

/**
 * @brief print len characters of padding to the screen
 *
 * @param len number of characters of padding to print
 */
static void printer::printPadding(size_t len) {
  // TODO at some point support tab padding, but for now everything is spaces
  static std::string pad(256, ' ');
  pad.resize(len, ' ');
  std::cout << pad;
}

