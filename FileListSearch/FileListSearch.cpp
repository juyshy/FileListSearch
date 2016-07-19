// FileListSearch.cpp : Defines the entry point for the console application.
//
#pragma once
#include "stdafx.h"
#include "utility_funcs.h"
#include "searchoptions.h"
//#include "searchfromcdtree.h"
#include "FileSearch.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


#include "Search.h"
//#include "Storage.h"
#include "SearchResult.h"
#include "DuplicateSearch.h"
#include "SearchByFileExtension.h"
#include "SizeSearch.h"
#include "FolderContentSearch.h"
#include "CdTreeSearch.h"

#include "SearchManager.h"

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
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  //_crtBreakAlloc = 6227;
  boost::timer::auto_cpu_timer t;


  // process search options

  SearchOptions searchOptions = SearchOptions();
  if (!searchOptions.getParameters(argc, argv))
    return 1;
  searchOptions.initializeVariables();


  // prepare search result object

  file_list_search::SearchResult searchresult(searchOptions);
  if (!searchresult.prepareResultsFile())
  {
    return 1;
  }

  // run search

  file_list_search::SearchManager searchmanager(searchOptions, searchresult);
  if (!searchmanager.run())
  {
    return 1;
  }

  // results:
  searchresult.reportResults();


  _CrtDumpMemoryLeaks();

}
