#include "stdafx.h"
#include "SearchResult.h"
#include "utility_funcs.h"
#include <iostream>  
using std::cout;
using std::cin;
using std::endl;
namespace file_list_search {
 

  SearchResult::SearchResult(SearchOptions so) : searchOptions(so) {
    resultsFilename = searchOptions.resultsFilename;
  }

  SearchResult::~SearchResult()
  {
  }


  bool SearchResult::checkExistingFile() {

    std::string resultsFilenameOriginal = resultsFilename;
    int renameSuffix = 1;
    while (fexists(searchOptions.resultsFilename) && !searchOptions.overwrite)
    {
      std::cout << "results file " << resultsFilename << " allready exists" << "\n";
      std::cout << "overwrite, rename or cancel? (o,r,c)?" << "\n";
      string mystr;

      getline(cin, mystr);
      if (mystr == "r")
      {
        string renameSuffixStr = boost::lexical_cast<string>(renameSuffix);
        resultsFilename = resultsFilenameOriginal + renameSuffixStr;

      }
      else if (mystr == "c")
      {
        std::cout << "canceling.." << "\n";
        return false;
      }
      else
      {
        std::cout << "over";
        break;
      }
      renameSuffix++;

    }
    return true;
  }


  bool SearchResult::prepareResultsFile(){
   
    if (!checkExistingFile())
       return false;

    resuts_file.open(resultsFilename);

    //resuts_file << "JEEEEE";
    //resuts_file.close();

    return true;
  }



  void SearchResult::reportResults() {

  
    //cout << "Benchmark: filtering with size: " << endl;
    //cout << "Number of results: " << hitcount << /*searchResults.size() <<*/ endl;
    
   
    
    if (searchType == dupli)
    {
      cout << "Number of rows: " << linecount << endl;
      cout << "Number of unique files: " << hitcount << endl;
      cout << "Number of dups: " << dups << endl;
    }
    else if (searchType == fileExt || searchType == filename)
    {
      cout << "Number of chars: " << linecount << endl;
      cout << "Number of files before filter: " << filecount << endl;
      cout << "Number of results: " << hitcount << endl;

    }
    else if (searchType == size)
    {
      cout << "Number of lines: " << linecount << endl;
      cout << "Number of files before filter: " << filecount << endl;
      cout << "Number of results: " << hitcount << endl;

    }
    
    else if (searchType == bydir)
    {
      cout << "Number of lines: " << linecount << endl;
      cout << "Number of files before filter: " << filecount << endl;
      cout << "Number of results: " << hitcount << endl;

    }
    else if (searchType == cdtree)
    {
      cout << "Number of potential lines: " << linecount << endl;
      cout << "Number of files before filter: " << filecount << endl;
      cout << "Number of results: " << hitcount << endl;

    }
    cout.precision(3);

    if (dupfileSizesTotal > 0)  {

        dupfileSizesTotalTuple = scaleWithMetricPrefix(dupfileSizesTotal);
      cout << "Duplicate file sizes total: ";
      cout << std::get<0>(dupfileSizesTotalTuple) << std::get<1>(dupfileSizesTotalTuple) << " (" << dupfileSizesTotal << ")  " << endl;

    } 
    //cout << "search results found: " << hitcount << /*searchResults.size() <<*/ endl;
    std::cout << "Writing results to " << searchOptions.resultsFilename << "\n";
    //if (hitcount == 0)
    //  resuts_file << "NOTHING FOUND " << "\n";
 
    finalize();
  }

  void SearchResult::finalize() {
    resuts_file.close();
  }
}
