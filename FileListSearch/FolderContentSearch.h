#pragma once


#include "Search.h"
#include "Storage.h"
#include "searchoptions.h"
#include "SearchResult.h"

namespace file_list_search {


  class FolderContentSearch : public Search
  {
  public:
    FolderContentSearch(SearchOptions &searchOptions, SearchResult  & searchRes);
    ~FolderContentSearch();
    virtual void runSearch(Storage * storage);
  };
}

