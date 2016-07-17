#include "stdafx.h"
#include "FolderContentSearch.h"
#include "search_constants.h"
#include "utility_funcs.h"

namespace file_list_search {
  FolderContentSearch::FolderContentSearch(SearchOptions &so, SearchResult  & searchRes) :
    Search(so, searchRes) {
  }


  FolderContentSearch::~FolderContentSearch()
  {
  }


  void FolderContentSearch::runSearch(Storage * storage){
    std::locale loc;
    // collect all directories and their contents where search string found in directory name 
    while (storage->f2 && storage->f2 != storage->end) {
      if (storage->f2 = static_cast<const char*>(memchr(storage->f2, searchOptions.searchChar1, storage->end - storage->f2)))
      {
        storage->linecount++;
        // check for search string
        if (((storage->end - storage->f2) > searchOptions.searchStringLen) && memcmp(searchOptions.searchCharArray, storage->f2, searchOptions.searchStringLen) == 0)
        {
          storage->filecount++;
          // locate search result line start and end
          storage->linestartPoint = storage->lineEndPoint = storage->beginning + (storage->f2 - storage->beginning2); // flip to search from original in case of caseinsensitive search
          while ((storage->linestartPoint - storage->beginning) > 0 && memcmp(newLineChar, storage->linestartPoint, 1) != 0)
          {
            --(storage->linestartPoint);
          }
          ++(storage->linestartPoint); // step forward to drop "\n"
          while ((storage->end - storage->lineEndPoint) > 0 && memcmp(newLineChar, storage->lineEndPoint, 1) != 0)
          {
            ++(storage->lineEndPoint);
          }
          --(storage->lineEndPoint); //  this time no need to step back to drop "\n"
          //string resultString(storage->linestartPoint, storage->lineEndPoint - storage->linestartPoint);
          //resultString;
          // do we have a directory lissting start?
          if (memcmp(dirnamestr, storage->linestartPoint, compsize) == 0)

          {
            storage->f = storage->linestartPoint; // start searching for the next directory in the listing
            while (storage->f && storage->f != storage->end) {
              if (storage->f = static_cast<const char*>(memchr(storage->f, '\n', storage->end - storage->f)))
              {
                storage->f++;
                // check for next dir
                if (((storage->end - storage->f) > compsize) && memcmp(dirnamestr, storage->f, compsize) == 0)
                {
                  storage->f--;
                  string resultString(storage->linestartPoint, storage->f - storage->linestartPoint); // grab the whole thing

                  ++hitcount;
                  //cout << " resultString:  " << resultString << endl;
                  replace(resultString, "\r\n", "\n");

                  searchResult.resuts_file << resultString << "\n";

                  storage->f2 = storage->beginning2 + (storage->f - storage->beginning);
                  break;

                }
              }
            }
          }
        }
        storage->f2++;
      }
    }
  }

}