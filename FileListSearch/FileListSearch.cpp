// FileListSearch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "utility_funcs.h"
#include "searchoptions.h"
#include "searchfromcdtree.h"
#include "searchbyname.h"
#include "searchfilesbyfoldername.h"
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

  //opt::options_description desc("All options: (search and listingfiles required)");
  //searchOptions.desc = desc.c;
  //_crtBreakAlloc = 894;
  searchOptions.desc.add_options()
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
    ("searchby,b", opt::value<std::string>()->default_value("filename"), "searchtype (filename, by_directory_name or cdtree)")
    ("cdtreefilenameflag,d", opt::value<std::string>()->default_value("cdt"), "if this string found in the file name switch to cdtree search function")
    
    ("timestamp,e", opt::value<bool>()->default_value(false), "include timestamp in auto generated result file name")
    ("help", "produce help message");

  opt::variables_map vm;
  try {
    opt::store(opt::parse_command_line(argc, argv, searchOptions.desc), vm);
  }
  catch (std::exception& e)
  {
    //  
    std::cerr << "\n\nERROR in options!!!  check your options: " << e.what() << "\n\n";
    std::cout << searchOptions.desc << "\n";
    return false;
  }
  opt::notify(vm);

  if (vm.count("help")) {
    std::cout <<  "Usage: FileListSearch options\n";
    std::cout << "--search and --listingfiles options required\n";
    std::cout << searchOptions.desc << "\n";
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
    std::cout << searchOptions.desc << "\n";
    searchOptions.success = false;
    return false;
  }

  searchOptions.casesensitive = vm["casesensitive"].as<bool>();
  searchOptions.overwrite = vm["overwrite"].as<bool>();
  searchOptions.fullpath = vm["fullpath"].as<bool>();
  searchOptions.filetype = vm["filetype"].as<std::string>();
  searchOptions.cdtreefilenameflag = vm["cdtreefilenameflag"].as<std::string>();

  

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
    std::cout << searchOptions.desc << "\n";
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
    std::size_t cdtreeFlagPos  = fileListFilename.find(searchOptions.cdtreefilenameflag);
    if (cdtreeFlagPos != std::string::npos || searchOptions.searchby == "cdtree")
      searchFromCdTree(fileListFilename, searchOptions, resuts_file);
    else if(searchOptions.searchby == "filename")
      searchByName(fileListFilename, searchOptions, resuts_file);
    else if (searchOptions.searchby == "by_directory_name")
      searchFilesByFolderName(fileListFilename, searchOptions, resuts_file);
  
    else {
      cout << "ERROR!! searchby search function option not valid! " << endl;
      cout << "search function needs to be one of the following: filename, by_directory_name or cdtree" << endl;
      std::cout << searchOptions.desc << "\n";
    }
  }

  resuts_file.close();


}
