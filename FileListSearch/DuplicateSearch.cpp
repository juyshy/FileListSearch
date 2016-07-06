#pragma once

#include "stdafx.h"
#include "DuplicateSearch.h"
#include "search_constants.h"


namespace file_list_search {

  DuplicateSearch::DuplicateSearch(SearchOptions &so, SearchResult  & searchRes) :
    Search(so, searchRes) {

  }


  DuplicateSearch::~DuplicateSearch()
  {
  }


  void DuplicateSearch::runSearch(Storage * storage) {
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

                searchResult.resuts_file << "DUP: " << dirLineString << "; " << resultString << "\n";
                dups++;
              }
            }
            else
            {
              uniquerows.insert(resultString);
              hitcount++;
              //dupCount.insert(make_pair(resultString, 1));
              //resultRows.push_back(resultString);
              //searchResult.resuts_file << resultString << "\n";
            }
          }

        }

        storage->f2 += 2;

        storage->linestartPoint = storage->f2;

      }
    }



  }

}
