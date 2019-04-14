# smview
Sheet Music PDF Organizer and Viewer

Organize your sheet music PDFs by artist or title.  It also keeps track of your recently played songs.
The underlying database is a simple csv file that is read and written by the application.

The integrated PDF viewer uses the mupdf engine.  The front-end has been modified to create a continuous vertical scroll over
multiple pages.  I also stripped out support for linking and other file formats. 

Building the code for Linux.
step 1. You will need to download mupdf and build the library - see steps 2 and 3.
https://mupdf.com/downloads/

step 2. Overwrite the mupdf files with the files from the ./mupdf_build directory of this repository.  This strips out alot of the great features of mupdf that you don't need if you are just reading PDF files.  The config.h file in the ./mupdf_build directory must overwrite the file in the $MUPDF/include/mupdf/fitz directory.

step 3. build mupdf using the makefile listed above.  You will need some dependancy packages like freetype.

step 4. Download FLTK
https://www.fltk.org/software.php

step 5. copy the config.h file from this ./FLTK_build directory into your FLTK/FL source (overwrite old one).

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

