class printer {
  public:
    enum verbose {
      shortFormat,
      longFormat
    };

    enum format {
      tree,
      columns,
      rows,
      type,
      list
    };

  private:
    std::function<void(std::vector<fileEnt> &)>  _printStyle;
    std::function<void(fileEnt const &)>         _printFile;
    std::function<ssize_t(fileEnt const &)>      _getLength;
    std::vector<fileEnt>                       & _filenames;

  public:
    printer(std::vector<fileEnt> filenames,
            std::function<void(fileEnt const &) printFile,
            std::function<void(fileEnt const &) getLength);

    static void printPadding(size_t length);
    static bool fitsInNRows(std::vector<fileEnt> & filenames,
                            unsigned short width, size_t rows,
                            std::vector<size_t> & colWidths); 

    // Main print function demultiplex on types
    static void printFiles(std::vector<fileEnt> & filenames);

    // Print formats
    void printByType(std::vector<fileEnt> & filenames);
    void printColumns(std::vector<fileEnt> & filenames);
    void printTree(std::vector<fileEnt> & filenames);
    // Should merge longlist into regular list function
    void printLongList(std::vector<fileEnt> & filenames);
    void printList(std::vector<fileEnt> & filenames);

    void printFormatColumn(fileEnt & f, size_t length);

    const std::function<void(fileEnt const &)> printLongFormat;
    const std::function<void(fileEnt const &)> printLongLine;
    const std::function<void(fileEnt const &)> printShortFormat;
    const std::function<void(fileEnt const &)> printShortLine;
}
