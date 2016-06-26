#ifndef SEARCHBYFILEEXTENSIONONLY_H_
#define SEARCHBYFILEEXTENSIONONLY_H_

#include "searchoptions.h"
#include <string>
#include <iostream>
using std::string;

bool searchByFileExtensionOnly(string fileListFilename, SearchOptions searchOptions, std::ofstream &resuts_file);


#endif SEARCHBYFILEEXTENSIONONLY_H_