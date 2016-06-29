#include "stdafx.h"
#include "searchbyfileextensiononly.h"
#include "search_constants.h"
#include "utility_funcs.h"

#include <boost/timer/timer.hpp>
#include <boost/iostreams/device/mapped_file.hpp> // for mmap
#include <sstream> 
#include <string>
#include <iostream> 
#include <regex>
#include <map>
using std::cout;
using std::endl;
class SearchOptions;


void reportDriveMetadata(const char * f, std::ofstream & resuts_file){
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

bool findDups(string fileListFilename, SearchOptions searchOptions, std::ofstream &resuts_file) {

  string searchString = searchOptions.searchString;
  bool casesensitive = searchOptions.casesensitive || searchOptions.fileExtensionCheckCaseSensitive;
  string filetype = searchOptions.filetype;
  bool fullpath = searchOptions.fullpath;
  string  dateFilterStr = searchOptions.date;
  string yearFilterStr = searchOptions.year;
  string  monthYearFilterStr = searchOptions.monthYear;// "07.2011";
  string  sizeFilterStr = searchOptions.sizeFilter;

  bool timerProfiling = true;

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

  cout << "\nPreparing search.." << endl;
  cout << "Search by file size only.." << endl;
  //cout << "Search string: " << searchString << endl;

  if (filetype == "both")
  {
    cout << "Searching for both files and directories " << filetype << endl;
  }
  else
    cout << "Searching for file type: " << filetype << endl;

  const char * f = mmap.const_data();
  const char * beginning = f;
  const char * beginning2; // lowercase duplicate beginning pointer
  auto end = f + mmap.size();
  auto size2 = end - f;

  if (!f) {
    printf("Not enough memory for f. It's the end I'm afraid.\n");
    return false;
  }

  std::cout << "\nListing file: " << fileListFilename << " ";
  std::cout << "Loaded " << "\n";
  std::cout << "Size: " << mmap.size() << "\n";
  reportDriveMetadata(f, resuts_file);

  resuts_file << ">>>>" << fileListFilename + "\n";

  if (timerProfiling)
  {
    t.report();
    t.stop();
    t.start();
  }


  // prepare search

  auto searchChar1 = searchString[0];
  int searchStringLen = searchString.size();
  char * searchCharArray = reinterpret_cast<char *>(alloca(searchString.size() + 1));
  memcpy(searchCharArray, searchString.c_str(), searchStringLen + 1);
  const char * f2;


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


  int hitcount = 0;


  string fileExtension = searchOptions.fileExtension;

  bool filterFileExt = false;
  if (fileExtension.size() > 0 && fileExtension != "*")
    filterFileExt = true;

  if (fileExtension.at(0) != '.')
    fileExtension = "." + fileExtension;

  // hard coded new line appended: file extension should always be at the end of file list line
  fileExtension += "\r\n";
  int fileExtLen = fileExtension.size();
  char * fileExt = reinterpret_cast<char *>(alloca(fileExtension.size() + 1));
  memcpy(fileExt, fileExtension.c_str(), fileExtLen + 1);
  // do the file extension filtering
  bool fileExtensionCheck = false; // actual test variable initial value

  std::locale loc;
  searchChar1 = fileExt[1]; //  look initially for the first letter of the extension 

  bool sizeFilterActive = searchOptions.sizeOperand.greaterThanActive || searchOptions.sizeOperand.smallerThanActive;

  char * dateFilter = reinterpret_cast<char *>(alloca(dateFilterStr.size() + 1));
  memcpy(dateFilter, dateFilterStr.c_str(), dateFilterStr.size() + 1);

  char * yearFilter = reinterpret_cast<char *>(alloca(yearFilterStr.size() + 1));
  memcpy(yearFilter, yearFilterStr.c_str(), yearFilterStr.size() + 1);

  char * monthYearFilter = reinterpret_cast<char *>(alloca(monthYearFilterStr.size() + 1));
  memcpy(monthYearFilter, monthYearFilterStr.c_str(), monthYearFilterStr.size() + 1);

  // if empty no filtering
  bool dateFilterActive = dateFilterStr.size() > 0;
  // if empty no filtering and if dateFilterActive allready true override monthYearFilterActive
  bool monthYearFilterActive = monthYearFilterStr.size() > 0 && !dateFilterActive;
  bool yearFilterActive = yearFilterStr.size() > 0 && (!monthYearFilterActive && !dateFilterActive);

  int linecount = 0;

  //strip non file info from start and finish
  // search for first date in list to skip header lines
  string first1000chars(f2, 1000);
  std::regex firstDateReg("\n(\\d\\d\\.\\d\\d\\.\\d\\d\\d\\d)"); // internationalization needed!
  string firstDate = get_match(first1000chars, firstDateReg);
  size_t firstlinePoint = first1000chars.find(firstDate);
  f2 += firstlinePoint;
  // search for "Total Files Listed:" in the end trim search range
  string last1000chars(end - 1000, 1000);
  size_t lastlinePoint = last1000chars.rfind("     Total Files Listed:");
  end = end - 1000 + lastlinePoint;
  linestartPoint = f2;

  if (timerProfiling)
  {
    t.report();
    t.stop();
    t.start();
  }

  std::vector<string> resultRows;
  std::vector<string> duplicates;
  std::vector<string>::iterator it;
  std::cout << "searching... " << "\n";

  //std::map<string, int> dupCount;
  std::set<string> uniquerows;
  int dups = 0;
  // loop through all potential search hits
  while (f2 && f2 != end) {
    if (f2 = static_cast<const char*>(memchr(f2, '\r', end - f2)))
    {
      ++linecount;
      //
      //int charsize = f2 - linestartPoint + 1;
      //char * line = new char[charsize]();
      //strncpy(line, linestartPoint, charsize);
      bool  filter;
      // filter out directories 
      lineEndPoint = f2;
      unsigned long long size;
      bool sizeFilterCheck = false;
      filter = memcmp(dirnamestr, linestartPoint, compsize) != 0
        // filter out lines containing "<DIR>"
        && memcmp(dirStr, linestartPoint + 21, compsize2) != 0
        && memcmp("\r\n", linestartPoint, 2) != 0

        ;

      // duplicate search todo:
      // include paths for all instances in search result
      // identify duplicate directories


      if (/*sizeFilterActive  &&*/ linecount > 5 && filter /*&& linecount < 150000*/) {

        string resultString(linestartPoint, f2 - linestartPoint);
        
        size_t  numfilesrowpos = resultString.find("File(s)");
        if (numfilesrowpos == std::string::npos)
        {
          //it = find(resultRows.begin(), resultRows.end(), resultString);
          if (uniquerows.find(resultString) != uniquerows.end()){
            //duplicates.push_back(resultString);

            // search  and fetch the containging directory name
            dirStartPoint = linestartPoint;
            while ((dirStartPoint - beginning) > 0 && memcmp(dirnamestr, dirStartPoint, compsize) != 0)
            {
              --dirStartPoint;
            }
            dirEndPoint = dirStartPoint;
            while ((end - dirEndPoint) > 0 && memcmp(newLineChar, dirEndPoint, 1) != 0)
            {
              ++dirEndPoint;
            }
            --dirEndPoint; //  step back to drop "\n"

            // capture only the directory name
            string dirLineString(dirStartPoint + compsize, dirEndPoint - dirStartPoint - compsize);

            resuts_file << "DUP: " << dirLineString << "; " << resultString << "\n";
            dups++;
   
          }
          else
          {
            uniquerows.insert( resultString );
            hitcount++;
            //dupCount.insert(make_pair(resultString, 1));
            //resultRows.push_back(resultString);
            //resuts_file << resultString << "\n";
          }
        }
      
      }

      f2 += 2;

      linestartPoint = f2;

    }
  }

  //cout << "Benchmark: filtering with size: " << endl;
  //cout << "Number of results: " << hitcount << /*searchResults.size() <<*/ endl;
  cout << "Number of rows: " << linecount << endl;
    cout << "Unique files : " << hitcount << endl;
  cout << "Number of dups: " << dups << endl;
  //cout << "search results found: " << hitcount << /*searchResults.size() <<*/ endl;
  std::cout << "Writing results to " << searchOptions.resultsFilename << "\n";
  if (hitcount == 0)
    resuts_file << "NOTHING FOUND " << "\n";
  return true;
}


bool searchBySizeOnly(string fileListFilename, SearchOptions searchOptions, std::ofstream &resuts_file) {

  string searchString = searchOptions.searchString;
  bool casesensitive = searchOptions.casesensitive || searchOptions.fileExtensionCheckCaseSensitive;
  string filetype = searchOptions.filetype;
  bool fullpath = searchOptions.fullpath;
  string  dateFilterStr = searchOptions.date;
  string yearFilterStr = searchOptions.year;
  string  monthYearFilterStr = searchOptions.monthYear;// "07.2011";
  string  sizeFilterStr = searchOptions.sizeFilter;

  bool timerProfiling = true;

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

  cout << "\nPreparing search.."   << endl;
  cout << "Search by file size only.."   << endl;
  //cout << "Search string: " << searchString << endl;

  if (filetype == "both")
  {
    cout << "Searching for both files and directories " << filetype << endl;
  }
  else
    cout << "Searching for file type: " << filetype << endl;

  const char * f = mmap.const_data();
  const char * beginning = f;
  const char * beginning2; // lowercase duplicate beginning pointer
  auto end = f + mmap.size();
  auto size2 = end - f;

  if (!f) {
    printf("Not enough memory for f. It's the end I'm afraid.\n");
    return false;
  }

  std::cout << "\nListing file: " << fileListFilename << " ";
  std::cout << "Loaded " << "\n";
  std::cout << "Size: " << mmap.size() << "\n";
  reportDriveMetadata(  f,  resuts_file);

  resuts_file << ">>>>" << fileListFilename + "\n";

  if (timerProfiling)
  {
    t.report();
    t.stop();
    t.start();
  }


  // prepare search
 
  auto searchChar1 = searchString[0];
  int searchStringLen = searchString.size();
  char * searchCharArray = reinterpret_cast<char *>(alloca(searchString.size() + 1));
  memcpy(searchCharArray, searchString.c_str(), searchStringLen + 1);
  const char * f2;


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

  
  int hitcount = 0;


  string fileExtension = searchOptions.fileExtension;

  bool filterFileExt = false;
  if (fileExtension.size() > 0 && fileExtension != "*")
    filterFileExt = true;

  if (fileExtension.at(0) != '.')
    fileExtension = "." + fileExtension;

  // hard coded new line appended: file extension should always be at the end of file list line
  fileExtension += "\r\n";
  int fileExtLen = fileExtension.size();
  char * fileExt = reinterpret_cast<char *>(alloca(fileExtension.size() + 1));
  memcpy(fileExt, fileExtension.c_str(), fileExtLen + 1);
  // do the file extension filtering
  bool fileExtensionCheck = false; // actual test variable initial value

  std::locale loc;
  searchChar1 = fileExt[1]; //  look initially for the first letter of the extension 

  bool sizeFilterActive = searchOptions.sizeOperand.greaterThanActive || searchOptions.sizeOperand.smallerThanActive;

  char * dateFilter = reinterpret_cast<char *>(alloca(dateFilterStr.size() + 1));
  memcpy(dateFilter, dateFilterStr.c_str(), dateFilterStr.size() + 1);

  char * yearFilter = reinterpret_cast<char *>(alloca(yearFilterStr.size() + 1));
  memcpy(yearFilter, yearFilterStr.c_str(), yearFilterStr.size() + 1);

  char * monthYearFilter = reinterpret_cast<char *>(alloca(monthYearFilterStr.size() + 1));
  memcpy(monthYearFilter, monthYearFilterStr.c_str(), monthYearFilterStr.size() + 1);

  // if empty no filtering
  bool dateFilterActive = dateFilterStr.size() > 0;
  // if empty no filtering and if dateFilterActive allready true override monthYearFilterActive
  bool monthYearFilterActive = monthYearFilterStr.size() > 0 && !dateFilterActive;
  bool yearFilterActive = yearFilterStr.size() > 0 && (!monthYearFilterActive && !dateFilterActive);

  int linecount = 0;

  //strip non file info from start and finish
  // search for first date in list to skip header lines
  string first1000chars(f2, 1000);
  std::regex firstDateReg("\n(\\d\\d\\.\\d\\d\\.\\d\\d\\d\\d)"); // internationalization needed!
  string firstDate = get_match(first1000chars, firstDateReg);
  size_t firstlinePoint = first1000chars.find(firstDate);
  f2 += firstlinePoint;
  // search for "Total Files Listed:" in the end trim search range
  string last1000chars(end-1000, 1000);
  size_t lastlinePoint = last1000chars.rfind("     Total Files Listed:");
  end = end - 1000 + lastlinePoint;
  linestartPoint = f2;

  if (timerProfiling)
  {
    t.report();
    t.stop();
    t.start();
  }

  std::cout << "searching... " << "\n";

  // loop through all potential search hits
  while (f2 && f2 != end) {
    if (f2 = static_cast<const char*>(memchr(f2, '\r', end - f2)))
    { 
      ++linecount;
      //
      //int charsize = f2 - linestartPoint + 1;
      //char * line = new char[charsize]();
      //strncpy(line, linestartPoint, charsize);
      bool  filter;
      // filter out directories 
      lineEndPoint = f2;
      unsigned long long size;
      bool sizeFilterCheck = false;
      filter = memcmp(dirnamestr, linestartPoint, compsize) != 0
        // filter out lines containing "<DIR>"
        && memcmp(dirStr, linestartPoint + 21, compsize2) != 0
        && memcmp("\r\n", linestartPoint , 2) != 0

        ;
      if (sizeFilterActive  && linecount > 5 && filter) {
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
        if (sizeString != "File(s)" ) {
          size = boost::lexical_cast<long long>(sizeString);

          sizeFilterCheck = (!searchOptions.sizeOperand.greaterThanActive 
            || searchOptions.sizeOperand.greaterThan < size )
            && (!searchOptions.sizeOperand.smallerThanActive
            || searchOptions.sizeOperand.smallerThan > size);

        }
        else
          sizeFilterCheck = false;

        if (sizeFilterCheck) {
          string resultString(linestartPoint, f2 - linestartPoint  );

          // search  and fetch the containging directory name
          dirStartPoint = linestartPoint;
          while ((dirStartPoint - beginning) > 0 && memcmp(dirnamestr, dirStartPoint, compsize) != 0)
          {
            --dirStartPoint;
          }
          dirEndPoint = dirStartPoint;
          while ((end - dirEndPoint) > 0 && memcmp(newLineChar, dirEndPoint, 1) != 0)
          {
            ++dirEndPoint;
          }
          --dirEndPoint; //  step back to drop "\n"

          // capture only the directory name
          string dirLineString(dirStartPoint + compsize, dirEndPoint - dirStartPoint - compsize);
          resuts_file << dirLineString << "; " << resultString << "\n";
          hitcount++;
        }
      }

      f2+=2;
      
      linestartPoint  = f2;
      
    }
  }

  //cout << "Benchmark: filtering with size: " << endl;
  cout << "Number of results: " << hitcount << /*searchResults.size() <<*/ endl;

  //cout << "search results found: " << hitcount << /*searchResults.size() <<*/ endl;
  std::cout << "Writing results to " << searchOptions.resultsFilename << "\n";
  if (hitcount == 0)
    resuts_file << "NOTHING FOUND " << "\n";
  return true;
}





bool searchByFileExtensionOnly(string fileListFilename, SearchOptions searchOptions, std::ofstream &resuts_file) {

  const char   newLine = '\n';
  const char * newLineChar = &newLine;
  const char * linestartPoint;
  const char * lineEndPoint;
  const char * dirStartPoint;
  const char * dirEndPoint;
  char dirnamestr[] = " Directory of ";
  const size_t compsize = sizeof(dirnamestr) - 1;
  char dirStr[] = "<DIR>";
  const size_t compsize2 = sizeof(dirStr) - 1;

  string searchString = searchOptions.searchString;
  bool casesensitive = searchOptions.casesensitive || searchOptions.fileExtensionCheckCaseSensitive;
  string filetype = searchOptions.filetype;
  bool fullpath = searchOptions.fullpath;

 
  string  dateFilterStr = searchOptions.date;
  string yearFilterStr = searchOptions.year;
  string  monthYearFilterStr = searchOptions.monthYear;// "07.2011";
  string  sizeFilterStr = searchOptions.sizeFilter;
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
  //if (timerProfiling)
  //{
  //  t.report();
  //  t.stop();
  //  t.start();
  //}


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
 
  std::cout << "searching... " << "\n";
  int hitcount = 0;
  // loop through all potential search hits

  string fileExtension = searchOptions.fileExtension;

  bool filterFileExt = false;
  if (fileExtension.size() > 0 && fileExtension != "*")
    filterFileExt = true;

  if (fileExtension.at(0) != '.')
    fileExtension = "." + fileExtension;

  // hard coded new line appended: file extension should always be at the end of file list line
  fileExtension +=  "\r\n"; 
  int fileExtLen = fileExtension.size();
  char * fileExt = reinterpret_cast<char *>(alloca(fileExtension.size() + 1));
  memcpy(fileExt, fileExtension.c_str(), fileExtLen + 1);
  // do the file extension filtering
  bool fileExtensionCheck = false; // actual test variable initial value

  std::locale loc;
  searchChar1 = fileExt[1]; //  look initially for the first letter of the extension 
  
  
  bool sizeFilterActive = searchOptions.sizeOperand.greaterThanActive  || searchOptions.sizeOperand.smallerThanActive;
  
  char * dateFilter = reinterpret_cast<char *>(alloca(dateFilterStr.size() + 1));
  memcpy(dateFilter, dateFilterStr.c_str(), dateFilterStr.size() + 1);

  char * yearFilter = reinterpret_cast<char *>(alloca(yearFilterStr.size() + 1));
  memcpy(yearFilter, yearFilterStr.c_str(), yearFilterStr.size() + 1);
 
  char * monthYearFilter = reinterpret_cast<char *>(alloca(monthYearFilterStr.size() + 1));
  memcpy(monthYearFilter, monthYearFilterStr.c_str(), monthYearFilterStr.size() + 1);

  // if empty no filtering

  bool dateFilterActive = dateFilterStr.size() > 0 ;
  // if empty no filtering and if dateFilterActive allready true override monthYearFilterActive
  bool monthYearFilterActive = monthYearFilterStr.size() > 0 && !dateFilterActive;
  bool yearFilterActive = yearFilterStr.size() > 0 && (!monthYearFilterActive && !dateFilterActive);

  while (f2 && f2 != end) {
    if (f2 = static_cast<const char*>(memchr(f2, searchChar1, end - f2)))
    {

      f2--; // back one step to include the dot for comparison
      // check for search string

      //this time compare to file extension 
      bool endBoundaryCheck = (end - f2) > fileExtLen; 
      bool matchSting = memcmp(fileExt, f2, fileExtLen) == 0;

      if (endBoundaryCheck && matchSting)
      {

        // locate search result line start and end
        linestartPoint =  f + (f2 - beginning2); // flip to grab result from original in case of caseinsensitive search


        while ((linestartPoint - beginning) > 0 && memcmp(newLineChar, linestartPoint, 1) != 0)
        {
          --linestartPoint;
        }
        ++linestartPoint; // step forward to drop "\n"

        // as "\r\n" was appended to search string we know allready where the line ends
        // it's current location + fileExtLen and then - 2 which strips the "\r\n" appended from pointer; 
        // oh and then we have to flip it to the original f
        lineEndPoint = f + (f2 - beginning2  + fileExtLen - 2);

        bool  filter;
        // filter out directories 
        if (filetype == "file") {

          int size;
          bool sizeFilterCheck = false;
          if (sizeFilterActive) {
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
            size = boost::lexical_cast<int>(sizeString);
            
            sizeFilterCheck = searchOptions.sizeOperand.greaterThan == -1 || searchOptions.sizeOperand.greaterThan < size;
            sizeFilterCheck = sizeFilterCheck && (searchOptions.sizeOperand.smallerThan == -1 || searchOptions.sizeOperand.smallerThan > size);
          }

          // offset 3 in dd.mm.yyyy, mm.yyyy 7 chars long
          bool  monthyearCheck = memcmp(monthYearFilter, linestartPoint+3, 7) == 0;
          // offset 6 in dd.mm.yyyy, yyyy 4 chars long
          bool yearFilterCheck = memcmp(yearFilter, linestartPoint + 6, 4) == 0;
          // offset 0 in dd.mm.yyyy, yyyy 10 chars long
          bool dateFilterCheck = memcmp(dateFilter, linestartPoint , 10) == 0;
          // filter out directories
          // filter out lines containing " Directory of "
          filter = memcmp(dirnamestr, linestartPoint, compsize) != 0 
            // filter out lines containing "<DIR>"
            && memcmp(dirStr, linestartPoint + 21, compsize2) != 0
            && (!monthYearFilterActive   || monthyearCheck)
            && (!yearFilterActive  || yearFilterCheck)
            && (!dateFilterActive || dateFilterCheck)
            && (!sizeFilterActive || sizeFilterCheck);

        }
        else if (filetype == "dir" || filetype == "folder" || filetype == "directory")
        {
          // todo: date,year filterin here:
          // only directories
          filter = memcmp(dirStr, linestartPoint + 21, compsize2) == 0;
        }
        else
        {
          // filter out directories  
          filter = memcmp(dirnamestr, linestartPoint, compsize) != 0;
        }
        // filter abnormaly long results
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
          while ((end - dirEndPoint) > 0 && memcmp(newLineChar, dirEndPoint, 1) != 0)
          {
            ++dirEndPoint;
          }
          --dirEndPoint; //  step back to drop "\n"

          // capture only the directory name
          string dirLineString(dirStartPoint + compsize, dirEndPoint - dirStartPoint - compsize);

          // if fullpath written to results file extract filename
          if (fullpath) {
            // offset 17 for size and filenames in the row this hard coded value
            // propably needs to be 
            // parametrized in internationalization
            string size_filename = resultString.substr(17); 
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
      f2+= 2; //don't get stuck!!
    }
  }

  cout << "search results found: " << hitcount << /*searchResults.size() <<*/ endl;

  if (hitcount == 0)
    resuts_file << "NOTHING FOUND " << "\n";
  return true;
}


