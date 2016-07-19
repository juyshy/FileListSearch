#include "stdafx.h"
#include "SizeSearch.h"

#include "search_constants.h"
#include "utility_funcs.h"

#include <boost/timer/timer.hpp>

namespace file_list_search {

  SizeSearch::SizeSearch(SearchOptions &so, SearchResult  & searchRes) :
    Search(so, searchRes) {
  }



  SizeSearch::~SizeSearch()
  {
  }

  void SizeSearch::runSearch(Storage * storage){
  

    //bool searchBySizeOnly(string fileListFilename, SearchOptions searchOptions, std::ofstream &resuts_file) {
 
      bool timerProfiling = true;
      boost::timer::auto_cpu_timer t;
 
      // loop through all potential search hits
      while (storage->f2 && storage->f2 != storage->end) {
        if (storage->f2 = static_cast<const char*>(memchr(storage->f2, '\r', storage->end - storage->f2)))
        {
          ++(storage->linecount);
          //
          //int charsize = storage->f2 - linestartPoint + 1;
          //char * line = new char[charsize]();
          //strncpy(line, linestartPoint, charsize);
          bool  filter;
          // filter out directories 
          storage->lineEndPoint = storage->f2;
          unsigned long long size;
          bool sizeFilterCheck = false;
          filter = memcmp(dirnamestr, storage->linestartPoint, compsize) != 0
            // filter out lines containing "<DIR>"
            && memcmp(dirStr, storage->linestartPoint + 21, compsize2) != 0
            && memcmp("\r\n", storage->linestartPoint, 2) != 0
            && memcmp(" ", storage->linestartPoint, 1) != 0 // this would cover this: (sizeString != "File(s)") as well
            ;
          if (searchOptions.sizeFilterActive  /*&& linecount > 5*/ && filter) {
            ++(storage->filecount);
            const char * sizeStartPoint = storage->linestartPoint + 17; //offset after date & time
            while ((storage->lineEndPoint - sizeStartPoint) > 0 && memcmp(" ", sizeStartPoint, 1) == 0)
            {
              ++sizeStartPoint;
            }
            const char * sizeEndPoint = sizeStartPoint;
            while ((storage->lineEndPoint - sizeEndPoint) > 0 && memcmp(" ", sizeEndPoint, 1) != 0)
            {
              ++sizeEndPoint;
            }
            string sizeString(sizeStartPoint, sizeEndPoint - sizeStartPoint);
            
            size = boost::lexical_cast<long long>(sizeString);

            sizeFilterCheck = (!searchOptions.sizeOperand.greaterThanActive
              || searchOptions.sizeOperand.greaterThan < size)
              && (!searchOptions.sizeOperand.smallerThanActive
              || searchOptions.sizeOperand.smallerThan > size);



            if (sizeFilterCheck) {
              bool  monthyearCheck;
                bool yearFilterCheck;
                bool dateFilterCheck;
                bool filter2 = true;
                if (searchOptions.yearFilterActive
                  || searchOptions.monthYearFilterActive
                  || searchOptions.dateFilterActive){

                  // offset 3 in dd.mm.yyyy, mm.yyyy 7 chars long
                  monthyearCheck = memcmp(searchOptions.monthYearFilter, storage->linestartPoint + 3, 7) == 0;
                  // offset 6 in dd.mm.yyyy, yyyy 4 chars long
                  yearFilterCheck = memcmp(searchOptions.yearFilter, storage->linestartPoint + 6, 4) == 0;
                  // offset 0 in dd.mm.yyyy, yyyy 10 chars long
                  dateFilterCheck = memcmp(searchOptions.dateFilter, storage->linestartPoint, 10) == 0;
                  filter2 = (!searchOptions.monthYearFilterActive || monthyearCheck)
                    && (!searchOptions.yearFilterActive || yearFilterCheck)
                    && (!searchOptions.dateFilterActive || dateFilterCheck);
                }
           
              if (filter2) {
                string resultString(storage->linestartPoint, storage->f2 - storage->linestartPoint);

                // search  and fetch the containging directory name
                storage->dirStartPoint = storage->linestartPoint;
                while ((storage->dirStartPoint - storage->beginning) > 0 && memcmp(dirnamestr, storage->dirStartPoint, compsize) != 0)
                {
                  --(storage->dirStartPoint);
                }
                storage->dirEndPoint = storage->dirStartPoint;
                while ((storage->end - storage->dirEndPoint) > 0 && memcmp(newLineChar, storage->dirEndPoint, 1) != 0)
                {
                  ++(storage->dirEndPoint);
                }
                --(storage->dirEndPoint); //  step back to drop "\n"

                // capture only the directory name
                string dirLineString(storage->dirStartPoint + compsize, storage->dirEndPoint - storage->dirStartPoint - compsize);
                searchResult.resuts_file << dirLineString << "; " << resultString << "\n";
                hitcount++;
              }
            }
          }

          storage->f2 += 2;

          storage->linestartPoint = storage->f2;

        }
      }
    }
}