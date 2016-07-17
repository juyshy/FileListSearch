// FileListSearch.cpp : Defines the entry point for the console application.
//
#pragma once
#include "stdafx.h"
#include "utility_funcs.h"
#include "searchoptions.h"
//#include "searchfromcdtree.h"
#include "FileSearch.h"
//#include "searchfilesbyfoldername.h"


#include "Search.h"
//#include "Storage.h"
#include "SearchResult.h"
#include "DuplicateSearch.h"
#include "SearchByFileExtension.h"
#include "SizeSearch.h"
#include "FolderContentSearch.h"

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


int main(int argc, char *argv[])
{
  boost::timer::auto_cpu_timer t;
  SearchOptions searchOptions = SearchOptions();
  if (!searchOptions.getParameters(argc, argv))
    return 1;

  searchOptions.initializeVariables();

  file_list_search::SearchResult searchresult(searchOptions);
  if (!searchresult.prepareResultsFile())
  {
    return 1;

  }
  file_list_search::Search * search;

 
  if (searchOptions.searchby == "duplicate"){
    cout << "starting duplicate search " << endl;
 
    searchresult.searchType = file_list_search::SearchResult::search_class::dupli;
    searchOptions.casesensitive = true; // force case sensitive (no need for case insensitive)
      search = new file_list_search::DuplicateSearch(searchOptions, searchresult);
 
  }
  // if searchstring any and file extension option given:
  else if (searchOptions.searchString == "*" && searchOptions.fileExtension.size() > 0 && searchOptions.fileExtension != "*")
  {
      cout << "starting  file extension search " << endl;
      
      searchresult.searchType = file_list_search::SearchResult::search_class::fileExt;
      search = new file_list_search::SearchByFileExtension(searchOptions, searchresult);
 
  }

  else if (searchOptions.searchString == "*" && searchOptions.sizeFilterActive)
  {
    cout << "starting  size only search " << endl;
    searchresult.searchType = file_list_search::SearchResult::search_class::size ;
    search = new file_list_search::SizeSearch(searchOptions, searchresult);
  }

  else if (searchOptions.searchby == "filename")
  {
    cout << "starting  file extension search " << endl;

    searchresult.searchType = file_list_search::SearchResult::search_class::filename;
    search = new file_list_search::FileSearch(searchOptions, searchresult);
  }

  else if (searchOptions.searchby == "by_directory_name")
  {
    cout << "starting  file extension search " << endl;

    searchresult.searchType = file_list_search::SearchResult::search_class::bydir;
    search = new file_list_search::FolderContentSearch(searchOptions, searchresult);
  }

  if (!search->initializeSearch())
    return 1;  //
  searchresult.reportResults();
  searchresult.finalize();
  ///////////////////////////////


   
  //std::size_t cdtreeFlagPos = searchOptions. .find(searchOptions.cdtreefilenameflag);
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

  //resuts_file.close();


}
