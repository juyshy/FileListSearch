// FileListSearch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost/program_options.hpp>
#include <boost/program_options/errors.hpp>
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
namespace opt = boost::program_options;

 
using std::endl;

std::ofstream resuts_file; 

const char   rivinvaihto = '\n';
const char * rivinvaihtoChar = &rivinvaihto;
const char * linestartPoint;
const char * lineEndPoint;
const char * dirStartPoint;
const char * dirEndPoint;
const char dirnamestr[] = " Directory of ";
const size_t compsize = sizeof(dirnamestr) - 1;
const char dirStr[] = "<DIR>";
const size_t compsize2 = sizeof(dirStr) - 1;

bool search2(string filename, string searchString) {

  boost::timer::auto_cpu_timer t;

  // Load file
  boost::iostreams::mapped_file mmap(filename, boost::iostreams::mapped_file::readonly);
  const char * f = mmap.const_data();
  const char * beginning = f;
  auto end = f + mmap.size();
  auto size2 = end - f;

   
  t.report();
  t.stop();
  t.start();

  // search
  //std::size_t found = filecontents.find(searchString);
  std::vector<string> searchResults;

  //static_cast<char>(searchString[0]
  auto searchChar1 = searchString[0];
  
  int searchStringLen = searchString.size();
  char * searchCharArray = reinterpret_cast<char *>(alloca(searchString.size() + 1));
  memcpy(searchCharArray, searchString.c_str(), searchStringLen + 1);

  int hitcount = 0;
  // loop through all potential search hits
  while (f && f != end  ) {
    if (f = static_cast<const char*>(memchr(f, searchChar1, end - f))) 
    {

      // check for search string
      if (  ((end - f) > searchStringLen) && memcmp(searchCharArray, f, searchStringLen) == 0)
      {
        // locate search result line start and end
        linestartPoint = lineEndPoint= f;
         while ((linestartPoint - beginning) > 0 && memcmp(rivinvaihtoChar, linestartPoint, 1) != 0)
        {
          --linestartPoint;
        }
         ++linestartPoint; // step forward to drop "\n"
         while ((end - lineEndPoint) > 0 && memcmp(rivinvaihtoChar, lineEndPoint, 1) != 0)
        {
          ++lineEndPoint;
        }
         --lineEndPoint; //  step back to drop "\n"

         // filter out directories and abnormaly long results
         if (memcmp(dirnamestr, linestartPoint, compsize) != 0
           && memcmp(dirStr, linestartPoint + 21, compsize2) != 0
           && lineEndPoint - linestartPoint < 1000)
         {
             string resultString(linestartPoint, lineEndPoint - linestartPoint);
             //searchResults.push_back(resultString);
             ++hitcount;
             //cout << " resultString:  " << resultString << endl;

             // search  and fetch the containging directory name
             dirStartPoint = linestartPoint;
             while ((dirStartPoint - beginning) > 0 && memcmp(dirnamestr, dirStartPoint, compsize) != 0)
             {
               --dirStartPoint;
             }
             dirEndPoint = dirStartPoint;
             while ((end - dirEndPoint) > 0 && memcmp(rivinvaihtoChar, dirEndPoint, 1) != 0)
             {
               ++dirEndPoint;
             }
             --dirEndPoint; //  step back to drop "\n"

             // capture only the directory name
             string lineString(dirStartPoint + compsize, dirEndPoint - dirStartPoint - compsize);
             resuts_file <<  lineString << "; " <<  resultString << "\n";
             f = lineEndPoint; // continue searching from the end of last result line
         }
      }
      f++;
    }
  }
   
 

  cout << "search results found: " << hitcount << /*searchResults.size() <<*/ endl;

  return true;
}

bool search(string filename, string searchString) {

  boost::timer::auto_cpu_timer t;
  
  // Load file
  boost::iostreams::mapped_file mmap(filename, boost::iostreams::mapped_file::readonly);
  const char * f = mmap.const_data();
  const char * beginning = f;
  auto end = f + mmap.size();
  auto size2 = end - f;

  // load file contnts to string
  string filecontents(beginning, size2);
  t.report();
  t.stop();
  t.start();

  // search
  std::size_t found = filecontents.find(searchString);
  std::vector<string> searchResults;
  if (found == std::string::npos)
  {
    cout << "No search results" << endl;
    return false;

  }
  string currentDir; 
  while (found != std::string::npos) {
    
    // locate search result line start and end
    std::size_t lineEndIndx = filecontents.find("\r", found + 1);
    std::size_t lineStart = filecontents.rfind('\n', found) + 1;

    if (lineStart == std::string::npos)
      lineStart = 0;

    int lineLength = lineEndIndx - lineStart;
    string resultRow = filecontents.substr(lineStart, lineLength);
    std::size_t dirfound = resultRow.find("Directory of");
    std::size_t dirfound2 = resultRow.find("<DIR>");

    // filter out Directories
    if (dirfound == std::string::npos && dirfound2 == std::string::npos)
    {
      // fetch the containging directory
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


int main(int argc, char *argv[]) 
{
   boost::timer::auto_cpu_timer t;
   opt::options_description desc("All options: (search and listingfiles required)");
   //_crtBreakAlloc = 894;
   desc.add_options()
     ("search,s", opt::value<std::string>(), "search string")
     ("resultfile,r",
     opt::value<std::string>()->default_value("search_resultsfile.csv"),
     "results output file name")
     ("listingfiles,l", opt::value<std::vector<std::string> >()->multitoken(),
     "file listings")
     ("help", "produce help message");

   opt::variables_map vm;

   opt::store(opt::parse_command_line(argc, argv, desc), vm);
   opt::notify(vm);

   if (vm.count("help")) {
     std::cout << desc << "\n";
     return 1;
   }

   opt::notify(vm);
   std::string searchString;
   // extracting search word from command line options
   if (!vm["search"].empty()) {
     searchString = vm["search"].as<std::string>();
   }
   else {
     std::cout << "Search option required:" << "\n";
     std::cout << desc << "\n";
     return 1;
   }
   //char * haku = reinterpret_cast<char *>(alloca(searchString.size() + 1));
   //memcpy(haku, hakustr.c_str(), hakustr.size() + 1);

   // extracting search results file file name from command line options
   std::string resultfileName = vm["resultfile"].as<std::string>();

   // extracting file listing path names from command line options
   std::vector<string> listFiles;
   if (!vm["listingfiles"].empty() &&
     (listFiles = vm["listingfiles"].as<std::vector<string> >()).size() > 0) {
     // good to go
   }
   else {
     std::cout << desc << "\n";
     return 1;
   }

 
   std::string filename; // = "E:/adm/hdlist/stuff/LACIESHARE_12012015-113107_30K_EKAARIVIA.txt"; 
   resuts_file.open(resultfileName);  //string searchString = "animaatio";
   cout << "searchString " <<  searchString << endl;
   for (string filename : listFiles) {
     cout << filename << endl;
     search2(filename, searchString);
   }

   resuts_file.close();
 
}
//
//
//searchString opengl
//E : / adm / hdlist / SeagateExpansionDrive_22DB - 0CBF__12012015 - 2034.txt
//    0.000191s wall, 0.000000s user + 0.000000s system = 0.000000s CPU(n / a%)
//    search results found : 1551
//    1.609561s wall, 1.546875s user + 0.062500s system = 1.609375s CPU(100.0%)
//    1.626751s wall, 1.546875s user + 0.062500s system = 1.609375s CPU(98.9%)
//    Press any key to continue . . .