#pragma once


#include "searchoptions.h"
#include <boost/iostreams/device/mapped_file.hpp> // for mmap
#include <boost/filesystem/operations.hpp> 

#include <string>
#include <iostream>
using std::string;






namespace file_list_search {

  class Storage
  {
  public:
    Storage();

    Storage(const std::string &s);

    ~Storage();
    unsigned long long getFileSize(const char *   linestartPoint, const char * lineEndPoint);
    bool Storage::loadFileList();
    void Storage::reportDriveMetadata(const char * f, std::ofstream & resuts_file);
    const std::string fileListFileName;
    const char * f;
    const char * beginning;
    const char * beginning2; // lowercase duplicate beginning pointer
    const char * end;
 
    int fileListSize2;
    const char * f2; // lowercase copy
    void Storage::makeLowerCaseCopy();
    void Storage::trimListSearchRange();
    int linecount = 0;
    int filecount = 0;
    const char * linestartPoint;
    const char * lineEndPoint;
    const char * dirStartPoint;
    const char * dirEndPoint;
  private:

    boost::iostreams::mapped_file mmap;
  };
}

