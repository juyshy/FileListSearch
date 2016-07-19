#pragma once

#include "stdafx.h"
#include "SearchByFileExtension.h"
#include "search_constants.h"
#include "utility_funcs.h"

namespace file_list_search {

  SearchByFileExtension::SearchByFileExtension(SearchOptions &so, SearchResult  & searchRes) :
    Search(so, searchRes) {

  }


  SearchByFileExtension::~SearchByFileExtension()
  {
  }

  void SearchByFileExtension::runSearch(Storage * storage) {
    // hard coded new line appended: file extension should always be at the end of file list line
    searchOptions.fileExtension += "\r\n";
    searchOptions.initExtensionChar();

    searchOptions.searchChar1 = searchOptions.fileExt[1]; //  look initially for the first letter of the extension 
    while (storage->f2 && storage->f2 != storage->end) {

      // intially searching for the searchChar1 = fileExt[1]

      if (storage->f2 = static_cast<const char*>(memchr(storage->f2, searchOptions.searchChar1, storage->end - storage->f2)))
      {
        storage->linecount++; // number of fileextension chars searchChar1 = fileExt[1]
        storage->f2--; // back one step to include the dot for comparison
        // check for search string

        //this time compare to file extension 
        bool endBoundaryCheck = (storage->end - storage->f2) > searchOptions.fileExtLen;
        bool matchString = memcmp(searchOptions.fileExt, storage->f2, searchOptions.fileExtLen) == 0;

        if (endBoundaryCheck && matchString)
        {
          ++(storage->filecount);
          // locate search result line start and end
          storage->linestartPoint = storage->f + (storage->f2 - storage->beginning2); // flip to grab result from original in case of caseinsensitive search

          while ((storage->linestartPoint - storage->beginning) > 0 && memcmp(newLineChar, storage->linestartPoint, 1) != 0)
          {
            --(storage->linestartPoint);
          }
          ++(storage->linestartPoint); // step forward to drop "\n"

          // as "\r\n" was appended to search string we know allready where the line ends
          // it's current location + fileExtLen and then - 2 which strips the "\r\n" appended from pointer; 
          // oh and then we have to flip it to the original f
          storage->lineEndPoint = storage->f + (storage->f2 - storage->beginning2 + searchOptions.fileExtLen - 2);

          bool  filter;
          
          if (searchOptions.filetype == "file") {

            unsigned long long size;

            bool sizeFilterCheck = false;
            if (searchOptions.sizeFilterActive) {
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
              size = boost::lexical_cast<unsigned long long>(sizeString);

              sizeFilterCheck = searchOptions.sizeOperand.greaterThan == -1 || searchOptions.sizeOperand.greaterThan < size;
              sizeFilterCheck = sizeFilterCheck && (searchOptions.sizeOperand.smallerThan == -1 || searchOptions.sizeOperand.smallerThan > size);
            }

            // offset 3 in dd.mm.yyyy, mm.yyyy 7 chars long
            bool  monthyearCheck = memcmp(searchOptions.monthYearFilter, storage->linestartPoint + 3, 7) == 0;
            // offset 6 in dd.mm.yyyy, yyyy 4 chars long
            bool yearFilterCheck = memcmp(searchOptions.yearFilter, storage->linestartPoint + 6, 4) == 0;
            // offset 0 in dd.mm.yyyy, yyyy 10 chars long
            bool dateFilterCheck = memcmp(searchOptions.dateFilter, storage->linestartPoint, 10) == 0;
            // filter out directories
            // filter out lines containing " Directory of "
            filter = memcmp(dirnamestr, storage->linestartPoint, compsize) != 0
              // filter out lines containing "<DIR>"
              && memcmp(dirStr, storage->linestartPoint + 21, compsize2) != 0
              && (!searchOptions.monthYearFilterActive || monthyearCheck)
              && (!searchOptions.yearFilterActive || yearFilterCheck)
              && (!searchOptions.dateFilterActive || dateFilterCheck)
              && (!searchOptions.sizeFilterActive || sizeFilterCheck);

          }
          else if (searchOptions.filetype == "dir" || searchOptions.filetype == "folder" || searchOptions.filetype == "directory")
          {
            // todo: date,year filterin here:
            // only directories
            filter = memcmp(dirStr, storage->linestartPoint + 21, compsize2) == 0;
          }
          else
          {
            // filter out directories  
            filter = memcmp(dirnamestr, storage->linestartPoint, compsize) != 0;
          }
          // filter abnormaly long results
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
              // offset 17 for size and filenames in the row this hard coded value
              // propably needs to be 
              // parametrized in internationalization
              string size_filename = resultString.substr(17);
              trim(size_filename);
              std::size_t endOfSizeLocation = size_filename.find(" ");
              string filename;

              string filesizeStr;
              int filesize;
              if (endOfSizeLocation != std::string::npos)
              {
                filename = size_filename.substr(endOfSizeLocation + 1);
                filesizeStr = size_filename.substr(0, endOfSizeLocation);
                if (filesizeStr != "<DIR>")
                  filesize = boost::lexical_cast<int>(filesizeStr);
                else
                  trim(filename);

              }
              else
              {
                throw std::runtime_error("filename not found!");
                filename = size_filename;// todo: 
              }

              searchResult.resuts_file << /*dirLineString << "; " <<*/ resultString << "; " << dirLineString << "\\" << filename << "\n";
            }
            else
              searchResult.resuts_file << dirLineString << "; " << resultString << "\n";

            storage->f2 = storage->beginning2 + (storage->lineEndPoint - storage->beginning); // continue searching from the end of last result line
          }
        }
        storage->f2 += 2; //don't get stuck!!
      }
    }
  }


}