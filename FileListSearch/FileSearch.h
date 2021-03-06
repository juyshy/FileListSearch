#pragma once

#include "Search.h"
#include "Storage.h"
#include "searchoptions.h"
#include "SearchResult.h"

namespace file_list_search {

  class FileSearch : public Search
  {
  public:
    FileSearch(SearchOptions &searchOptions, SearchResult  & searchRes);
    ~FileSearch();


    virtual void runSearch(Storage * storage);
  };


}