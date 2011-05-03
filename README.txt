This directory contains skeleton code as a starting point for assignment2 of COS 426. 


FILE STRUCTURE
==============

There are several files, but you should mainly change src/raytrace.cpp.

  src/ - Directory with source code
    Makefile - Same as Makefile.mac.
    Makefile.mac - Mac makefile for building the project with "make". 
    Makefile.linux - Linux makefile for building the project with "make". 
    Makefile.cygwin - Cygwin makefile for building the project with "make". 
    raypro.[vcproj/sln] - Project file for compiling raypro in Visual Studio on Windows
    rayview.[vcproj/sln] - Project file for compiling rayview in Visual Studio on Windows
    raypro.cpp - Batch program for ray tracing, reads a scene and generates an image
    rayview.cpp - Interactive program for viewing scenes
    raytrace.cpp - Main ray tracing functions (this is the file you will edit)
    R3Scene.[cpp/h] - Class used for reading and storing scenes to be ray traced.
    R2/ - A library of useful 2D geometric primitives (includes R2Image and R2Pixel)
    R3/ - A library of useful 3D geometric primitives (includes R3Mesh)
    jpeg/ - A library for reading/writing JPEG files
  input/ - Contains example input scenes. 
  output/ - Is empty to start -- it will contain the images produced by your program
  art/ - Empty to start -- it will (optionally) contain movies and art contest submissions produced by your program
  runme.bat - a script (for Windows) that you will edit with commands to demonstrate execution of your program
  runme.sh - same as <code>runme.bat</code>, but for Mac OS X or linux
  Makefile - similar to <code>runme.bat</code>, but only makes output if missing or out-of-date
  writeup.html - a skeleton HTML file that you can use as a basis for your writeup 


COMPILATION
===========

If you are developing on a Windows machine and have Visual Studio
installed, use the provided project solution files (src/raypro.sln and
src/rayview.sln) to build the program. If you are developing on a Mac, 
cd into the src/ directory and type "make".  If you are developing on 
a Linux machine, cd into the src/ directory, then type "cp Makefile.linux 
Makefile" and then you can type "make" to compile the programs.  In any
case, executables called raypro and rayview (or raypro.exe and rayview.exe) 
will be created in the src/ directory.
