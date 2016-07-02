#pragma once

#include "searchoptions.h"
#include <tuple>
#include <iostream> 
#include <fstream> 

namespace file_list_search {
  class SearchResult
  {
  public:
    
    SearchResult(SearchOptions searchOptions);
    
    ~SearchResult();
    bool SearchResult::checkExistingFile();
    bool SearchResult::prepareResultsFile();
    void SearchResult::reportResults();
    void SearchResult::finalize();
    SearchOptions searchOptions;
    std::ofstream resuts_file;
    std::string resultsFilename;
    int linecount  ;
    int filecount;
    int  hitcount  ;
    int dups  ;

 
    unsigned long long dupfileSizesTotal;

    std::tuple<double, char>  dupfileSizesTotalTuple; 


  };


}