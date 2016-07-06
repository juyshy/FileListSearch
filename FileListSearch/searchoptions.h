#ifndef SEARCHOPTIONS_H_
#define SEARCHOPTIONS_H_

#include <boost/program_options.hpp>
#include <boost/program_options/errors.hpp>
#include <string>
#include <climits>

using std::string;

namespace opt = boost::program_options;


struct SizeFilterElements{
  unsigned long long greaterThan = 0;
  unsigned long long smallerThan = LLONG_MAX;
  bool greaterThanActive = false;
  bool smallerThanActive = false;
};

class SearchOptions
{
public:
  SearchOptions();
  ~SearchOptions();
  void SearchOptions::getSizeOperands();
  bool getParameters(int argc, char *argv[]);
  void SearchOptions::checkWildCardInFileListings();

  bool success; // success for gathering options
  string searchString;
  bool casesensitive; // is searchh casesensitive
  string filetype; // filetypes to search
  string resultsFilename; // search results written to this file
  std::vector<std::string> listFiles; // file listings path names
  bool overwrite; // overwrite by default of result file allready exists
  bool fullpath; // include full paths in the results
  string searchby; // search function
  bool timestampInAutoName;
  opt::options_description desc;
  string cdtreefilenameflag;
  string fileExtension;
  bool fileExtensionCheckCaseSensitive;
  string year;
  string date;
  string monthYear;
  string sizeFilter;
  SizeFilterElements sizeOperand;
  bool exposeOptions;

  void SearchOptions::initializeVariables();
  bool sizeFilterActive;


  int searchStringLen;
  char * searchCharArray;
  bool filterFileExt = false;
  int fileExtLen ;
  char * fileExt; 
 
  bool fileExtensionCheck = false; // actual test variable initial value

  std::locale loc;
  char searchChar1 ; //  look initially for the first letter of the extension 
  
  char * dateFilter;
  char * yearFilter;
  char * monthYearFilter ;
  bool dateFilterActive ;
  bool monthYearFilterActive ;
  bool yearFilterActive ;

private:

};


#endif SEARCHOPTIONS_H_