#ifndef SEARCHOPTIONS_H_
#define SEARCHOPTIONS_H_

#include <boost/program_options.hpp>
#include <boost/program_options/errors.hpp>
#include <string>
using std::string;

namespace opt = boost::program_options;


struct SizeFilterElements{
  int greaterThan = -1;
  int smallerThan = -1;
};

class SearchOptions
{
public:
  SearchOptions() {
    opt::options_description desc("All options: (search and listingfiles required)");
  }
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
};


#endif SEARCHOPTIONS_H_