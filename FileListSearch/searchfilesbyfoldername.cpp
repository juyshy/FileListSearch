#include "stdafx.h"
#include "searchbyname.h"
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



bool searchFilesByFolderName(string fileListFilename, SearchOptions searchOptions, std::ofstream &resuts_file) {

  const char   rivinvaihto = '\n';
  const char * rivinvaihtoChar = &rivinvaihto;
  const char * linestartPoint;
  const char * lineEndPoint;
  //const char * dirStartPoint;
  //const char * dirEndPoint;
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

    std::cerr << "exception caught:  " << e.what() << '\n';
    return false;
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
  string listingbeginning(f, 200);
  std::regex serPattern("Serial Number is (.*?)\r?\n");
  string sernum = get_match(listingbeginning, serPattern);
  std::regex volPattern("Volume in drive ([A-Z]) is\\s+.*?\r?\n");
  string volLetter = get_match(listingbeginning, volPattern);
  std::cout << "volume letter: " << volLetter << "\n";
  std::regex volnamePattern("Volume in drive [A-Z] is\\s+(.*?)\r?\n");
  string volName = get_match(listingbeginning, volnamePattern);
  std::cout << "volume name: " << volName << "\n";
  resuts_file << ">>>>" << fileListFilename + "\n";
  resuts_file << sernum + "\n";
  resuts_file << volName + "\n";
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
        linestartPoint = lineEndPoint = beginning + (f2 - beginning2); // flip to search from original in case of caseinsensitive search
        while ((linestartPoint - beginning) > 0 && memcmp(rivinvaihtoChar, linestartPoint, 1) != 0)
        {
          --linestartPoint;
        }
        ++linestartPoint; // step forward to drop "\n"
        while ((end - lineEndPoint) > 0 && memcmp(rivinvaihtoChar, lineEndPoint, 1) != 0)
        {
          ++lineEndPoint;
        }
        //--lineEndPoint; //  this time no need to step back to drop "\n"
        //string resultString(linestartPoint, lineEndPoint - linestartPoint);

        // do we have a directory lissting start?
        if (memcmp(dirnamestr, linestartPoint, compsize) == 0)

        {
          f = lineEndPoint; // start searching for the next directory in the listing
          while (f && f != end) {
            if (f = static_cast<const char*>(memchr(f, '\n', end - f)))
            {
              f++;
              // check for next dir
              if (((end - f) > compsize) && memcmp(dirnamestr, f, compsize) == 0)
              {
                f--;
                string resultString(linestartPoint, f - linestartPoint); // grab the whole thing

                ++hitcount;
                //cout << " resultString:  " << resultString << endl;
                replace(resultString, "\r\n", "\n");

                resuts_file << resultString << "\n";

                f2 = beginning2 + (f - beginning);
                break;

              }
            }
          }
        }
      }
      f2++;
    }
  }

  cout << hitcount << " directories and their contents found  " << /*searchResults.size() <<*/ endl;

  if (hitcount == 0)
    resuts_file << "NOTHING FOUND " << "\n";
  return true;
}



