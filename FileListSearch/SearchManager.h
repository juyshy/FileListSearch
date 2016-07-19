#pragma once

#include "Search.h"
#include "Storage.h"
#include "searchoptions.h"
#include "SearchResult.h"

namespace file_list_search {
  class SearchManager
  {
  public:
    SearchManager(SearchOptions searchOptions, SearchResult  & searchRes);
    ~SearchManager();

    bool SearchManager::run();

    SearchOptions searchOptions;
    SearchResult  & searchresult;
  };


}
