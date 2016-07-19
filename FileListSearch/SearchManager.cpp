#include "stdafx.h"
#include "SearchManager.h"
#include "Search.h"
#include "FileSearch.h"
#include "SearchResult.h"
#include "DuplicateSearch.h"
#include "SearchByFileExtension.h"
#include "SizeSearch.h"
#include "FolderContentSearch.h"
#include "CdTreeSearch.h"

#include <iostream>   
using std::string;
using std::cout;
using std::endl;

namespace file_list_search {
  SearchManager::SearchManager(SearchOptions so, SearchResult  & searchRes) :
    searchOptions(so), searchresult(searchRes) {
  }


  SearchManager::~SearchManager()
  {
  }

  bool SearchManager::run() {

    file_list_search::Search * search;

    if (searchOptions.searchby == "duplicate"){
      cout << "starting duplicate search " << endl;

      searchresult.searchType = file_list_search::SearchResult::search_class::dupli;
      searchOptions.casesensitive = true; // force case sensitive (no need for case insensitive)
      search = new file_list_search::DuplicateSearch(searchOptions, searchresult);

    }
    else if (searchOptions.searchby == "cdtree")
    {
      cout << "starting  cdtree extension search " << endl;

      searchresult.searchType = file_list_search::SearchResult::search_class::cdtree;
      search = new file_list_search::CdTreeSearch(searchOptions, searchresult);
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
      searchresult.searchType = file_list_search::SearchResult::search_class::size;
      search = new file_list_search::SizeSearch(searchOptions, searchresult);
    }

    else if (searchOptions.searchby == "filename")
    {
      cout << "starting  filename search " << endl;

      searchresult.searchType = file_list_search::SearchResult::search_class::filename;
      search = new file_list_search::FileSearch(searchOptions, searchresult);
    }

    else if (searchOptions.searchby == "by_directory_name")
    {
      cout << "starting search all directory contents" << endl;

      searchresult.searchType = file_list_search::SearchResult::search_class::bydir;
      search = new file_list_search::FolderContentSearch(searchOptions, searchresult);
    }
    else {
      cout << "ERROR!! searchby search function option not valid! " << endl;
      cout << "search function (--searchby / -b option) needs to be one of the following: filename, by_directory_name, duplicate or cdtree" << endl;
      std::cout << searchOptions.desc << "\n";
      return false;
    }

    if (!search->initializeSearch())
      return false;  //

    return true;
  }
}
