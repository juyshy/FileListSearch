// FileListSearch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "utility_funcs.h"
#include <boost/program_options.hpp>
#include <boost/program_options/errors.hpp>
#include <boost/iostreams/device/mapped_file.hpp> // for mmap
#include <boost/filesystem/operations.hpp> 

#include <algorithm>  // for std::find
#include <functional> 
#include <iostream>   // for std::cout
#include <cstring>
#include <boost/timer/timer.hpp>
#include <string>
#include <sstream> 
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <regex>
#include <exception> 
#include <cctype>
#include <locale>

using std::string;
using std::cout;
using std::cin;
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
char dirnamestr[] = " Directory of ";
const size_t compsize = sizeof(dirnamestr) - 1;
char dirStr[] = "<DIR>";
const size_t compsize2 = sizeof(dirStr) - 1;


class SearchOptions
{
public:
  bool success; // success for gathering options
  string searchString;
  bool casesensitive; // is searchh casesensitive
  string filetype; // filetypes to search
  string resultsFilename; // search results written to this file
  std::vector<std::string> listFiles; // file listings path names
  bool overwrite; // overwrite by default of result file allready exists
  bool fullpath; // include full paths in the results
  string searchby; // search function
  bool timestampInAutoName;
};



bool searchByName(string fileListFilename, SearchOptions searchOptions) {

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
          // filter out directories
          filter = memcmp(dirnamestr, linestartPoint, compsize) != 0
            && memcmp(dirStr, linestartPoint + 21, compsize2) != 0;
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

bool searchFilesByFolderName(string fileListFilename, SearchOptions searchOptions) {

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


bool search(string fileListFilename, string searchString) {

  boost::timer::auto_cpu_timer t;

  // Load file
  boost::iostreams::mapped_file mmap(fileListFilename, boost::iostreams::mapped_file::readonly);
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


void checkWildCardInFileListings(std::vector<string> &listFiles) {
  std::size_t wildcardPos = listFiles.at(0).find("*");

  // if wild card in the first parameter list all files on the dir
  if (listFiles.size() == 1 && wildcardPos != std::string::npos){

    string listingDir = listFiles.at(0).substr(0, wildcardPos);
    listFiles.clear();
    boost::filesystem::directory_iterator begin(listingDir);
    boost::filesystem::directory_iterator end;

    for (; begin != end; ++begin) {

      boost::filesystem::file_status fs =
        begin->status();

      switch (fs.type()) {
      case boost::filesystem::regular_file:
        std::cout << "listing file:  ";
        std::cout << begin->path() << '\n';

        listFiles.push_back(begin->path().string());
        break;

      default:
        //std::cout << "OTHER      ";
        break;
      }
    }
  }
}


bool checkExistingFile(std::string & resultsFilename, const bool overwrite) {

  std::string resultsFilenameOriginal = resultsFilename;
  int renameSuffix = 1;
  while (fexists(resultsFilename) && !overwrite)
  {
    std::cout << "results file " << resultsFilename << " allready exists" << "\n";
    std::cout << "overwrite, rename or cancel? (o,r,c)?" << "\n";
    string mystr;

    getline(cin, mystr);
    if (mystr == "r")
    {
      string renameSuffixStr = boost::lexical_cast<string>(renameSuffix);
      resultsFilename = resultsFilenameOriginal + renameSuffixStr;

    }
    else if (mystr == "c")
    {
      std::cout << "canceling.." << "\n";
      return false;
    }
    else
    {
      std::cout << "over";
      break;
    }
    renameSuffix++;

  }
  return true;
}


bool getParameters(int argc, char *argv[], SearchOptions &searchOptions){

  opt::options_description desc("All options: (search and listingfiles required)");
  //_crtBreakAlloc = 894;
  desc.add_options()
    ("search,s", opt::value<std::string>(), "search string")
    ("listingfiles,l", opt::value<std::vector<std::string> >()->multitoken(),
    "file listings")
    ("casesensitive,c", opt::value<bool>()->default_value(false), "casesensitive")
    ("filetype,f", opt::value<std::string>()->default_value("file"), "file type to search (file, directory or both)")
    ("resultfile,r",
    opt::value<std::string>()->default_value("auto"),
    "results output file name (auto means automatically generated file name)")
    ("overwrite,o", opt::value<bool>()->default_value(false), "overwrite results file by default")
    ("fullpath,u", opt::value<bool>()->default_value(false), "fullpath included in results")
    ("searchby,b", opt::value<std::string>()->default_value("filename"), "searchtype (filename, by_directory_name)")
    ("timestamp,e", opt::value<bool>()->default_value(false), "include timestamp in auto generated result file name")
    ("help", "produce help message");

  opt::variables_map vm;
  try {
    opt::store(opt::parse_command_line(argc, argv, desc), vm);
  }
  catch (std::exception& e)
  {
    //  
    std::cerr << "\n\nERROR in options!!!  check your options: " << e.what() << "\n\n";
    std::cout << desc << "\n";
    return false;
  }
  opt::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    searchOptions.success = false;
    return false;
  }

  opt::notify(vm);

  // extracting search word from command line options
  if (!vm["search"].empty()) {
    searchOptions.searchString = vm["search"].as<std::string>();
  }
  else {
    std::cout << "Search option required:" << "\n";
    std::cout << desc << "\n";
    searchOptions.success = false;
    return false;
  }

  searchOptions.casesensitive = vm["casesensitive"].as<bool>();
  searchOptions.overwrite = vm["overwrite"].as<bool>();
  searchOptions.fullpath = vm["fullpath"].as<bool>();

  searchOptions.filetype = vm["filetype"].as<std::string>();

  searchOptions.searchby = vm["searchby"].as<std::string>();
  searchOptions.timestampInAutoName = vm["timestamp"].as<bool>();

  // extracting search results file file name from command line options
  searchOptions.resultsFilename = vm["resultfile"].as<std::string>();
  //searchOptions.timestampInAutoName = true;
  if (searchOptions.resultsFilename == "auto")
  {
    string timeString = "";
    if (searchOptions.timestampInAutoName) {
      //boost::posix_time::ptime nowTime = boost::posix_time::second_clock::local_time();

      boost::posix_time::ptime my_ptime = boost::posix_time::second_clock::universal_time();
      boost::local_time::local_date_time ldt(my_ptime, s_timezone);

      timeString = mydateformat(ldt);
      //to_simple_string(nowTime);
      searchOptions.resultsFilename = "results_for_searchTerm_" + searchOptions.searchString + "_" + timeString + ".txt";
    }
    else
    {
      searchOptions.resultsFilename = "results_for_searchTerm_" + searchOptions.searchString + ".txt";
    }

  }
  // extracting file listing path names from command line options

  if (!vm["listingfiles"].empty() &&
    (searchOptions.listFiles = vm["listingfiles"].as<std::vector<string> >()).size() > 0) {
    // good to go
  }
  else {
    std::cout << desc << "\n";
    searchOptions.success = false;
    return false;
  }
  searchOptions.success = true;
  return true;
}

int main(int argc, char *argv[])
{
  boost::timer::auto_cpu_timer t;
  SearchOptions searchOptions = SearchOptions();

  if (!getParameters(argc, argv, searchOptions))
    return 1;

  checkWildCardInFileListings(searchOptions.listFiles);

  std::string fileListFilename; // = "E:/adm/hdlist/stuff/LACIESHARE_12012015-113107_30K_EKAARIVIA.txt"; 

  if (!checkExistingFile(searchOptions.resultsFilename, searchOptions.overwrite))
    return 1;

  std::cout << "writing results to " << searchOptions.resultsFilename << "\n";
  resuts_file.open(searchOptions.resultsFilename);
  resuts_file << "searchString: " << searchOptions.searchString << "\n";
  for (string fileListFilename : searchOptions.listFiles) {
    //cout << fileListFilename << endl;
    if (searchOptions.searchby == "filename")
      searchByName(fileListFilename, searchOptions);
    else
      searchFilesByFolderName(fileListFilename, searchOptions);
  }

  resuts_file.close();


}
