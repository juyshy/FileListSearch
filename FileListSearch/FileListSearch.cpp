// FileListSearch.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <boost/iostreams/device/mapped_file.hpp> // for mmap
#include <algorithm>  // for std::find
#include <iostream>   // for std::cout
#include <cstring>
#include <boost/timer/timer.hpp>
#include <string>
#include <sstream> 
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <regex>
using std::string;
using std::cout;
using std::ifstream;
using namespace boost::filesystem;


 
using std::endl;

std::ofstream resuts_file("hakutulokset2.txt");

bool search(string filenimi, string searchString) {
  //boost::iostreams::mapped_file mmap;
  boost::iostreams::mapped_file mmap(filenimi, boost::iostreams::mapped_file::readonly);
  const char * f = mmap.const_data();
  //const char * alku = f;
  //char eka = searchString.at(0);
  //boost::timer::auto_cpu_timer t;
  //auto end = f + mmap.size();
  //auto size2 = end - f;
 
  //string filecontents(alku, size2);
  //std::size_t found = filecontents.find(searchString);
  //std::vector<string> hakutulokset;
  //if (found == std::string::npos)
  //  return false;


  //string currentDir;
  //resuts_file << "listaus: *******************************************************\n";
  //resuts_file << "listaus 2 : ***************************\n";
  //while (found != std::string::npos) {
  //  //std::cout << "  'needle' found at: " << found << '\n';

  //  // if (found != std::string::npos){
  //  std::size_t linendindx = filecontents.find("\r", found + 1);
  //  std::size_t rivinalku = filecontents.rfind('\n', found) + 1;

  //  if (rivinalku == std::string::npos)
  //    rivinalku = 0;
  //  int pituus = linendindx - rivinalku;
  //  string tulosrivi = filecontents.substr(rivinalku, pituus);
  //  std::size_t dirfound = tulosrivi.find("Directory of");
  //  std::size_t dirfound2 = tulosrivi.find("<DIR>");

  //  if (dirfound == std::string::npos && dirfound2 == std::string::npos)
  //  {
  //    std::size_t previousDirectory = filecontents.rfind("Directory of", rivinalku) + 13;
  //    std::size_t dirlinEndindx = filecontents.find("\r", previousDirectory + 1);
  //    currentDir = filecontents.substr(previousDirectory, dirlinEndindx - previousDirectory);
  //    hakutulokset.push_back(tulosrivi);
  //    resuts_file << currentDir << "; " << tulosrivi << "\n";
  //  }
 
  //  found = filecontents.find(searchString, rivinalku + pituus);

  //}
 
  //cout << "hakutuloksia: " << hakutulokset.size() << endl;

 
  return true;
}


int main()
{
   boost::timer::auto_cpu_timer t;
   //string filenimi = "E:/adm/hdlist/LACIESHARE_12012015 - 113107_30K_EKAARIVIA.txt"; 
   std::string filenimi = "E:/adm/hdlist/stuff/LACIESHARE_12012015-113107_30K_EKAARIVIA.txt";// listings[2];

   string searchString = "opengl";
   search(filenimi, searchString);
 
}

