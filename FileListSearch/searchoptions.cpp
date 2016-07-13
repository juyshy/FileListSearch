#include "stdafx.h"
#include "searchoptions.h"
#include "utility_funcs.h"
#include <boost/filesystem/operations.hpp> 
#include <iostream>  
#include <regex>
using std::cout;
using std::endl;
 
 
  SearchOptions::SearchOptions()
  {
    opt::options_description desc("All options: (search and listingfiles required)");
  }


  SearchOptions::~SearchOptions()
  {
  }


  void SearchOptions::getSizeOperands(){

    std::regex greaterSmallerReg1("^([><gs]\\d+[kmgtKMGT]?)");
    string greaterSmaller1 = get_match(sizeFilter, greaterSmallerReg1);

    std::regex greaterSmallerReg2("^[><gs]\\d+[kmgtKMGT]?\\s*([><gs]\\d+[kmgtKMGT]?)");
    string greaterSmaller2 = get_match(sizeFilter, greaterSmallerReg2);

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
      sizeOperand.greaterThan = metricMult1 * sizeValue1;
      sizeOperand.greaterThanActive = true;
      if (greaterSmaller2 != "" && (greaterSmaller2[0] == '<' && greaterSmaller2[0] == 's')){
        sizeOperand.smallerThan = metricMult2 * sizeValue2;
        sizeOperand.smallerThanActive = true;
      }
    }
    else {
      sizeOperand.smallerThan = metricMult1 * sizeValue1;
      sizeOperand.smallerThanActive = true;
      bool greater = greaterSmaller2[0] == 'g';
      bool greater1 = greaterSmaller2[0] == '>';
      if (greaterSmaller2 != "" && (greater1 || greater))
        sizeOperand.greaterThan = metricMult2 * sizeValue2;
      sizeOperand.greaterThanActive = true;
    }
  }

  bool SearchOptions::getParameters(int argc, char *argv[]){

    //opt::options_description desc("All options: (search and listingfiles required)");
    //desc = desc.c;
    //_crtBreakAlloc = 894;
    desc.add_options()
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
      opt::store(opt::parse_command_line(argc, argv, desc), vm);
    }
    catch (std::exception& e)
    {
      //  
      std::cerr << "\n\nERROR in options!!!  check your options: " << e.what() << "\n\n";
      std::cout << desc << "\n";
      return false;
    }
    opt::notify(vm);

    if (vm.count("help")) {
      std::cout << "Usage: FileListSearch options\n";
      std::cout << "--search and --listingfiles options required\n";
      std::cout << desc << "\n";
      success = false;
      return false;
    }

    opt::notify(vm);

    // extracting search word from command line options
    if (!vm["search"].empty()) {
      searchString = vm["search"].as<std::string>();
    }
    else {
      std::cout << "Search option required:" << "\n";
      std::cout << desc << "\n";
      success = false;
      return false;
    }

    casesensitive = vm["casesensitive"].as<bool>();
    overwrite = vm["overwrite"].as<bool>();
    fullpath = vm["fullpath"].as<bool>();
    filetype = vm["filetype"].as<std::string>();
    cdtreefilenameflag = vm["cdtreefilenameflag"].as<std::string>();
    fileExtension = vm["fileextension"].as<std::string>();
    fileExtensionCheckCaseSensitive = vm["fileextensioncase"].as<bool>();
    exposeOptions = vm["exposeoptions"].as<bool>();

    year = vm["year"].as<std::string>();
    date = vm["date"].as<std::string>();
    monthYear = vm["monthyear"].as<std::string>();
    sizeFilter = vm["size"].as<std::string>();

    std::regex  sizereg1("^[<>gs]\\d+[MmKkGgTt]?(\\s*[<>sg]\\d+[MmKkGgTt]?)?\\s*$");

    if (sizeFilter != "" && !std::regex_match(sizeFilter, sizereg1)) {
      std::cout << "\n\nAttention!!!\n\nSizeFilter option needs to be in this format: < or > number and optional metric prefix (k,M,G or T)" << std::endl;
      std::cout << "Please try again like this example: -z\">100\" or -z\"<20M\" " << std::endl;
      std::cout << "Or for a range like this example: -z\">100k <20M\"" << std::endl;
      success = false;
      return false;
    }

    if (sizeFilter != "") {
      getSizeOperands();
    }

    std::regex  datereg1("^\\d\\d\\.\\d\\d\\.\\d\\d\\d\\d$");

    if (date != "" && !std::regex_match(date, datereg1)) {
      std::cout << "\n\nAttention!!!\n\nDate option needs to be in this format: dd.mm.yyyy" << std::endl;
      std::cout << "Please try again like this example: -d17.08.2014" << std::endl;
      success = false;
      return false;
    }

    std::regex  monthYearreg1("^\\d\\d\\.\\d\\d\\d\\d$");

    if (monthYear != "" && !std::regex_match(monthYear, monthYearreg1)) {
      std::cout << "\n\nAttention!!!\n\nMonthYear filter option needs to be in this format: mm.yyyy" << std::endl;
      std::cout << "Please try again like this example: -m08.2014" << std::endl;
      success = false;
      return false;
    }

    std::regex  yearreg1("^\\d\\d\\d\\d$");

    if (year != "" && !std::regex_match(year, yearreg1)) {
      std::cout << "\n\nAttention!!!\n\nYear filter option needs to be in this format: yyyy" << std::endl;
      std::cout << "Please try again like this example: -y2014" << std::endl;
      success = false;
      return false;
    }

    searchby = vm["searchby"].as<std::string>();

    if (searchby == "dup")
      searchby = "duplicate";

    timestampInAutoName = vm["timestamp"].as<bool>();

    // extracting search results file file name from command line options
    resultsFilename = vm["resultfile"].as<std::string>();
    //timestampInAutoName = true;

    if (resultsFilename == "auto")
    {
      string resultfileTermString = searchString;
      if (resultfileTermString == "*")
        resultfileTermString = "any";
      if (fileExtension != "*")
        resultfileTermString += "_with_fileExt_" + fileExtension;
      string timeString = "";
      if (timestampInAutoName) {
        //boost::posix_time::ptime nowTime = boost::posix_time::second_clock::local_time();

        boost::posix_time::ptime my_ptime = boost::posix_time::second_clock::universal_time();
        boost::local_time::local_date_time ldt(my_ptime, s_timezone);

        timeString = mydateformat(ldt);
        //to_simple_string(nowTime);
        resultsFilename = "results_for_searchTerm_" + resultfileTermString + "_" + timeString + ".txt";
      }
      else
      {
        resultsFilename = "results_for_searchTerm_" + resultfileTermString + ".txt";
      }

    }
    // extracting file listing path names from command line options

    if (!vm["listingfiles"].empty() &&
      (listFiles = vm["listingfiles"].as<std::vector<string> >()).size() > 0) {
      // good to go
    }
    else {
      std::cout << desc << "\n";
      success = false;
      return false;
    }
    success = true;
    //bool fullexposureOfSearchOptions = true;
    if (exposeOptions)
    {


      cout << "searchString: " << searchString << endl;
      cout << "casesensitive: " << casesensitive << endl;
      cout << "overwrite: " << overwrite << endl;
      cout << "fullpath: " << fullpath << endl;
      cout << "filetype: " << filetype << endl;
      cout << "cdtreefilenameflag: " << cdtreefilenameflag << endl;
      cout << "fileExtension : " << fileExtension << endl;
      cout << "fileExtensionCheckCaseSensitive : " << fileExtensionCheckCaseSensitive << endl;
      cout << "year : " << year << endl;
      cout << "date : " << date << endl;
      cout << "monthYear : " << monthYear << endl;
      cout << "sizeFilter: " << sizeFilter << endl;
      cout << "searchby: " << searchby << endl;
      cout << "timestampInAutoName: " << timestampInAutoName << endl;
      cout << "resultsFilename: " << resultsFilename << endl;
      //cout << "listFiles: " <<  listFiles << endl;
      //cout << "sizeOperand.greaterThanActive: " << sizeOperand.greaterThanActive << endl;
      //cout << "sizeOperand.smallerThanActive: " << sizeOperand.smallerThanActive << endl;
      if (sizeOperand.greaterThanActive)
        cout << "size filter greaterThan: " << sizeOperand.greaterThan << endl;
      if (sizeOperand.smallerThanActive)
        cout << "size filter smallerThan: " << sizeOperand.smallerThan << endl;

    }

    checkWildCardInFileListings();
    return true;
  }


  void SearchOptions::checkWildCardInFileListings() {
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

  void SearchOptions::initializeVariables(){

    searchChar1 = searchString[0];
    searchStringLen = searchString.size();
    searchCharArray = reinterpret_cast<char *>(malloc(searchString.size() + 1));
    memcpy(searchCharArray, searchString.c_str(), searchStringLen + 1);

    filterFileExt = false;
    if (fileExtension.size() > 0 && fileExtension != "*") {

      filterFileExt = true;
      if (fileExtension.at(0) != '.')
        fileExtension = "." + fileExtension;

      // hard coded new line appended: file extension should always be at the end of file list line
      fileExtension += "\r\n";
    }

    fileExtLen = fileExtension.size();
    fileExt = reinterpret_cast<char *>(malloc(fileExtension.size() + 1));
    memcpy(fileExt, fileExtension.c_str(), fileExtLen + 1);
    // do the file extension filtering
    fileExtensionCheck = false; // actual test variable initial value

    

    sizeFilterActive = sizeOperand.greaterThanActive || sizeOperand.smallerThanActive;

    dateFilter = reinterpret_cast<char *>(malloc(date.size() + 1));
    memcpy(dateFilter, date.c_str(), date.size() + 1);

    yearFilter = reinterpret_cast<char *>(malloc(year.size() + 1));
    memcpy(yearFilter, year.c_str(), year.size() + 1);

    monthYearFilter = reinterpret_cast<char *>(malloc(monthYear.size() + 1));
    memcpy(monthYearFilter, monthYear.c_str(), monthYear.size() + 1);

    // if empty no filtering
    dateFilterActive = date.size() > 0;
    // if empty no filtering and if dateFilterActive allready true override monthYearFilterActive
    monthYearFilterActive = monthYear.size() > 0 && !dateFilterActive;
    yearFilterActive = year.size() > 0 && (!monthYearFilterActive && !dateFilterActive);

  }

