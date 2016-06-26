#include "stdafx.h"
#include "searchfromcdtree.h"
#include "utility_funcs.h"

#include <boost/timer/timer.hpp>
#include <boost/iostreams/device/mapped_file.hpp> // for mmap
#include <sstream> 
#include <string>
#include <iostream> 
#include <regex>
using std::cout;
using std::endl;
class SearchOptions;



bool searchFromCdTree(string fileListFilename, SearchOptions searchOptions, std::ofstream &resuts_file) {

  const char   rivinvaihto = '\n';
  const char * rivinvaihtoChar = &rivinvaihto;
  const char * linestartPoint;
  const char * lineEndPoint;
  const char * dirStartPoint;
  const char * dirEndPoint;
  char dirnamestr[] = " Directory of ";
  const size_t compsize = sizeof(dirnamestr) - 1;
  char dirStr[] = "<DIR>";
  const size_t compsize2 = sizeof(dirStr) - 1;

  string searchString = searchOptions.searchString;
  bool casesensitive = searchOptions.casesensitive;
  string filetype = searchOptions.filetype;
  bool fullpath = searchOptions.fullpath;

  //boost::timer::auto_cpu_timer t;
  boost::iostreams::mapped_file mmap;
  try {

    // Load file
    mmap.open(fileListFilename, boost::iostreams::mapped_file::readonly);
  }
  catch (std::exception& e)
  {

    std::cerr << "exception caught: " << e.what() << '\n';
    return 1;
  }
  cout << "searchString " << searchString << endl;
  if (filetype == "both")
  {
    cout << "searching for both files and directories " << filetype << endl;
  }
  else
    cout << "searching for file type: " << filetype << endl;

  const char * f = mmap.const_data();
  const char * beginning = f;
  const char * beginning2; // lowercase duplicate beginning pointer
  auto end = f + mmap.size();
  auto size2 = end - f;


  if (!f) {
    printf("Not enough memory for f. It's the end I'm afraid.\n");
    return false;
  }
  std::cout << "listing file: " << fileListFilename << " ";
  std::cout << "loaded " << "\n";
  std::cout << "size: " << mmap.size() << "\n";
  // ulong stop1 = GetTickCount();
  // cout << "mmap.open, mmap.const_data took " << stop1 - start1 << "mS" << endl;
 

  resuts_file << ">>>>" << fileListFilename + "\n";
 
  //t.report();
  //t.stop();
  //t.start();

  // search
  //std::vector<string> searchResults;
  auto searchChar1 = searchString[0];
  int searchStringLen = searchString.size();
  char * searchCharArray = reinterpret_cast<char *>(alloca(searchString.size() + 1));
  memcpy(searchCharArray, searchString.c_str(), searchStringLen + 1);
  const char * f2;
  //bool casesensitive = false;
  if (!casesensitive) { // not casesensitive make a lower copy

    std::cout << "making lowercase copy for caseinsenstive search: " << "\n";
    char * lowrcasecopy = new char[size2 + 1]();
    int i = 0;
    char c;
    while (f[i])
    {
      c = f[i];
      lowrcasecopy[i] = tolower(c);
      i++;
    }
    f2 = &lowrcasecopy[0]; // using lowercase copy for search
    end = f2 + size2;
  }
  else
  {
    f2 = f; // using the original

  }

  beginning2 = f2;
  end = f2 + size2;
  //t.report();
  //t.stop();
  //t.start();
  std::cout << "searching: " << "\n";
  int hitcount = 0;
  // loop through all potential search hits
  while (f2 && f2 != end) {
    if (f2 = static_cast<const char*>(memchr(f2, searchChar1, end - f2)))
    {

      // check for search string
      if (((end - f2) > searchStringLen) && memcmp(searchCharArray, f2, searchStringLen) == 0)
      {
        // locate search result line start and end
        linestartPoint = lineEndPoint = f + (f2 - beginning2); // flip to search from original in case of caseinsensitive search
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

        bool  filter;
        // filter out directories and abnormaly long results
        if (filetype == "file") {
          // filter in only files    
          filter = memcmp("F,", linestartPoint, 2) == 0  ;
        }
        else if (filetype == "dir" || filetype == "folder" || filetype == "directory")
        {
          //  only directories
          filter = memcmp("D,", linestartPoint, 2) == 0  ;
        }
        else
        {
          // filter out  cd/dvd names  
          filter = memcmp("C,", linestartPoint, 2) != 0;
        }
        if (filter  && lineEndPoint - linestartPoint < 1000)
        {
          string resultString(linestartPoint, lineEndPoint - linestartPoint);
          //searchResults.push_back(resultString);
          ++hitcount;
          //cout << " resultString:  " << resultString << endl;

          // search  and fetch the containging directory name
          dirStartPoint = linestartPoint;
          while ((dirStartPoint - beginning) > 0 && memcmp("\nD,", dirStartPoint, 3) != 0)
          {
            --dirStartPoint;
          }
          dirStartPoint++;
          dirEndPoint = dirStartPoint;
          while ((end - dirEndPoint) > 0 && memcmp(rivinvaihtoChar, dirEndPoint, 1) != 0)
          {
            ++dirEndPoint;
          }
          --dirEndPoint; //  step back to drop "\n"

          // capture  the directory  line
          string dirLineString(dirStartPoint, dirEndPoint - dirStartPoint );


          const char * cdStartPoint = dirStartPoint;
          while ((cdStartPoint - beginning) > 0 && memcmp("\nC,", cdStartPoint, 3) != 0)
          {
            --cdStartPoint;
          }
          cdStartPoint++;
          const char *  cdEndPoint = cdStartPoint;
          while ((end - cdEndPoint) > 0 && memcmp(rivinvaihtoChar, cdEndPoint, 1) != 0)
          {
            ++cdEndPoint;
          }
          --cdEndPoint; //  step back to drop "\n"

          cdStartPoint--; // step backward
          string cdLineString(cdStartPoint, cdEndPoint - cdStartPoint);


          string cdname = cdLineString.substr(3 );
          trim(cdname);
          std::size_t endOfname = cdname.find(",");
          cdname = cdname.substr(0, endOfname);
          string dirname = dirLineString.substr(3 );
          std::size_t begOfname = dirname.find(",");
          
          dirname = dirname.substr(begOfname+1 );
          endOfname = dirname.find(",");
          dirname = dirname.substr(0, endOfname);
          string fileinfo = resultString.substr(3 );

          begOfname = fileinfo.find(",");
          fileinfo = fileinfo.substr(begOfname + 1);
      
          resuts_file << cdname << "; " << dirname << "; " << fileinfo << "\n";
 
         // resuts_file << cdLineString << "; " << dirLineString << "; " << resultString << "\n";

          f2 = beginning2 + (lineEndPoint - beginning); // continue searching from the end of last result line
        }
      }
      f2++;
    }
  }

  cout << "search results found: " << hitcount << /*searchResults.size() <<*/ endl;

  if (hitcount == 0)
    resuts_file << "NOTHING FOUND " << "\n";
  return true;
}



