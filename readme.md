FileListingSearch
======

FileListingSearch is application for searching files from multiple hard drives or usb sticks in one application run. It searches file names from text files containing a files and folders listing. More than one listing file can be searched at once. 

FileListingSearch is currently a windows command line application.
It searches files from file listing text files and produces aggregated csv/txt file for search results.

This application arose from a need to search simultaneously from multiple hard drives (and/or usb sticks)  where several drives may contain millions of files thus making search a performance critical task. (Some optimization for performance has been attempted.. possibly violating the phrase "premature optimization .." )


Blog post / home page:
--------
[http://juy.fi/blogi/searching-for-lost-files-with-filelistsearch/] ( http://juy.fi/blogi/searching-for-lost-files-with-filelistsearch/)

Usage:
--------
1. you need to create text files containing list of files and folders for all hard drives you wish to search files from.
  * this is how it is done: 
     navigate to the drive in command prompt and issue this command: (you may modify the file name to your preference)
    * dir /S /-C <DRIVE LETTER>: > drive_filelisting.txt
  * for example if you want to make a listing from usb stick found in drive G: you could make following command:
    * dir /S /-C G: > g_usb_drive_listing.txt
  * Note that this application currently works ONLY on English windows os with this specific dir command  options:  /S /-C

2. then issue command for searching the file listings:
FileListingSearch.exe --search=searchterm --resultfile=searchresults_for_searchterm.csv --listingfiles=g_usb_drive_listing.txt

replace the option values: searchterm, searchresults_for_searchterm.csv, g_usb_drive_listing.txt

Options
--------

FileListSearch.exe --help:

Usage: FileListSearch options

--search and --listingfiles options required

*  -s [ --search ] arg                   search string
*  -l [ --listingfiles ] arg             file listings
*  -c [ --casesensitive ] arg (=0)       casesensitive search true/false
*  -f [ --filetype ] arg (=file)         file type to search (file, directory or
                                        both)
* -r [ --resultfile ] arg (=auto)       results output file name (auto means
                                        automatically generated file name with
                                        format: results_for_searchTerm_searchte
                                        rm.txt )
* -u [ --fullpath ] arg (=0)            fullpath included in results
* -b [ --searchby ] arg (=filename)     searchtype (filename, by_directory_name
                                        or cdtree)
                                             filename = regular file name
                                        search,
                                             by_directory_name = list all files
                                        in directories that match search term,
                                             cdtree = search cdtree format csv
                                        file
* -x [ --fileextension ] arg (=*)       file extension filter for search
                                        default to any
* -n [ --fileextensioncase ] arg (=0)   file extension filter casesensitive
                                        defaults to false (= case insensitive)
* -e [ --timestamp ] arg (=0)           include timestamp in auto generated
                                        result file name
* -o [ --overwrite ] arg (=0)           overwrite results file by default in
                                        case it exists
* -d [ --cdtreefilenameflag ] arg (=cdtree)
                                        if this string found in the file name
                                        switch to cdtree search function
*  --help                                produce help message

Compilation:
--------
You need [http://www.boost.org/] ( http://www.boost.org/ ) library to compile this.
Tested with Visual Studio 2013, windows 8.1

replace &lt;AdditionalIncludeDirectories&gt;
and  &lt;AdditionalLibraryDirectories&gt; values in  FileListSearch.vcxproj project file for your boost paths

Binary:
--------
Download link for compiled exe can be found from blog post:
[http://juy.fi/blogi/searching-for-lost-files-with-filelistsearch/] ( http://juy.fi/blogi/searching-for-lost-files-with-filelistsearch/)


Todo:
--------
* internationalization: time zones, operating system languages
* further performance optimization 
* parallel code
* memory leak checks
* more search options: date, year, file size, regex
* splitting date, size into csv
* GUI
* post processing option: duplicate removal

