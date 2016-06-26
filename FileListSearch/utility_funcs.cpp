#include "stdafx.h"
#include "utility_funcs.h"
 

std::string mydateformat(boost::local_time::local_date_time const& ldt)
{
  using namespace boost;
  std::ostringstream ss;

  boost::local_time::local_time_facet* output_facet = new boost::local_time::local_time_facet();
  ss.imbue(std::locale(std::locale::classic(), output_facet));
  output_facet->format("%Y-%m-%d_%H-%M-%S");

  ss.str("");
  ss << ldt;
  return ss.str();
}


string  get_match(std::string const &s, std::regex const &r) {
  std::smatch match;
  string matchStr;
  if (std::regex_search(s, match, r)) {
    /*std::cout << "Sernum: " << */
    matchStr = match[1].str();
    return matchStr;
  }
  else {
    std::cerr << s << "did not match\n";
    return "";
  }
}

char * stringToCharPtr(string str1) {
  char * searchCharPtr = reinterpret_cast<char *>(alloca(str1.size() + 1));
  memcpy(searchCharPtr, str1.c_str(), str1.size() + 1);
  return searchCharPtr;
}


int replace(std::string& str, const std::string& from, const std::string& to) {
  size_t start_pos = str.find(from);
  int count = 0;
  if (start_pos == std::string::npos)
    return 0;
  while (start_pos != std::string::npos) {

    count++;

    str.replace(start_pos, from.length(), to);
    start_pos = str.find(from, start_pos + 1);
  }
  return count;
}


bool fexists(string filenameToCheck)
{
  auto filenameToCheckChar = reinterpret_cast<char *>(alloca(filenameToCheck.size() + 1));
  memcpy(filenameToCheckChar, filenameToCheck.c_str(), filenameToCheck.size() + 1);

  ifstream ifile(filenameToCheckChar);
  return static_cast<bool>(ifile);
}


