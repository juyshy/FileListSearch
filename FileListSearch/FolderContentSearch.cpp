#include "stdafx.h"
#include "FolderContentSearch.h"
#include "search_constants.h"
#include "utility_funcs.h"

namespace file_list_search {
  FolderContentSearch::FolderContentSearch(SearchOptions &so, SearchResult  & searchRes) :
    Search(so, searchRes) {
  }


  FolderContentSearch::~FolderContentSearch()
  {
  }


  void FolderContentSearch::runSearch(Storage * storage){
    std::locale loc;
  }

}