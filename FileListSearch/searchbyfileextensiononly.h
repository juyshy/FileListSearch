#ifndef SEARCHBYFILEEXTENSIONONLY_H_
#define SEARCHBYFILEEXTENSIONONLY_H_

#include "searchoptions.h"
#include <string>
#include <iostream>
using std::string;


unsigned long long getSize(const char *   linestartPoint, const char * lineEndPoint);

void reportDriveMetadata(const char * f, std::ofstream & resuts_file);
bool findDups(string fileListFilename, SearchOptions searchOptions, std::ofstream &resuts_file);
bool searchBySizeOnly(string fileListFilename, SearchOptions searchOptions, std::ofstream &resuts_file);


bool searchByFileExtensionOnly(string fileListFilename, SearchOptions searchOptions, std::ofstream &resuts_file);


#endif SEARCHBYFILEEXTENSIONONLY_H_