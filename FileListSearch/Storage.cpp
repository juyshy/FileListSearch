#pragma once

#include "stdafx.h"
#include "Storage.h"

//#include "searchbyfileextensiononly.h"
#include "utility_funcs.h"

#include <boost/timer/timer.hpp>
#include <boost/iostreams/device/mapped_file.hpp> // for mmap
#include <boost/units/systems/si/prefixes.hpp>
#include <iomanip>
#include <sstream> 
#include <string>
#include <iostream> 
#include <regex>
#include <tuple>
#include <map>
using std::cout;
using std::endl;
class SearchOptions;

namespace file_list_search {

  Storage::Storage() 
  {
  }
  Storage::Storage(const std::string &s) : fileListFileName(s) { }

  Storage::~Storage()
  {

    //std::cout << "Storage destructor" << std::endl;
    mmap.close();

  }

  unsigned long long Storage::getFileSize(const char *   linestartPoint, const char * lineEndPoint){

    long long size;
    const char * sizeStartPoint = linestartPoint + 17; //offset after date & time
    while ((lineEndPoint - sizeStartPoint) > 0 && memcmp(" ", sizeStartPoint, 1) == 0)
    {
      ++sizeStartPoint;
    }
    const char * sizeEndPoint = sizeStartPoint;
    while ((lineEndPoint - sizeEndPoint) > 0 && memcmp(" ", sizeEndPoint, 1) != 0)
    {
      ++sizeEndPoint;
    }
    string sizeString(sizeStartPoint, sizeEndPoint - sizeStartPoint);

    size = boost::lexical_cast<long long>(sizeString);
    return size;
  }


  void Storage::makeLowerCaseCopy(){

      std::cout << "making lowercase copy for caseinsenstive search: " << "\n";
      char * lowrcasecopy = new char[fileListSize2 + 1]();
      int i = 0;
      //char c;
      while (f[i])
      {
        //c = ;
        lowrcasecopy[i] = tolower(f[i]);
        i++;
      }
      f2 = &lowrcasecopy[0]; // using lowercase copy for search
      end = f2 + fileListSize2;
    }

 

//

void Storage::trimListSearchRange() {

  string first1000chars(f2, 1000);
  std::regex firstDateReg("\n(\\d\\d\\.\\d\\d\\.\\d\\d\\d\\d)"); // internationalization needed!
  string firstDate = get_match(first1000chars, firstDateReg);
  size_t firstlinePoint = first1000chars.find(firstDate);
  f2 += firstlinePoint;
  // search for "Total Files Listed:" in the end trim search range
  string last1000chars(end - 1000, 1000);
  size_t lastlinePoint = last1000chars.rfind("     Total Files Listed:");
  end = end - 1000 + lastlinePoint;
  

}

  void Storage::reportDriveMetadata(const char * f, std::ofstream & resuts_file){
    string listingbeginning(f, 200);
    std::regex serPattern("Serial Number is (.*?)\r?\n");
    string sernum = get_match(listingbeginning, serPattern);
    std::regex volPattern("Volume in drive ([A-Z]) is\\s+.*?\r?\n");
    string volLetter = get_match(listingbeginning, volPattern);
    std::cout << "volume letter: " << volLetter << "\n";
    std::regex volnamePattern("Volume in drive [A-Z] is\\s+(.*?)\r?\n");
    string volName = get_match(listingbeginning, volnamePattern);
    std::cout << "volume name: " << volName << "\n\n";

    resuts_file << sernum + "\n";
    resuts_file << volName + "\n";
  }
//
  bool Storage::loadFileList(){

    try {
      // Load file
      mmap.open(fileListFileName, boost::iostreams::mapped_file::readonly);
    }
    catch (std::exception& e)
    {
      std::cerr << "Program encountered following problem: " << e.what() << '\n';

      std::cerr << "Please check your filelist filename. \n ";
      return false;
    }

    f = mmap.const_data();
    beginning = f;
    beginning2; // lowercase duplicate beginning pointer
    end = f + mmap.size();
    fileListSize2 = end - f;

    if (!f) {
      printf("Not enough memory for file load.\n");
      return false;
    }

    std::cout << "\nListing file: " << fileListFileName << " ";
    std::cout << "Loaded " << "\n";
    std::cout << "Size: " << mmap.size() << "\n";

    return true;
  }
}