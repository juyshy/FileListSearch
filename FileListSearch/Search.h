
#pragma once
#include "storage.h"
#include "SearchResult.h"

namespace file_list_search {

  using file_list_search::Storage;

  class Search
  {
  public:
    //Search();
    Search(SearchOptions &searchOptions, std::ofstream & re_file);
    ~Search();

    
    bool Search::initializeSearch();
    bool Search::prepare(Storage * storage);
    //bool Search::initilizeOptions();
    SearchOptions &searchOptions;
    std::ofstream & resuts_file;
    SearchResult * searchResult;
    std::vector<Storage *> storages;
  private:
    //void Search::execute(Storage * storage);
    std::vector<string> resultRows;
    std::vector<string> duplicates;
    std::vector<string>::iterator it;
    std::set<string> uniquerows;
    int dups = 0;
    unsigned long long dupfileSizesTotal = 0;
    int hitcount = 0;
  };

}
