#ifndef SEARCHFROMCDTREE_H_
#define SEARCHFROMCDTREE_H_

#include "searchoptions.h"
#include <string>
 
#include <iostream>

using std::string;
class SearchOptions;

bool searchFromCdTree(string fileListFilename, SearchOptions searchOptions, std::ofstream &resuts_file);


#endif SEARCHFROMCDTREE_H_