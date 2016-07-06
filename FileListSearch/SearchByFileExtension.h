#pragma once
#include "Search.h"
#include "Storage.h"
#include "searchoptions.h"
#include "SearchResult.h"

namespace file_list_search {

  class SearchByFileExtension : public Search
  {
  public:
    SearchByFileExtension(SearchOptions &searchOptions, SearchResult  & searchRes);
    ~SearchByFileExtension();

    void runSearch(Storage * storage) override;
  };

}
