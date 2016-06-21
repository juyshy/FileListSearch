// FileListSearch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost/iostreams/device/mapped_file.hpp> // for mmap
#include <algorithm>  // for std::find
#include <iostream>   // for std::cout
#include <cstring>
#include <boost/timer/timer.hpp>
#include <string>
#include <sstream> 
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <regex>
using std::string;
using std::cout;
using std::ifstream;
using namespace boost::filesystem;


 
using std::endl;

std::ofstream resuts_file("searchResults2.txt");

bool search(string filename, string searchString) {
  boost::timer::auto_cpu_timer t;
  boost::iostreams::mapped_file mmap(filename, boost::iostreams::mapped_file::readonly);
  const char * f = mmap.const_data();
  const char * beginning = f;
  auto end = f + mmap.size();
  auto size2 = end - f;
 
  string filecontents(beginning, size2);
  t.report();
  t.stop();
  t.start();
  std::size_t found = filecontents.find(searchString);
  std::vector<string> searchResults;
  if (found == std::string::npos)
  {
    cout << "No search results" << endl;
    return false;

  }
    

  string currentDir;
 
  while (found != std::string::npos) {
 
    std::size_t lineEndIndx = filecontents.find("\r", found + 1);
    std::size_t lineStart = filecontents.rfind('\n', found) + 1;

    if (lineStart == std::string::npos)
      lineStart = 0;
    int lineLength = lineEndIndx - lineStart;
    string resultRow = filecontents.substr(lineStart, lineLength);
    std::size_t dirfound = resultRow.find("Directory of");
    std::size_t dirfound2 = resultRow.find("<DIR>");

    if (dirfound == std::string::npos && dirfound2 == std::string::npos)
    {
      std::size_t previousDirectory = filecontents.rfind("Directory of", lineStart) + 13;
      std::size_t dirlineEndIndx = filecontents.find("\r", previousDirectory + 1);
      currentDir = filecontents.substr(previousDirectory, dirlineEndIndx - previousDirectory);
      searchResults.push_back(resultRow);
      resuts_file << currentDir << "; " << resultRow << "\n";
    }
 
    found = filecontents.find(searchString, lineStart + lineLength);

  }
 
  cout << "search results found: " << searchResults.size() << endl;

 
  return true;
}


int main()
{
   boost::timer::auto_cpu_timer t;
   //string filename = "E:/adm/hdlist/seagate500dir.txt"; 
   string filename = "E:/adm/hdlist/SeagateExpansionDrive_22DB-0CBF__12012015-2034.txt";
   //std::string filename = "E:/adm/hdlist/stuff/LACIESHARE_12012015-113107_30K_EKAARIVIA.txt";// listings[2];

   string searchString = "animaatio";
   search(filename, searchString);
 
}
//
//0.527381s wall, 0.218750s user + 0.312500s system = 0.531250s CPU(100.7%)
//search results found : 795
//1.262302s wall, 1.187500s user + 0.078125s system = 1.265625s CPU(100.3%)
//1.799449s wall, 1.406250s user + 0.390625s system = 1.796875s CPU(99.9%)
//Press any key to continue . . .