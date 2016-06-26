#ifndef SEARCHFILESBYFOLDERNAME_H_
#define SEARCHFILESBYFOLDERNAME_H_

#include "searchoptions.h"
#include <string>
#include <iostream>
using std::string;

bool searchFilesByFolderName(string fileListFilename, SearchOptions searchOptions, std::ofstream &resuts_file);

#endif SEARCHFILESBYFOLDERNAME_H_