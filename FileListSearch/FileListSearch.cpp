// FileListSearch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost/program_options.hpp>
#include <boost/program_options/errors.hpp>
#include <boost/iostreams/device/mapped_file.hpp> // for mmap
 
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



//http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
// trim from start (in place)
static inline void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
  ltrim(s);
  rtrim(s);
}

string  get_match(std::string const &s, std::regex const &r) {
  std::smatch match;
  string matchStr;
  if (std::regex_search(s, match, r)) {
    /*std::cout << "Sernum: " << */
    matchStr = match[1].str();
    return matchStr;
  }
  else {
    std::cerr << s << "did not match\n";
    return "";
  }
}
bool searchByName(string fileListFilename, string searchString, bool casesensitive, string filetype, bool fullpath) {

  boost::timer::auto_cpu_timer t;
  boost::iostreams::mapped_file mmap;
  try {

  // Load file
   mmap.open(fileListFilename, boost::iostreams::mapped_file::readonly);
  }
  catch (std::exception& e)
  {
    // remind the user that the ISBNs must match and prompt for another pair
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
  resuts_file <<  sernum + "\n";
  resuts_file <<  volName + "\n";
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
 
    std::cout << "making lowercase copy for caseinsenstive search: "   << "\n";
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
  while (f2 && f2 != end  ) {
    if (f2 = static_cast<const char*>(memchr(f2, searchChar1, end - f2))) 
    {

      // check for search string
      if (  ((end - f2) > searchStringLen) && memcmp(searchCharArray, f2, searchStringLen) == 0)
      {
        // locate search result line start and end
        linestartPoint = lineEndPoint = f + (f2-beginning2); // flip to search from original in case of caseinsensitive search
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

         bool  filter ;
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
         } else
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
                 filename = size_filename.substr(endOfSizeLocation+1);
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
               resuts_file << dirLineString << "; " << resultString <<   "\n";
             
             f2 = beginning2 + ( lineEndPoint - beginning); // continue searching from the end of last result line
         }
      }
      f2++;
    }
  }

  cout << "search results found: " << hitcount << /*searchResults.size() <<*/ endl;

  if (hitcount == 0)
    resuts_file << "NOTHING FOUND "  << "\n";
  return true;
}

int replace(std::string& str, const std::string& from, const std::string& to) {
  size_t start_pos = str.find(from);
  int count = 0;
  if (start_pos == std::string::npos)
    return 0;
  while (start_pos != std::string::npos) {

    count++;

  str.replace(start_pos, from.length(), to);
  start_pos = str.find(from, start_pos+1);
  }
  return count;
}

bool searchFilesByFolderName(string fileListFilename, string searchString, bool casesensitive, string filetype, bool fullpath) {

  boost::timer::auto_cpu_timer t;
  boost::iostreams::mapped_file mmap;
  try {

    // Load file
    mmap.open(fileListFilename, boost::iostreams::mapped_file::readonly);
  }
  catch (std::exception& e)
  {
    // remind the user that the ISBNs must match and prompt for another pair
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

  cout << hitcount <<  " directories and their contents found  " << /*searchResults.size() <<*/ endl;

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

bool fexists(string filenameToCheck)
{
  auto filenameToCheckChar = reinterpret_cast<char *>(alloca(filenameToCheck.size() + 1));
  memcpy(filenameToCheckChar, filenameToCheck.c_str(), filenameToCheck.size() + 1);

  ifstream ifile(filenameToCheckChar);
  return static_cast<bool>( ifile);
}

int main(int argc, char *argv[]) 
{
   boost::timer::auto_cpu_timer t;
   opt::options_description desc("All options: (search and listingfiles required)");
   //_crtBreakAlloc = 894;
   desc.add_options()
     ("search,s", opt::value<std::string>(), "search string")
     ("casesensitive,c", opt::value<bool>()->default_value(false),"casesensitive")
     ("filetype,s", opt::value<std::string>()->default_value("file"), "file type to search (file, directory or both)")
     ("resultfile,r",
     opt::value<std::string>()->default_value("search_resultsfile.csv"),
     "results output file name")
     ("listingfiles,l", opt::value<std::vector<std::string> >()->multitoken(),
     "file listings")
     ("overwrite,c", opt::value<bool>()->default_value(false), "overwrite results file by default")
     ("fullpath,c", opt::value<bool>()->default_value(false), "fullpath included in results")
     ("searchby,s", opt::value<std::string>()->default_value("filename"), "searchtype (filename, by_directory_name)")

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
   bool casesensitive;
   if (!vm["casesensitive"].empty()) {
     casesensitive = vm["casesensitive"].as<bool>();
   }
 
   bool overwrite;
   if (!vm["overwrite"].empty()) {
     overwrite = vm["overwrite"].as<bool>();
   }

   bool fullpath;
   if (!vm["fullpath"].empty()) {
     fullpath = vm["fullpath"].as<bool>();
   }
   
   std::string filetype;
   // extracting search word from command line options
   if (!vm["filetype"].empty()) {
     filetype = vm["filetype"].as<std::string>();
   }
   std::string searchby;
   // extracting search word from command line options
   if (!vm["searchby"].empty()) {
     searchby = vm["searchby"].as<std::string>();
   }
   

   // extracting search results file file name from command line options
   std::string resultsFilename = vm["resultfile"].as<std::string>();

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
 

   std::string fileListFilename; // = "E:/adm/hdlist/stuff/LACIESHARE_12012015-113107_30K_EKAARIVIA.txt"; 
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
       return 1;
     }
     else
     {
       std::cout << "over"; 
       break;
     }
     renameSuffix++;
     
   }
   std::cout << "writing results to " << resultsFilename << "\n";
   resuts_file.open(resultsFilename);
   resuts_file << "searchString: " << searchString <<  "\n";
   for (string fileListFilename : listFiles) {
     //cout << fileListFilename << endl;
     if (searchby == "filename")
       searchByName(fileListFilename, searchString, casesensitive, filetype, fullpath);
     else
       searchFilesByFolderName(fileListFilename, searchString, casesensitive, filetype, fullpath);
   }

   resuts_file.close();
 
}
 