#include <getopt.h>
#include <unistd.h>

#include "argSet.hpp"

// Tell getopt not to print error messages. This allows cleaner handling of
// a getopt failure where /bin/ls is simply called
extern int opterr = 0;

/**
 * @brief 
 *
 * @param argc number of command line argumnets
 * @param argv array of the command line arguments
 */
argSet::argSet(int argc, char * const * argv) {
  short c;
  int  option_index = 0;

  // gnu style long options 
  // start switch indices after ascii to avoid collisions
  struct option longopts[] = {
    {"all",             0, NULL, 'a'    },
    {"allmost-aLl",     0, NULL, 'A'    },
    {"author",          0, NULL, opt_author },
    {"help",            0, NULL, opt_help   },
    {"reverse",         0, NULL, 'r'    },
    {"ft",              1, NULL, opt_ft     },
    {"noFmt",           0, NULL, opt_noFmt  },
    {"type",            0, NULL, opt_type   },
    {"color",           2, NULL, opt_color  },
    {"icon",            2, NULL, opt_icon   },
    {"recursive",       0, NULL, 'R'    },
    {"tree",            0, NULL, opt_tree   },
    {"perm",            0, NULL, opt_perm   },
    {"quote-name",      0, NULL, 'Q'    },
    {NULL,              0, NULL, 0      }
  };

  // parse the args
  while((c = getopt_long(argc, argv, "aAglorQRStuUX1", longopts, &option_index)) != -1) {
    switch (c) {
      // Handle long only args
      case opt_ft:
        setFlag(ft);
        setListType(optarg);
        break;
      case opt_color:   
        if (optarg == NULL) {
          // default to "always"
          setFlag(color);  
        } else {
          std::string colorArg = std::string(optarg);
          if (!colorArg.compare("auto") && isatty(1)) {
            // only color the output when stdout is a tty
            setFlag(color);
          } else if (!colorArg.compare("always")) { 
            // always color the output
            setFlag(color);
          } else if (!colorArg.compare("never")) {
            // never color the output
            setFlag(color, false);
          }
        }
        break;
      case opt_icon:   
        if (optarg == NULL) {
          // default to "always"
          setFlag(icon);  
        } else {
          std::string iconArg = std::string(optarg);
          if (!iconArg.compare("auto") && isatty(1)) {
            // only color the output when stdout is a tty
            setFlag(icon);
          } else if (!iconArg.compare("always")) { 
            // always color the output
            setFlag(icon);
          } else if (!iconArg.compare("never")) {
            // never color the output
            setFlag(icon, false);
          }
        }
        break;
      case opt_author:  setFlag(author); break;
      case opt_help:    setFlag(help);   break;
      case opt_noFmt:   setFlag(noFmt);  break;
      case opt_perm:    setFlag(perm);   break;
      case opt_type:    setFlag(type);   break;
      case opt_tree:    setFlag(tree);   break;

      // Handle short args, argumnets with both a long and short argument
      // have their long argument routed to the same location as the short arg
      case 'a': setFlag(all);        break;
      case 'A': setFlag(almostAll);  break;
      case 'g': setFlag(noOwner);    break;
      case 'l': setFlag(longList);   break;
      case 'o': setFlag(noGroup);    break;
      case 'r': setFlag(reverse);    break;
      case 'Q': setFlag(quote);      break;
      case 'R': setFlag(recursive);  break;
      case 'S': setFlag(sortSize);   break;
      case 't': setFlag(sortTime);   break;
      case 'u': setFlag(sortAccTime);break;
      case 'U': setFlag(sortInDir);  break;
      case 'X': setFlag(sortExt);    break;
      case '1': setFlag(filePerLine);break;
      default:  execvp("ls", argv);
    }
  }

  // Get directory to list or use "." if none provided
  if (optind < argc) {
    setLsDir(std::string(argv[optind]));
  } else {
    setLsDir(".");
  }
}
