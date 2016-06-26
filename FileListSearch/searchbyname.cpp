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


bool searchByName(string fileListFilename, SearchOptions searchOptions,std::ofstream &resuts_file) {

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

  boost::timer::auto_cpu_timer t;
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
  t.report();
  t.stop();
  t.start();

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
  t.report();
  t.stop();
  t.start();
  std::cout << "searching: " << "\n";
  int hitcount = 0;
  // loop through all potential search hits

  string fileExtension = ".jpg";
  int fileExtLen = fileExtension.size();
  char * fileExt = reinterpret_cast<char *>(alloca(fileExtension.size() + 1));
  memcpy(fileExt, fileExtension.c_str(), fileExtLen + 1);

  std::locale loc;
  //searchChar1 = fileExt[0];

  while (f2 && f2 != end) {
    if (f2 = static_cast<const char*>(memchr(f2, searchChar1, end - f2)))
    {

      // check for search string

      bool endBoundaryCheck = (end - f2) > searchStringLen;
      bool matchSting = memcmp(searchCharArray, f2, searchStringLen) == 0;
     
      if (endBoundaryCheck && matchSting)
      {
        
        //if ()
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

        
        bool filterFileExt = false; // do the file extension filtering
        bool fileExtensionCheck = false; // actual test variable initial value
        bool fileExtensionCheckCaseSensitive = false;

        bool  filter;
        // filter out directories and abnormaly long results
        if (filetype == "file") {

          if (filterFileExt && !fileExtensionCheckCaseSensitive) {
            const char * fileExtensionCheckStart = lineEndPoint - fileExtLen; //  beginning2 + (lineEndPoint - beginning - fileExtLen);
            string fileExtensionPortion(fileExtensionCheckStart, fileExtLen);
            string fileExtensionPortionLower = "";
            // make lowercase version
            for (std::string::size_type i = 0; i<fileExtensionPortion.length(); ++i)
              fileExtensionPortionLower += std::tolower(fileExtensionPortion[i], loc);

            fileExtensionCheck = fileExtensionPortionLower == fileExtension;// memcmp(fileExt, fileExtensionCheckStart, fileExtLen) == 0;
          
          }
          // filter out directories
          filter = memcmp(dirnamestr, linestartPoint, compsize) != 0
            && memcmp(dirStr, linestartPoint + 21, compsize2) != 0 
            && (!filterFileExt || (filterFileExt
            && fileExtensionCheck))  ;
        }
        else if (filetype == "dir" || filetype == "folder" || filetype == "directory")
        {
          // only directories
          filter = memcmp(dirStr, linestartPoint + 21, compsize2) == 0;
        }
        else
        {
          // filter out directories  
          filter = memcmp(dirnamestr, linestartPoint, compsize) != 0;
        }
        if (filter  && lineEndPoint - linestartPoint < 1000)
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
          string dirLineString(dirStartPoint + compsize, dirEndPoint - dirStartPoint - compsize);

          // if fullpath written to results file extract filename
          if (fullpath) {
            string size_filename = resultString.substr(17); // offset for size and filenames in the row
            trim(size_filename);
            std::size_t endOfSizeLocation = size_filename.find(" ");
            string filename;

            string filesizeStr;
            int filesize;
            if (endOfSizeLocation != std::string::npos)
            {
              filename = size_filename.substr(endOfSizeLocation + 1);
              filesizeStr = size_filename.substr(0, endOfSizeLocation);
              if (filesizeStr != "<DIR>")
                filesize = boost::lexical_cast<int>(filesizeStr);
              else
                trim(filename);

            }
            else
            {
              throw std::runtime_error("filename not found!");
              filename = size_filename;// todo: 
            }

            resuts_file << /*dirLineString << "; " <<*/ resultString << "; " << dirLineString << "\\" << filename << "\n";
          }
          else
            resuts_file << dirLineString << "; " << resultString << "\n";

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
