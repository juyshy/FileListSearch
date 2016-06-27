#ifndef UTILITY_FUNCS_H_
#define UTILITY_FUNCS_H_

#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp> 
#include <boost/date_time/local_time/local_time.hpp>
#include <sstream> 
#include <string>
#include <regex>
#include <iostream>

using std::string;
using std::ifstream;

namespace greg = boost::gregorian;


static boost::local_time::time_zone_ptr const s_timezone(new boost::local_time::posix_time_zone("+03:00"));


std::string mydateformat(boost::local_time::local_date_time const& ldt);

 

//http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
// trim from start (in place)
static inline void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
  ltrim(s);
  rtrim(s);
}

string  get_match(std::string const &s, std::regex const &r);

//void stringToCharPtr(const string str1, char * searchCharPtr);
int replace(std::string& str, const std::string& from, const std::string& to);
bool fexists(string filenameToCheck);

int metricPrefix2Integer(char prefix);

#endif UTILITY_FUNCS_H_