// FileListSearch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "utility_funcs.h"
#include "searchoptions.h"
#include "searchfromcdtree.h"
#include "searchbyname.h"
#include "searchfilesbyfoldername.h"
#include "searchbyfileextensiononly.h"

//#include "Search.h"
//#include "Storage.h"
//#include "SearchResult.h"
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
#include <cmath>
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

//using namespace file_list_search::Storage;
namespace opt = boost::program_options;


using std::endl;




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


int main(int argc, char *argv[])
{
  boost::timer::auto_cpu_timer t;
  SearchOptions searchOptions = SearchOptions();
  if (!searchOptions.getParameters(argc, argv))
    return 1;

  //file_list_search::SearchResult searchresult(resuts_file);

  //std::string fileListFilename; // = "E:/adm/hdlist/stuff/LACIESHARE_12012015-113107_30K_EKAARIVIA.txt"; 
  //if (!checkExistingFile(searchOptions.resultsFilename, searchOptions.overwrite))
  //  return 1;


  std::ofstream resuts_file;
  //resuts_file.open(searchOptions.resultsFilename);

  //std::cout << " \n";
  //resuts_file << "searchString: " << searchOptions.searchString << "\n";

  //file_list_search::SearchResult searchresult(resuts_file);

  ////std::vector<file_list_search::Storage *> storages;

  //file_list_search::Search search = file_list_search::Search(searchOptions);
  //

  ////search.searchOptions = searchOptions;
  //search.initializeSearch();

  ///////////////////////////////

  //for (string fileListFilename : searchOptions.listFiles) {
  //  //cout << fileListFilename << endl;
  // 
   
    //std::size_t cdtreeFlagPos  = fileListFilename.find(searchOptions.cdtreefilenameflag);
    //if (cdtreeFlagPos != std::string::npos || searchOptions.searchby == "cdtree")
    //  searchFromCdTree(fileListFilename, searchOptions, resuts_file);
    //else if (searchOptions.searchby == "duplicate"){
    //  searchOptions.casesensitive = true; // force case sensitive (no need for case insensitive)
    //  findDups(fileListFilename, searchOptions, resuts_file);
    //}
    //else if (searchOptions.searchString == "*" && searchOptions.fileExtension.size() > 0 && searchOptions.fileExtension != "*")
    //  searchByFileExtensionOnly(fileListFilename, searchOptions, resuts_file);

    //else if(searchOptions.searchby == "filename")
    //  searchByName(fileListFilename, searchOptions, resuts_file);
    //else if (searchOptions.searchby == "by_directory_name")
    //  searchFilesByFolderName(fileListFilename, searchOptions, resuts_file);

    //else {
    //  cout << "ERROR!! searchby search function option not valid! " << endl;
    //  cout << "search function needs to be one of the following: filename, by_directory_name or cdtree" << endl;
    //  std::cout << searchOptions.desc << "\n";
    //}
  //}

  resuts_file.close();


}
