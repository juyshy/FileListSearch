
#pragma once
#include "storage.h"
#include "SearchResult.h"

namespace file_list_search {

  using file_list_search::Storage;

  class Search
  {
  public:
    //Search();
    Search(SearchOptions searchOptions, SearchResult  & searchRes);
    ~Search();

    
    bool Search::initializeSearch( );
    bool Search::prepare(Storage * storage);
    //bool Search::initilizeOptions();
    SearchOptions searchOptions;
    //std::ofstream & resuts_file;
    SearchResult  & searchResult;
    std::vector<Storage *> storages;
    virtual void Search::runSearch(Storage * storage);

  protected:

    std::vector<string> resultRows;
    std::vector<string> duplicates;
    std::vector<string>::iterator it;
    std::set<string> uniquerows;
    unsigned long long dupfileSizesTotal = 0;
    int dups = 0;
    int hitcount = 0;
    int totalLinecount = 0;
    int totalFilecount = 0;
  private:
  };

}
