// FileListSearch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "utility_funcs.h"
#include "searchoptions.h"
#include "searchfromcdtree.h"
#include "searchbyname.h"
#include "searchfilesbyfoldername.h"
#include "searchbyfileextensiononly.h"
#include <boost/program_options.hpp>
#include <boost/program_options/errors.hpp>
#include <boost/iostreams/device/mapped_file.hpp> // for mmap
#include <boost/filesystem/operations.hpp> 

#include <algorithm>  // for std::find
#include <functional> 
#include <iostream>   // for std::cout
#include <cstring>
#include <boost/timer/timer.hpp>
#include <string>
#include <sstream> 
#include <cmath>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <regex>
#include <exception> 
#include <cctype>
#include <locale>

using std::string;
using std::cout;
using std::cin;
using std::ifstream;
using namespace boost::filesystem;
namespace opt = boost::program_options;


using std::endl;

std::ofstream resuts_file;



// obsolete unused function for benchmarking
bool search(string fileListFilename, string searchString) {

  //boost::timer::auto_cpu_timer t;

  // Load file
  boost::iostreams::mapped_file mmap(fileListFilename, boost::iostreams::mapped_file::readonly);
  const char * f = mmap.const_data();
  const char * beginning = f;
  auto end = f + mmap.size();
  auto size2 = end - f;

  // load file contnts to string
  string filecontents(beginning, size2);
  //if (timerProfiling)
  //{
  //  t.report();
  //  t.stop();
  //  t.start();
  //}

  // search
  std::size_t found = filecontents.find(searchString);
  std::vector<string> searchResults;
  if (found == std::string::npos)
  {
    cout << "No search results" << endl;
    return false;

  }
  string currentDir;
  while (found != std::string::npos) {

    // locate search result line start and end
    std::size_t lineEndIndx = filecontents.find("\r", found + 1);
    std::size_t lineStart = filecontents.rfind('\n', found) + 1;

    if (lineStart == std::string::npos)
      lineStart = 0;

    int lineLength = lineEndIndx - lineStart;
    string resultRow = filecontents.substr(lineStart, lineLength);
    std::size_t dirfound = resultRow.find("Directory of");
    std::size_t dirfound2 = resultRow.find("<DIR>");

    // filter out Directories
    if (dirfound == std::string::npos && dirfound2 == std::string::npos)
    {
      // fetch the containging directory
      std::size_t previousDirectory = filecontents.rfind("Directory of", lineStart) + 13;
      std::size_t dirlineEndIndx = filecontents.find("\r", previousDirectory + 1);
      currentDir = filecontents.substr(previousDirectory, dirlineEndIndx - previousDirectory);
      searchResults.push_back(resultRow);
      resuts_file << currentDir << "; " << resultRow << "\n";
    }

    found = filecontents.find(searchString, lineStart + lineLength);

  }

  cout << "search results found: " << searchResults.size() << endl;

  return true;
}


void checkWildCardInFileListings(std::vector<string> &listFiles) {
  std::size_t wildcardPos = listFiles.at(0).find("*");

  // if wild card in the first parameter list all files on the dir
  if (listFiles.size() == 1 && wildcardPos != std::string::npos){

    string listingDir = listFiles.at(0).substr(0, wildcardPos);
    listFiles.clear();
    boost::filesystem::directory_iterator begin(listingDir);
    boost::filesystem::directory_iterator end;

    for (; begin != end; ++begin) {

      boost::filesystem::file_status fs =
        begin->status();

      switch (fs.type()) {
      case boost::filesystem::regular_file:
        std::cout << "listing file:  ";
        std::cout << begin->path() << '\n';

        listFiles.push_back(begin->path().string());
        break;

      default:
        //std::cout << "OTHER      ";
        break;
      }
    }
  }
}


