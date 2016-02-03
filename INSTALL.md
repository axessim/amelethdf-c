Compilation and installation
============================

This file provides instructions for installing the Amelet-HDF library. If you
have any problems with the installation, please see the bug tracker at the
following location: https://git.framasoft.org/axessim/amelethdf-c/issues


Obtaining Amelet-HDF
--------------------

The last release of Amelet-HDF is avaiable from
https://code.google.com/p/amelet-hdf/. 

For get the source code checkout the code from the subversion repository.

 $ git clone git@git.framasoft.org:axessim/amelethdf-c.git

Prerequisites
-------------

This project requires:
  * cmake cross-platform builder

  * Reasonably conformant C compiler:
    - GCC 4.6 recomanded 4.8
    - Microsoft Visual C++ 2010 [Express] 32 or 64 
    - Clang 3.4

  * HDF5 Libraries [HEADERS and LIBRARIES] in version 1.8.12.
    (see http://www.hdfgroup.org/HDF5/release/obtain5.html)
    The main HDF5 dependance are Zlib, SZip and MPI. 

Basic installation
------------------

For those who don't like to read ;-) the following steps (after get source) can
be used to configure, build, test, and install the Amelet-HDF Library, header
files, and support programs.

    cd amelet-hdf
    cmake . -DCMAKE_INSTALL_PREFIX:PATH=/usr/local/amelet-hdf <more configure_flags>
    make
    make docs
    make test      # or "ctest ." for more stuff
    make install

<configure_flags> above refers to the configure flags appropriate to your
installation. For example, to install Amlet-HDF release with a specifique HDF5
library, the configure line might read as follows:

    cmake . -DCMAKE_INSTALL_PREFIX:PATH=/usr/local/amelet-hdf \
            -DCMAKE_BUILD_TYPE=RELEASE \
            -DHDF5_DIR:PATH=PATH_TO_HDF5

In this case, PATH_TO_HDF5 would be replaced with the path to the installed
location of the HDF5 library.

Full installation
-----------------

The 'cmake' script attempts to guess correct values for various
system-dependent variables used during compilation. It may also create one or
more '.h' files containing system-dependent definitions.

### Installation names

By default, 'cmake' will install the package's files in current directory
/usr/local/lib, /usr/local/bin etc. You can specify an installation prefix
other than '/usr/local' by giving 'cmake' the option
'-DCMAKE_INSTALL_PREFIX:PATH=$PATH'.

    cmake . -DCMAKE_INSTALL_PREFIX:PATH=$PATH 

### Build flavor

The build flavor is used to define the kind of compilation 'RELEASE', 'DEBUG'
or 'RELWITHDEBUG' corresponding to different compiler options. You can specify
the flavor build by giving 'cmake' the options '-DCMAKE_BUILD_TYPE=$FLAVOR'.

    cmake . -DCMAKE_INSTALL_PREFIX:PATH=$PATH -DCMAKE_BUILD_TYPE=$FLAVOR <more configure_flags>

### Build directory and temporal data

The build and source directory can be differente. It useful way for build
different flavor.

    mkdir $BUILD_PATH
    cd $BUILD_PATH
    cmake $AMELETHDF_SOURCE_PATH <more configure_flags>

### Optional Features

Some packages pay attention to '-DAMELETHDF_ENABLE_FEATURE' options to 'cmake',
where FEATURE indicates an optional part of the package. They may also pay
attention to '-DAMELETHDF_WITH_PACKAGE' options, where PACKAGE is something
like 'libz' or 'mpich' (for the paralle compilation).  The folowing section
should mention any '-DAMELETHDF_ENABLE_' and '-DAMELETHDF_WITH_' options that
the package recognizes.

For packages that use some PACKAGE, 'cmake' can usually find the PACKAGE
include and library files automatically, but if it doesn't, you can use the
'cmake' options '-DPACKAGE_INCLUDE_DIR=DIR' and '-DPACKAGE_LIBRARY_DIR=LIB' or
'-DPACKAGE_LIBRARY=LIB' to specify their locations.

 - Paralle compilation: -DAMELETHDF_ENABLE_MPI=OFF|ON
 - HDF5: -DHDF5_DIR:PATH=
 - Zlib: -DZLIB_INCLUDE_DIR:PATH= and -DZLIB_LIBRARY:PATH=