#ifndef SEARCHBYNAME_H_
#define SEARCHBYNAME_H_

#include "searchoptions.h"
#include <string>
 
#include <iostream>

using std::string;
class SearchOptions;

bool searchByName(string fileListFilename, SearchOptions searchOptions, std::ofstream &resuts_file);


#endif SEARCHBYNAME_H_