bool checkExistingFile(std::string & resultsFilename, const bool overwrite) {

  std::string resultsFilenameOriginal = resultsFilename;
  int renameSuffix = 1;
  while (fexists(resultsFilename) && !overwrite)
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

void getSizeOperands(SearchOptions & searchOptions){

  std::regex greaterSmallerReg1("^([><gs]\\d+[kmgtKMGT]?)");
  string greaterSmaller1 = get_match(searchOptions.sizeFilter, greaterSmallerReg1);

  std::regex greaterSmallerReg2("^[><gs]\\d+[kmgtKMGT]?\\s*([><gs]\\d+[kmgtKMGT]?)");
  string greaterSmaller2 = get_match(searchOptions.sizeFilter, greaterSmallerReg2);

  std::regex numvalueReg("(\\d+)");
  std::regex metricPrfixReg("([kmgtKMGT]?)$");

  string numValueStr1 = get_match(greaterSmaller1, numvalueReg);
  int sizeValue1 = boost::lexical_cast<int>(numValueStr1);
  string metricPrefix1 = get_match(greaterSmaller1, metricPrfixReg);
  metricPrefix1 = tolower(metricPrefix1[0]);
  int metricMult1 = metricPrefix2Integer(metricPrefix1[0]);

  int sizeValue2;
  int metricMult2;
  if (greaterSmaller2 != "") {
    string numValueStr2 = get_match(greaterSmaller2, numvalueReg);
    sizeValue2 = boost::lexical_cast<int>(numValueStr2);
    string metricPrefix2 = get_match(greaterSmaller2, metricPrfixReg);
    metricPrefix2 = tolower(metricPrefix2[0]);
    metricMult2 = metricPrefix2Integer(metricPrefix2[0]);
  }

  if (greaterSmaller1[0] == '>' || greaterSmaller1[0] == 'g'){
    searchOptions.sizeOperand.greaterThan = metricMult1 * sizeValue1;
    searchOptions.sizeOperand.greaterThanActive = true;
    if (greaterSmaller2 != "" && (greaterSmaller2[0] == '<' && greaterSmaller2[0] == 's')){
    searchOptions.sizeOperand.smallerThan = metricMult2 * sizeValue2;
    searchOptions.sizeOperand.smallerThanActive = true;
    }
  }
  else {
    searchOptions.sizeOperand.smallerThan = metricMult1 * sizeValue1;
    searchOptions.sizeOperand.smallerThanActive = true;
    bool greater = greaterSmaller2[0] == 'g';
    bool greater1 = greaterSmaller2[0] == '>';
    if (greaterSmaller2 != "" && (greater1 || greater))
      searchOptions.sizeOperand.greaterThan = metricMult2 * sizeValue2;
      searchOptions.sizeOperand.greaterThanActive = true;
  }
}

bool getParameters(int argc, char *argv[], SearchOptions &searchOptions){

  //opt::options_description desc("All options: (search and listingfiles required)");
  //searchOptions.desc = desc.c;
  //_crtBreakAlloc = 894;
  searchOptions.desc.add_options()
    ("search,s", opt::value<std::string>(), "search string")
    ("listingfiles,l", opt::value<std::vector<std::string> >()->multitoken(),
    "file listings")
    ("casesensitive,c", opt::value<bool>()->default_value(false), "casesensitive search true/false")
    ("filetype,f", opt::value<std::string>()->default_value("file"), "file type to search (file, directory or both)")
    ("resultfile,r",
    opt::value<std::string>()->default_value("auto"),
    "results output file name (auto means automatically generated file name with format: results_for_searchTerm_searchterm.txt )")
    ("year,y", opt::value<std::string>()->default_value(""), "filter by year: yyyy")
    ("monthyear,m", opt::value<std::string>()->default_value(""), "filter by monthyear with format mm.yyyy")
    ("date,d", opt::value<std::string>()->default_value(""), "filter by date with format dd.mm.yyyy")
    ("size,z", opt::value<std::string>()->default_value(""), 
    "filter by size. Example: -z\">1M\", -z\"<100k\" or -z\"g1M\" -z\"s100k\". \
     You can also make a range like this -z\">100k <2M\" (use allways double quotes!)")
    ("fullpath,u", opt::value<bool>()->default_value(false), "fullpath included in results")
    ("searchby,b", opt::value<std::string>()->default_value("filename"), 
    "searchtype (filename, by_directory_name, duplicate or cdtree)\n \
    filename = regular file name search,\n \
    by_directory_name = list all files in directories that match search term,\n \
    duplicate = search for file duplicates (date, size, filename match)\n \
    cdtree = search cdtree format csv file")
    ("fileextension,x", opt::value<std::string>()->default_value("*"), "file extension filter for search default to any")
    ("fileextensioncase,n", opt::value<bool>()->default_value(false), "file extension filter casesensitive defaults to false (= case insensitive)")     
    ("timestamp,e", opt::value<bool>()->default_value(false), "include timestamp in auto generated result file name")
    ("overwrite,o", opt::value<bool>()->default_value(false), "overwrite results file by default in case it exists")
    ("cdtreefilenameflag,t", opt::value<std::string>()->default_value("cdtree"), "if this string found in the file name switch to cdtree search function")
    ("exposeoptions,i", opt::value<bool>()->default_value(false), "list all option values to terminal")
    
    ("help", "produce help message");

  opt::variables_map vm;
  try {
    opt::store(opt::parse_command_line(argc, argv, searchOptions.desc), vm);
  }
  catch (std::exception& e)
  {
    //  
    std::cerr << "\n\nERROR in options!!!  check your options: " << e.what() << "\n\n";
    std::cout << searchOptions.desc << "\n";
    return false;
  }
  opt::notify(vm);

  if (vm.count("help")) {
    std::cout <<  "Usage: FileListSearch options\n";
    std::cout << "--search and --listingfiles options required\n";
    std::cout << searchOptions.desc << "\n";
    searchOptions.success = false;
    return false;
  }

  opt::notify(vm);

  // extracting search word from command line options
  if (!vm["search"].empty()) {
    searchOptions.searchString = vm["search"].as<std::string>();
  }
  else {
    std::cout << "Search option required:" << "\n";
    std::cout << searchOptions.desc << "\n";
    searchOptions.success = false;
    return false;
  }

  searchOptions.casesensitive = vm["casesensitive"].as<bool>();
  searchOptions.overwrite = vm["overwrite"].as<bool>();
  searchOptions.fullpath = vm["fullpath"].as<bool>();
  searchOptions.filetype = vm["filetype"].as<std::string>();
  searchOptions.cdtreefilenameflag = vm["cdtreefilenameflag"].as<std::string>();
  searchOptions.fileExtension = vm["fileextension"].as<std::string>();
  searchOptions.fileExtensionCheckCaseSensitive = vm["fileextensioncase"].as<bool>();
  searchOptions.exposeOptions = vm["exposeoptions"].as<bool>();
  
  searchOptions.year = vm["year"].as<std::string>();
  searchOptions.date = vm["date"].as<std::string>();
  searchOptions.monthYear = vm["monthyear"].as<std::string>();
  searchOptions.sizeFilter = vm["size"].as<std::string>();
 
  std::regex  sizereg1("^[<>gs]\\d+[MmKkGgTt]?(\\s*[<>sg]\\d+[MmKkGgTt]?)?\\s*$");

  if (searchOptions.sizeFilter != "" && !std::regex_match(searchOptions.sizeFilter, sizereg1)) {
    std::cout << "\n\nAttention!!!\n\nSizeFilter option needs to be in this format: < or > number and optional metric prefix (k,M,G or T)" << std::endl;
    std::cout << "Please try again like this example: -z\">100\" or -z\"<20M\" " << std::endl;
    std::cout << "Or for a range like this example: -z\">100k <20M\""  << std::endl;
    searchOptions.success = false;
    return false;
  }

  if (searchOptions.sizeFilter != "") {
    getSizeOperands(searchOptions);
  }

  std::regex  datereg1("^\\d\\d\\.\\d\\d\\.\\d\\d\\d\\d$");

    if (searchOptions.date != "" && !std::regex_match(searchOptions.date, datereg1)) {
    std::cout << "\n\nAttention!!!\n\nDate option needs to be in this format: dd.mm.yyyy" << std::endl;
    std::cout << "Please try again like this example: -d17.08.2014" << std::endl;
    searchOptions.success = false;
    return false;
  }

    std::regex  monthYearreg1("^\\d\\d\\.\\d\\d\\d\\d$");

    if (searchOptions.monthYear != "" && !std::regex_match(searchOptions.monthYear, monthYearreg1)) {
      std::cout << "\n\nAttention!!!\n\nMonthYear filter option needs to be in this format: mm.yyyy" << std::endl;
      std::cout << "Please try again like this example: -m08.2014" << std::endl;
      searchOptions.success = false;
      return false;
    }

    std::regex  yearreg1("^\\d\\d\\d\\d$");

    if (searchOptions.year != "" && !std::regex_match(searchOptions.year, yearreg1)) {
      std::cout << "\n\nAttention!!!\n\nYear filter option needs to be in this format: yyyy" << std::endl;
      std::cout << "Please try again like this example: -y2014" << std::endl;
      searchOptions.success = false;
      return false;
    }

    searchOptions.searchby = vm["searchby"].as<std::string>();

    if (searchOptions.searchby == "dup")
      searchOptions.searchby = "duplicate";

  searchOptions.timestampInAutoName = vm["timestamp"].as<bool>();

  // extracting search results file file name from command line options
  searchOptions.resultsFilename = vm["resultfile"].as<std::string>();
  //searchOptions.timestampInAutoName = true;
  
  if (searchOptions.resultsFilename == "auto")
  {
    string resultfileTermString = searchOptions.searchString;
    if (resultfileTermString == "*")
      resultfileTermString = "any";
    if (searchOptions.fileExtension != "*")
      resultfileTermString += "_with_fileExt_" + searchOptions.fileExtension;
    string timeString = "";
    if (searchOptions.timestampInAutoName) {
      //boost::posix_time::ptime nowTime = boost::posix_time::second_clock::local_time();

      boost::posix_time::ptime my_ptime = boost::posix_time::second_clock::universal_time();
      boost::local_time::local_date_time ldt(my_ptime, s_timezone);

      timeString = mydateformat(ldt);
      //to_simple_string(nowTime);
      searchOptions.resultsFilename = "results_for_searchTerm_" + resultfileTermString + "_" + timeString + ".txt";
    }
    else
    {
      searchOptions.resultsFilename = "results_for_searchTerm_" + resultfileTermString + ".txt";
    }

  }
  // extracting file listing path names from command line options

  if (!vm["listingfiles"].empty() &&
    (searchOptions.listFiles = vm["listingfiles"].as<std::vector<string> >()).size() > 0) {
    // good to go
  }
  else {
    std::cout << searchOptions.desc << "\n";
    searchOptions.success = false;
    return false;
  }
  searchOptions.success = true;
  //bool fullexposureOfSearchOptions = true;
  if (searchOptions.exposeOptions)
  {


    cout << "searchString: " << searchOptions.searchString << endl;
    cout << "casesensitive: " << searchOptions.casesensitive << endl;
    cout << "overwrite: " << searchOptions.overwrite << endl;
    cout << "fullpath: " << searchOptions.fullpath << endl;
    cout << "filetype: " << searchOptions.filetype << endl;
    cout << "cdtreefilenameflag: " << searchOptions.cdtreefilenameflag << endl;
    cout << "fileExtension : " << searchOptions.fileExtension << endl;
    cout << "fileExtensionCheckCaseSensitive : " << searchOptions.fileExtensionCheckCaseSensitive << endl;
    cout << "year : " << searchOptions.year << endl;
    cout << "date : " << searchOptions.date << endl;
    cout << "monthYear : " << searchOptions.monthYear << endl;
    cout << "sizeFilter: " << searchOptions.sizeFilter << endl;
    cout << "searchby: " << searchOptions.searchby << endl;
    cout << "timestampInAutoName: " << searchOptions.timestampInAutoName << endl;
    cout << "resultsFilename: " << searchOptions.resultsFilename << endl;
    //cout << "listFiles: " <<  searchOptions.listFiles << endl;
    //cout << "sizeOperand.greaterThanActive: " << searchOptions.sizeOperand.greaterThanActive << endl;
    //cout << "sizeOperand.smallerThanActive: " << searchOptions.sizeOperand.smallerThanActive << endl;
    if (searchOptions.sizeOperand.greaterThanActive)  
      cout << "size filter greaterThan: " << searchOptions.sizeOperand.greaterThan << endl;
    if (searchOptions.sizeOperand.smallerThanActive)
      cout << "size filter smallerThan: " << searchOptions.sizeOperand.smallerThan << endl;

  }
  return true;
}

int main(int argc, char *argv[])
{
  boost::timer::auto_cpu_timer t;
  SearchOptions searchOptions = SearchOptions();

  if (!getParameters(argc, argv, searchOptions))
    return 1;

  checkWildCardInFileListings(searchOptions.listFiles);

  std::string fileListFilename; // = "E:/adm/hdlist/stuff/LACIESHARE_12012015-113107_30K_EKAARIVIA.txt"; 

  if (!checkExistingFile(searchOptions.resultsFilename, searchOptions.overwrite))
    return 1;

  std::cout << " \n";
  resuts_file.open(searchOptions.resultsFilename);
  resuts_file << "searchString: " << searchOptions.searchString << "\n";
  for (string fileListFilename : searchOptions.listFiles) {
    //cout << fileListFilename << endl;
   
   
    std::size_t cdtreeFlagPos  = fileListFilename.find(searchOptions.cdtreefilenameflag);
    if (cdtreeFlagPos != std::string::npos || searchOptions.searchby == "cdtree")
      searchFromCdTree(fileListFilename, searchOptions, resuts_file);
    else if (searchOptions.searchby == "duplicate"){
      searchOptions.casesensitive = true; // force case sensitive (no need for case insensitive)
      findDups(fileListFilename, searchOptions, resuts_file);
    }
    else if (searchOptions.searchString == "*" && searchOptions.fileExtension.size() > 0 && searchOptions.fileExtension != "*")
      searchByFileExtensionOnly(fileListFilename, searchOptions, resuts_file);

    else if(searchOptions.searchby == "filename")
      searchByName(fileListFilename, searchOptions, resuts_file);
    else if (searchOptions.searchby == "by_directory_name")
      searchFilesByFolderName(fileListFilename, searchOptions, resuts_file);

    else {
      cout << "ERROR!! searchby search function option not valid! " << endl;
      cout << "search function needs to be one of the following: filename, by_directory_name or cdtree" << endl;
      std::cout << searchOptions.desc << "\n";
    }
  }

  resuts_file.close();


}
