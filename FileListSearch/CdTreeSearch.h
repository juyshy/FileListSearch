#pragma once
#include "Search.h"
#include "Storage.h"
#include "searchoptions.h"
#include "SearchResult.h"

namespace file_list_search {

  class CdTreeSearch : public Search
  {
  public:
    CdTreeSearch(SearchOptions &searchOptions, SearchResult  & searchRes);
    ~CdTreeSearch();

    virtual void runSearch(Storage * storage);
  };

}