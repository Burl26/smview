# smview
Sheet Music PDF Organizer and Viewer

Organize your sheet music PDFs by artist or title.  It also keeps track of your recently played songs.
The underlying database is a simple csv file that is read and written by the application.

The integrated PDF viewer uses the mupdf engine.  The front-end has been modified to create a continuous vertical scroll over
multiple pages.  I also stripped out 


Building the code
step 1. You will need to download mupdf and build the library - see steps 2 and 3.
https://mupdf.com/downloads/

step 2. Copy the files from this ./mupdf directory into your build.  This strips out alot of the great features of
mupdf that you don't need if you are just reading PDF files.  

step 3. build mupdf using the makefiles listed below.  You will dependancy packages like freetype.
Makefile
Makethird
Makerules

step 4. Download FLTK
https://www.fltk.org/software.php

step 5. copy the config.h file from this ./FLTK directory into your FLTK/FL source (overwrite old one).

step 6. build FLTK

step 7. Compile the smview source code.  I used eclipse, but here are the compiler and linker settings so you can create
your own makefile.

Compiler Includes
-I/yourmudfdirectory/include
-I/yourFLTKdirectory

Linker
-L/yourmupdfdirectory/build/release
-L/yourFLTKdirectory/lib

Libraries
fltk
Xrender
Xcursor
Xfixes
Xext
Xft
fontconfig
Xinerama
dl
X11
mupdf
mupdf-third
freetype

