#pragma once

#include "Search.h"
#include "Storage.h"
#include "searchoptions.h"
#include "SearchResult.h"

namespace file_list_search {

  //using file_list_search::Storage;
  class DuplicateSearch : public Search
  {
  public:
    DuplicateSearch(SearchOptions &searchOptions, SearchResult  & searchRes );
    ~DuplicateSearch();

    void runSearch(Storage * storage);

  };

}

