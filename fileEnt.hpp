#ifndef FILEENT_HPP
#define FILEENT_HPP

#include <unordered_map>

#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#include "format.hpp"

class fileEnt {
  private:
    std::string    _path;         // Full file path a/b/c/d.ex
    std::string    _name;         // File name d.ex
    unsigned char  _type;         // dirent type
    const fileFmt *_fmt;          // associated format struct
    struct stat    _stat;         // file stats from stat syscall
    size_t         _nSuffixIcons; // number of suffix icons

  private:
    // Cache for queried user names
    static std::unordered_map<uid_t, std::string> userNames;
    // Cache for queried group names
    static std::unordered_map<gid_t, std::string> groupNames;

  public: 
    fileEnt(std::string dir, std::string name, unsigned char type = DT_UNKNOWN);
    virtual ~fileEnt();

    // Setters
    void setFmt(const fileFmt *fmt);

    // Direct member getters
          unsigned char getType() const;
    const std::string & getColor()                    const;
    const std::string & getPermColor()                const;
    const std::string & getIcon()                     const;
          std::string   getName()                     const;
          std::string   getPath()                     const;
    const size_t      & getNSuffixIcons()             const;
          time_t        getModTS()                    const;
          time_t        getAccTS()                    const;
          off_t         getSize()                     const;

    // Other getters
          std::string   formatted(size_t length)      const;
          std::string   getPermissionString()         const;
          std::string & getOwnerName()                const;
          std::string & getGroupName()                const;
          std::string   getSizeStr()                  const;
          std::string   getTimestampStr()             const;
          std::string   getRefCnt(int padding = -1)   const;
          std::string   getSuffixIcons()              const;
          std::string   getTarget()                   const;
    const char        * getEmphasis()                 const;
    const char        * getLink()                     const;
    const fileType    * getFileType()                 const;
          bool          isLink()                      const;
          bool          isDir()                       const;
          bool          isVisible()                   const;

  private:

  public:

    #include "fileEnt.inl"
};

#endif /* FILEENT_HPP */
