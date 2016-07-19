#include "stdafx.h"
#include "FileSearch.h"
#include "search_constants.h"
#include "utility_funcs.h"

namespace file_list_search {


  FileSearch::FileSearch(SearchOptions &so, SearchResult  & searchRes) :
    Search(so, searchRes) {
  }


  FileSearch::~FileSearch()
  {
  }


  void FileSearch::runSearch(Storage * storage){
    std::locale loc;
    while (storage->f2 && storage->f2 != storage->end) {
      if (storage->f2 = static_cast<const char*>(memchr(storage->f2, searchOptions.searchChar1, storage->end - storage->f2)))
      {
        storage->linecount++;
        // check for search string

        bool endBoundaryCheck = (storage->end - storage->f2) > searchOptions.searchStringLen;
        bool matchString = memcmp(searchOptions.searchCharArray, storage->f2, searchOptions.searchStringLen) == 0;

        if (endBoundaryCheck && matchString)
        {
          ++(storage->filecount);
          //if ()
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


          // offset 3 in dd.mm.yyyy, mm.yyyy 7 chars long
          bool  monthyearCheck = memcmp(searchOptions.monthYearFilter, storage->linestartPoint + 3, 7) == 0;
          // offset 6 in dd.mm.yyyy, yyyy 4 chars long
          bool yearFilterCheck = memcmp(searchOptions.yearFilter, storage->linestartPoint + 6, 4) == 0;
          // offset 0 in dd.mm.yyyy, yyyy 10 chars long
          bool dateFilterCheck = memcmp(searchOptions.dateFilter, storage->linestartPoint, 10) == 0;

          bool  filter;
          // filter out directories and abnormaly long results
          if (searchOptions.filetype == "file") {

            if (searchOptions.filterFileExt) {
              const char * fileExtensionCheckStart = storage->lineEndPoint - searchOptions.fileExtLen;
              string fileExtensionPortion(fileExtensionCheckStart, searchOptions.fileExtLen);
              if (!searchOptions.fileExtensionCheckCaseSensitive) {
                //  beginning2 + (lineEndPoint - beginning - fileExtLen);
                string fileExtensionPortionLower = "";

                // make lowercase version
                for (std::string::size_type i = 0; i < fileExtensionPortion.length(); ++i)
                  fileExtensionPortionLower += std::tolower(fileExtensionPortion[i], loc);

                searchOptions.fileExtensionCheck = fileExtensionPortionLower == searchOptions.fileExtension;// memcmp(fileExt, fileExtensionCheckStart, fileExtLen) == 0;
              }
              else {
                searchOptions.fileExtensionCheck = fileExtensionPortion == searchOptions.fileExtension;
              }
            }
            // filter out directories

            // filter out directories
            // filter out lines containing " Directory of "
            // filter out lines containing "<DIR>"

              filter = memcmp(dirnamestr, storage->linestartPoint, compsize) != 0
              && memcmp(dirStr, storage->linestartPoint + 21, compsize2) != 0
              && (!searchOptions.filterFileExt || (searchOptions.filterFileExt
              && searchOptions.fileExtensionCheck))
              && (!searchOptions.monthYearFilterActive || monthyearCheck)
              && (!searchOptions.yearFilterActive || yearFilterCheck)
              && (!searchOptions.dateFilterActive || dateFilterCheck);
              //&&  (!searchOptions.sizeFilterActive || sizeFilterCheck);;
          }
          else if (searchOptions.filetype == "dir" || searchOptions.filetype == "folder" || searchOptions.filetype == "directory")
          {
            // only directories
           
           
            filter = memcmp(dirStr, storage->linestartPoint + 21, compsize2) == 0
              && (!searchOptions.monthYearFilterActive || monthyearCheck)
              && (!searchOptions.yearFilterActive || yearFilterCheck)
              && (!searchOptions.dateFilterActive || dateFilterCheck);
          }
          else // no time filter for both!
          {
            // filter out directories  
            filter = memcmp(dirnamestr, storage->linestartPoint, compsize) != 0 
              && (!searchOptions.monthYearFilterActive || monthyearCheck)
              && (!searchOptions.yearFilterActive || yearFilterCheck)
              && (!searchOptions.dateFilterActive || dateFilterCheck);
          }
          if (filter  && storage->lineEndPoint - storage->linestartPoint < 1000)
          {
            string resultString(storage->linestartPoint, storage->lineEndPoint - storage->linestartPoint);
            //searchResults.push_back(resultString);
            ++hitcount;
            //cout << " resultString:  " << resultString << endl;

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

            // if fullpath written to results file extract filename
            if (searchOptions.fullpath) {
            
              string filename = extractFileName(resultString);
              searchResult.resuts_file << /*dirLineString << "; " <<*/ resultString << "; " << dirLineString << "\\" << filename << "\n";
            }
            else
              searchResult.resuts_file << dirLineString << "; " << resultString << "\n";

            storage->f2 = storage->beginning2 + (storage->lineEndPoint - storage->beginning); // continue searching from the end of last result line
          }
        }
        storage->f2++;
      }
    }
  }
}