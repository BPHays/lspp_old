#ifndef FILEENT_HPP
#define FILEENT_HPP

#include <unordered_map>
#include <string.h>

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

  private:
    void statFile();

  public:

    #include "fileEnt.inl"
};

#endif /* FILEENT_HPP */
