# Whitebox Media Library

Whitebox is an open source library for processing, storing and playing of downloaded TV shows.

It relies on the standardised conventions used to name such files from most TOR sites in order to extract the show name, season, and episode from a file.

Eg:

```Archer.S10E03.480p.x264-mSD[eztv].mkv```

is interpreted as:

```Archer season 10, episode 3```

existing files are stored in a database and can be played back through the app, where they are organised in an intuitive interface.

watch progress is internally tracked, meaning it is easy to seamlessly resume playback of a series with a single click, even if you stopped mid-episode

  ------
  

## Features

Whitebox's feature list is always growing. currently, it supports the following:

* scan all subfolders and files in a given directory to find valid video files that match a known naming convention, and index these files

* seamlessly play shows through its interface, in correct season/episode order even if the original files were never organised

* organise all indexed files into a root 'library' where files are placed into their relevant show and season folders, and renamed to convention (showname_s01_e01.xyz)

* easily replace a file with higher quality/different version

* open files directly into the program from your file browser (automatically indexes the file if it is a valid shows, records its information and tracks the watch progress)

  ------
  
## Build from source and run:

 * Download or clone the repository
  
 * open the .pro file in the /src folder in QT creator
  
 * set the kit to either MSVC or MINGW  QT 5.13
  
*  Build.
  
 * move Whitebox.exe from release folder to its own folder
  
 * use the windeployqt.exe for your build kit on the exe
  
 * in the mediaservice folder created, *delete* the dsengine.dll file, as this file takes (direct show media plugin) takes precedence over WMF (windows media foundation), and DS does not play all file types.

  You are free to build this on any OS, but it has only been tested sufficiently on windows with these kits/versions
  
  Issues exist with QT 5.12 media playback on windows, where many common torrented show file types cannot be played. 
  
  a MSVC 5.13.0 built and deployed as above is included in the repo, in the /windeployqt folder
  
  ------
  
  
  ### In development:
  
  * Windows installer file for the program, to properly register it & streamline its use
  
  * automatically find new and/or missing torrents for shows
  
  * display images for the shows in the UI rather than their names, where images are available
  
  * pin favorite shows
