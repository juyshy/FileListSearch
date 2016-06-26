FileListingSearch
======

FileListingSearch is application for searching files from multiple hard drives or usb sticks in one application run. It searches file names from text files containing a files and folders listing. More than one listing file can be searched at once. 

FileListingSearch is currently a windows command line application.
It searches files from file listing text files and produces aggregated csv file for search results.

This application arose from a need to search simultaneusly from multiple hard drives (and/or usb sticks)  where several drives may contain millions of files thus making search a performance critical task.



Usage:
--------
1. you need to create text files containing list of files and folders for all hard drives you wish to search files from.

this is how it is done: 
navigate to the drive in command prompt and issue this command: (you may modify the file name to your preference)
 
dir /S /-C <DRIVE LETTER>: > drive_filelisting.txt

for example if you want to make a listing from usb stick found in drive G: you could make following command:

dir /S /-C G: > g_usb_drive_listing.txt

Note that this application currently works only on english windows os with this specific dir command  options:  /S /-C

2. then issue command for searhing the file listings:
FileListingSearch.exe --search=searchterm --resultfile=searchresults_for_searchterm.csv --listingfiles=g_usb_drive_listing.txt

replace the option values: searchterm, searchresults_for_searchterm.csv, g_usb_drive_listing.txt




Todo:
--------
internationalization: timezones, operating system languages
further performance optimations
parallel code
memory leak checks
more search options
splitting date, size in csv
GUI

