#include "stdafx.h"
#include "CdTreeSearch.h"
#include "search_constants.h"
#include "utility_funcs.h"

namespace file_list_search {

  CdTreeSearch::CdTreeSearch(SearchOptions &so, SearchResult  & searchRes) :
    Search(so, searchRes) {
  }


  CdTreeSearch::~CdTreeSearch()
  {
  }

  void CdTreeSearch::runSearch(Storage * storage){
  
    // loop through all potential search hits
    while (storage->f2 && storage->f2 != storage->end) {
      if (storage->f2 = static_cast<const char*>(memchr(storage->f2, searchOptions.searchChar1, storage->end - storage->f2)))
      {
        ++(storage->linecount); // in this case num of occurences of searchOptions.searchChar1
        // check for search string
        if (((storage->end - storage->f2) > searchOptions.searchStringLen) && memcmp(searchOptions.searchCharArray, storage->f2, searchOptions.searchStringLen) == 0)
        {
          ++(storage->filecount);
          // locate search result line start and end
          storage->linestartPoint = storage->lineEndPoint = storage->f + (storage->f2 - storage->beginning2); // flip to search from original in case of caseinsensitive search
          while ((storage->linestartPoint - storage->beginning) > 0 && memcmp(newLineChar, storage->linestartPoint, 1) != 0)
          {
            --(storage->linestartPoint);
          }
          ++(storage->linestartPoint); // step forward to drop "\n"
          while ((storage->end - storage->lineEndPoint) > 0 && memcmp(newLineChar, storage->lineEndPoint, 1) != 0)
          {
            ++(storage->lineEndPoint);
          }
          --(storage->lineEndPoint); //  step back to drop "\n"

          bool  filter;
          // filter out directories and abnormaly long results
          if (searchOptions.filetype == "file") {
            // filter in only files    
            filter = memcmp("F,", storage->linestartPoint, 2) == 0;
          }
          else if (searchOptions.filetype == "dir" || searchOptions.filetype == "folder" || searchOptions.filetype == "directory")
          {
            //  only directories
            filter = memcmp("D,", storage->linestartPoint, 2) == 0;
          }
          else
          {
            // filter out  cd/dvd names  
            filter = memcmp("C,", storage->linestartPoint, 2) != 0;
          }
          if (filter  && storage->lineEndPoint - storage->linestartPoint < 1000)
          {
            string resultString(storage->linestartPoint, storage->lineEndPoint - storage->linestartPoint);
            //searchResults.push_back(resultString);
            ++hitcount;
            //cout << " resultString:  " << resultString << endl;

            // search  and fetch the containging directory name
            storage->dirStartPoint = storage->linestartPoint;
            while ((storage->dirStartPoint - storage->beginning) > 0 && memcmp("\nD,", storage->dirStartPoint, 3) != 0)
            {
              --(storage->dirStartPoint);
            }
            storage->dirStartPoint++;
            storage->dirEndPoint = storage->dirStartPoint;
            while ((storage->end - storage->dirEndPoint) > 0 && memcmp(newLineChar, storage->dirEndPoint, 1) != 0)
            {
              ++(storage->dirEndPoint);
            }
            --(storage->dirEndPoint); //  step back to drop "\n"

            // capture  the directory  line
            string dirLineString(storage->dirStartPoint, storage->dirEndPoint - storage->dirStartPoint);


            const char * cdStartPoint = storage->dirStartPoint;
            while ((cdStartPoint - storage->beginning) > 0 && memcmp("\nC,", cdStartPoint, 3) != 0)
            {
              --cdStartPoint;
            }
            cdStartPoint++;
            const char *  cdEndPoint = cdStartPoint;
            while ((storage->end - cdEndPoint) > 0 && memcmp(newLineChar, cdEndPoint, 1) != 0)
            {
              ++cdEndPoint;
            }
            --cdEndPoint; //  step back to drop "\n"

            cdStartPoint--; // step backward
            string cdLineString(cdStartPoint, cdEndPoint - cdStartPoint);


            string cdname = cdLineString.substr(3);
            trim(cdname);
            std::size_t endOfname = cdname.find(",");
            cdname = cdname.substr(0, endOfname);
            string dirname = dirLineString.substr(3);
            std::size_t begOfname = dirname.find(",");

            dirname = dirname.substr(begOfname + 1);
            endOfname = dirname.find(",");
            dirname = dirname.substr(0, endOfname);
            string fileinfo = resultString.substr(3);

            begOfname = fileinfo.find(",");
            fileinfo = fileinfo.substr(begOfname + 1);

            searchResult.resuts_file << cdname << "; " << dirname << "; " << fileinfo << "\n";

            // resuts_file << cdLineString << "; " << dirLineString << "; " << resultString << "\n";

            storage->f2 = storage->beginning2 + (storage->lineEndPoint - storage->beginning); // continue searching from the end of last result line
          }
        }
        storage->f2++;
      }
    }
    hitcount;

  }

}