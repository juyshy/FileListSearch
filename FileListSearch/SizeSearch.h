#pragma once

#include "Search.h"
#include "Storage.h"
#include "searchoptions.h"
#include "SearchResult.h"

namespace file_list_search {

  class SizeSearch : public Search
  {
  public:
    SizeSearch(SearchOptions &searchOptions, SearchResult  & searchRes);
    ~SizeSearch();
    
    virtual void runSearch(Storage * storage);

  };
}

