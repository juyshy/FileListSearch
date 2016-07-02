#pragma once


#include "stdafx.h"
#include "Search.h"
#include "search_constants.h"
using std::cout;
using std::endl;
#include <boost/timer/timer.hpp>

namespace file_list_search {

  //Search::Search(){}
  Search::Search(SearchOptions &so, SearchResult  & searchRes, std::ofstream & re_file) : 
    searchOptions(so), resuts_file(re_file), searchResult(searchRes){
   
  }


  Search::~Search()
  {
  }


  //bool Search::initilizeOptions(){

  //}

  bool Search::initializeSearch(){

  
    for (string fileListFilename : searchOptions.listFiles) {
      file_list_search::Storage * storage = new file_list_search::Storage(fileListFilename);
      storages.push_back(storage);
    }


    searchOptions.initializeVariables();
    for (Storage * storage : storages){
      prepare(storage);
      runSearch(storage);
      totalLinecount += storage->linecount;
      totalFilecount += storage->filecount;
    }
    searchResult.dupfileSizesTotal = dupfileSizesTotal;
    searchResult.hitcount = hitcount;
    searchResult.dups = dups;
    searchResult.linecount = totalLinecount; //  
    return true;
  }

  bool Search::prepare(Storage * storage){

    bool timerProfiling = true;

    boost::timer::auto_cpu_timer t;
    storage->loadFileList();
    cout << "\nPreparing search.." << endl;
    cout << searchOptions.searchby << endl;
    storage->reportDriveMetadata(storage->f, resuts_file);
    resuts_file << ">>>>" << storage->fileListFileName + "\n";

    if (timerProfiling)
    {
      t.report();
      t.stop();
      t.start();
    }

    if (searchOptions.filetype == "both")
    {
      cout << "Searching for both files and directories " << endl;
    }
    else
      cout << "Searching for file type: " << searchOptions.filetype << endl;



    if (!searchOptions.casesensitive) { // not casesensitive make a lower copy
      storage->makeLowerCaseCopy();
    }
    else
    {
      storage->f2 = storage->f; // using the original

    }


    storage->beginning2 = storage->f2;
    storage->end = storage->f2 + storage->fileListSize2;

    storage->trimListSearchRange();

    return true;
  }

  void Search::runSearch(Storage * storage) {
    while (storage->f2 && storage->f2 != storage->end) {
      if (storage->f2 = static_cast<const char*>(memchr(storage->f2, '\r', storage->end - storage->f2)))
      {
        storage->linecount++;

        bool  filter;
        storage->lineEndPoint = storage->f2;
        bool sizeFilterCheck = false;
        bool filterEmptylines = memcmp(" ", storage->linestartPoint, 1) != 0;
        // filter out directories 
        filter = memcmp(dirnamestr, storage->linestartPoint, compsize) != 0
          // filter out lines containing "<DIR>"
          && memcmp(dirStr, storage->linestartPoint + 21, compsize2) != 0
          && memcmp("\r\n", storage->linestartPoint, 2) != 0
          && filterEmptylines;

          ;
        
        // duplicate search todo:
        // include paths for all instances in search result
        // identify duplicate directories


        if (/*sizeFilterActive  && linecount > 5 */   filter /*&& linecount < 150000*/) {

          ++(storage->filecount);

          string resultString(storage->linestartPoint, storage->f2 - storage->linestartPoint);

          size_t  numfilesrowpos = resultString.find("File(s)");
          if (numfilesrowpos == std::string::npos)
          {
            //it = find(resultRows.begin(), resultRows.end(), resultString);
            if (uniquerows.find(resultString) != uniquerows.end()){
              //duplicates.push_back(resultString);

              unsigned long long filsize;
              filsize = storage->getFileSize(storage->linestartPoint, storage->lineEndPoint);
              dupfileSizesTotal += filsize;
              if (searchOptions.sizeFilterActive   && filter) {



                sizeFilterCheck = (!searchOptions.sizeOperand.greaterThanActive
                  || searchOptions.sizeOperand.greaterThan < filsize)
                  && (!searchOptions.sizeOperand.smallerThanActive
                  || searchOptions.sizeOperand.smallerThan > filsize);

              }
              else
                sizeFilterCheck = false;

              if (!searchOptions.sizeFilterActive || sizeFilterCheck) {


                // search  and fetch the containging directory name
                storage->dirStartPoint = storage->linestartPoint;
                while ((storage->dirStartPoint - storage->beginning) > 0 && memcmp(dirnamestr, storage->dirStartPoint, compsize) != 0)
                {
                  storage->dirStartPoint--;
                }
                storage->dirEndPoint = storage->dirStartPoint;
                while ((storage->end - storage->dirEndPoint) > 0 && memcmp(newLineChar, storage->dirEndPoint, 1) != 0)
                {
                  storage->dirEndPoint++;
                }
                storage->dirEndPoint--; //  step back to drop "\n"

                // capture only the directory name
                string dirLineString(storage->dirStartPoint + compsize, storage->dirEndPoint - storage->dirStartPoint - compsize);

                resuts_file << "DUP: " << dirLineString << "; " << resultString << "\n";
                dups++;
              }
            }
            else
            {
              uniquerows.insert(resultString);
              hitcount++;
              //dupCount.insert(make_pair(resultString, 1));
              //resultRows.push_back(resultString);
              //resuts_file << resultString << "\n";
            }
          }

        }

        storage->f2 += 2;

        storage->linestartPoint = storage->f2;

      }
    }



  }

}

 