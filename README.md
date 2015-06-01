# Tracking faces with OpenTLD

Initialized with faces using OpenCV's Haar classifier. TODO: Extension to multiple faces in the video.

# Introduction

This is a C++ implementation of OpenTLD that was originally published in MATLAB by Zdenek Kalal. OpenTLD is used for tracking objects in video streams. What makes this algorithm outstanding is that it does not make use of any training data. This implementation is based solely on open source libraries, meaning that you do not need any commercial products to compile or run it.

A documentation of the internals as well as other possibly helpful information is contained in this [master thesis](https://github.com/downloads/gnebehay/OpenTLD/gnebehay_thesis_msc.pdf).

# Usage

```
cd build/bin
./opentld [video-file]
```

# Building
## Dependencies
* OpenCV
* CMake
* libconfig++ (optional)
* Qt4 (optional)

## Compiling
### CMake
OpenTLD uses CMake to create native build environments such as make, Eclipse, Microsoft Visual Studio.
If you need help look at [Running CMake](http://www.cmake.org/cmake/help/runningcmake.html).

You can use `cmake-gui`, if you need a graphical user interface.

Use CMake to build the project. You can use "cmake-gui", if you need a graphical user interface.

__Gui__  
* Specify the source path (root path of the dictionary) and the binary path (where to build the program, out
	  of source build recommended)
* Configure
* Select compiler
* Adjust the options (if needed)
* Configure
* Generate

__Command line__  
If you have uncompressed the source in $OPENTLD, type in a console:
```bash
cd $OPENTLD
mkdir ../build
cd ../build
cmake ../$OPENTLD -DBUILD_QOPENTLD=ON -DUSE_SYSTEM_LIBS=OFF
```

__CMake options__  
* `BUILD_QOPENTLD` build the graphical configuration dialog (requieres Qt)
* `USE_SYSTEM_LIBS` don't use the included cvblob version but the installed version (requieres cvblob)

### Windows (Microsoft Visual Studio)
Navigate to the binary directory and build the solutions you want (You have to compile in RELEASE mode):
* `opentld` build the project
* `INSTALL` install the project

_Note_: `vcomp.lib` is necessary when you try to compile it with OpenMP support and the
Express versions of MSVC. This file is included in the Windows Server SDK.

### Linux (make)
Navigate with the terminal to the build directory
* `make` build the project
* `make install` build and install the project

### Mac
* `brew install python`
* `brew install gfortran`
* `easy_install numpy`
* `brew install cmake`
* `brew install opencv`
* `cmake` build the project

# Debian package
* Navigate with the terminal into the root dictionary of OpenTLD (OpenTLD/)
* Type `debuild -us -uc`
* Delete the temporary files in the source tree with `debuild clean`

