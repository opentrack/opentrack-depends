Sixense SDK Package
12/15/2011
======================


Table of Contents
=================

1. Installing the SDK
   1.1 Requirements
   1.2 Install SDK using install.py

2. How to Run Sample Apps

3. Build From Source
   3.1 Build for Windows
      3.1.1 Requirements
      3.1.2 sixense_simple3d and freeglut
   3.2 Build for Linux
      3.2.1 Requirements
      3.2.2. sixense_simple3d and freeglut
         3.2.2.1 Using Eclipse
         3.2.2.2 Using Makefile
   3.3 Build for OSX
      3.3.1 Requirements
      3.3.2 Building sixense_simple3d and freeglut
         3.3.2.1 OSX 10.6 Snow Leopard
         3.3.2.1 OSX 10.7 Lion
     
4. Packaging Scripts
   4.1 Requirements
   4.2 install.py
      4.2.1 Information
      4.2.2 Install SixenseSDK using package_sdk.py

5. Developer Notes
   5.1 Library Paths
      5.1.1 Linux
      5.1.2 OSX
   5.2 Tested Operating Systems
      5.2.1 Linux

1. Installing the SDK
=====================

1.1 Requirements
----------------
    Python version 2.5 or later is required to install the SDK using install scripts.

1.2 Install SDK using install.py
--------------------------------
    1.  Navigate to the "install" directory in the top level of the sdk.
    2.  Run install.py as administrator.
        on Fedora Linux, to beome the root (administrator) user, type:
         su - <enter>
        On Ubuntu Linux, for each command you wish to execute as administrator, 
        type :
         sudo <command> <enter>
        [TODO: Windows, OSX...]
    3.  Select the operating system of the target computer when prompted.
    4.  Optionally, see section 5 below for further instructions on installing shared libraries.

2. How to run sample apps
=============================
    1.  Use a file browser to navigate to "sixense_simple3d" directory.
        There, folders for various OSes are found:
         OSX 10.6 32 bit : SixenseSDK/samples/osx10.6/
         OSX 10.6 64 bit : SixenseSDK/samples/osx10.6_x64/
         OSX 10.7 32 bit : SixenseSDK/samples/osx10.7/
         OSX 10.7 64 bit : SixenseSDK/samples/osx10.7_x64/
         Linux 32 bit    : SixenseSDK/samples/linux/
         Linux 64 bit    : SixenseSDK/samples/linux_x64/
         Windows 32 bit  : SixenseSDK/samples/win32/
         Windows 64 bit  : SixenseSDK/samples/x64/
    2.  Use a file browser to navigate into the proper folder and then double-click 
        the "sixense_simple3d" application. Alternatively, in a terminal in the proper dirctory
        type:
        ./sixense_simple3d <enter>
        
3. Build From Source
====================
    All source projects regardless of operating system use relative paths to source and library
    files.

3.1 Build for Windows
---------------------

3.1.1 Requirements
------------------
    To compile any of the libraries in windows, Microsoft Visual C++ must be used.  The Express
    version of Microsoft Visual C++ can not be used to compile the source code because Microsoft
    Visual C++ Express does not contain any MFC header's or libraries.

    Sixense_simple3d and freeglut require Visual Studio 2010 to compile the sample application.

3.1.2 sixense_simple3d and freeglut
---------------------------------
    1.  Navigate to "src/sixense_simple3d/VisualStudio2010" directory in the top level SDK directory.
    2.  Open the Visual Studio Solution for the Sixense driver:
        VC++ 2010   freeglut_vs2010.sln
    3.  Select the target configuration in the "Solution Configuration" dropdown combo box.
         Debug   : Debug
         Release : Release
    4.  Select the target platform in the "Solution Platform" dropdown combo box.
         Windows 32 bit : Win32
         Windows 65 bit : x64
    5.  Build the solution by selecting Build->Bulid Solution in the main menu ( shortcut F7 )
    6.  The libraries will be located at:
         Debug   : src/sixense_simple3d/VisualStudio2010/Debug/
         Release : src/sixense_simple3d/VisualStudio2010/Release/


3.2 Build for Linux
-------------------

3.2.1 Requirements
------------------
    In order to compile for Linux, the following package dependancies must be installed:
     libusb1
     gl-mesa-devel (required only for freeglut)
     libXi-devel
     g++
    See section 5 for instructions on installing packages.

3.2.2 sixense_simple3d and freeglut
-----------------------------------
    Pre-built binaries of sixense_simple3d and freeglut are provided in the SDK. If a build on a new 
    operating system is desired, see instructions which follow.

3.2.2.1 Using Eclipse
--------------------- 
    1.  Open Eclipse and select a workspace.
    2.  Click File->Import->Existing Project Into Workspace
    3.  Choose the SixenseSDK directory as the root directory.  Eclipse will list all projects under
        that directory.
    4.  Choose freeglut and sixense_simple_3d and click Finish.
    5.  Right click on the project and select "Build"

3.2.2.2. Using Makefiles
------------------------
    1.  Open a Terminal.
    2.  Navigate to one of the following directories in the SixenseSDK directory based on the build
        target.
         Release 32 bit : SixenseSDK/src/sixense_simple3d/Eclipse/freeglut/Release_x32
         Release 64 bit : SixenseSDK/src/sixense_simple3d/Eclipse/freeglut/Release_x64
    3.  Type:
         make <Enter>
    4.  Navigate to one of the following directories in the SixenseSDK directory based on the build
        target.
         Release 32 bit : SixenseSDK/src/sixense_simple3d/Eclipse/sixense_simple_3d/Release_x32
         Release 64 bit : SixenseSDK/src/sixense_simple3d/Eclipse/sixense_simple_3d/Release_x64
    5.  Type:
         make <Enter>

    Note that SDK Makefiles originally contain relative paths to sources. Once an Eclipse project is
    built, though, Eclipse overwrites the Makefiles with absolut paths, unless Eclipse options are 
    chosen to avoid generating Makefiles.

3.3 Build for OSX
-----------------

3.3.1 Requirements
------------------
    To keep the build results in the same directory click Xcode->Preferences->Locations, and set
    'Derived Data' to 'Relative', instead of 'Default'.  This is a system setting. By default,
    clicking Product->Build will compile a Debug version only.  To compile a Release version,
    click Product->'Build for Profiling'.  This behavior can be changed in Product->'Edit Schemes'
    Changing the location from 'Derived Data' to 'Relative' allows the files built in OSX to be
    placed in a relative location from the source instead of a system defined location allowing
    the files to be easily located.

3.3.2 Building sixense_simple3d and freeglut
-------------------------------------------------
    Pre-built binaries of sixense_simple3d and freeglut are provided in the SDK. If a build on a new 
    operating system is desired, see instructions which follow.

3.3.2.1 Using Xcode on OSX 10.6 Snow Leopard
-----------------------------
    1.  Using a file browser, navigate to the directory where the xcode project exists:
         SixenseSDK/src/sixense_simple3d/Xcode
    2.  Double click on "freeglut.xcodeproj" to open the XCode IDE.
    3.  Set XCode's active scheme to:
         OSX 32 bit : freeglut_i386, or
         OSX 64 bit - freeglut_x86_64
        by selecting the drop down list in the upper left hand corner of the XCode window,
        to the right of the 'stop' button.
    4.  Click on "freeglut" project in the file view list to open "Build Settings"
    5.  Change "Mac OSX Deployment Target" setting to Mac OSX 10.6
    6.  Click on Product->Build from the main menu ( shortcut: command B ).
    7.  Set XCode's active scheme to:
         OSX 32 bit - sixense_simple3d_i386, or
         OSX 64 bit - sixense_simple3d_x86_64
        by selecting the drop down list in the upper left hand corner of the XCode window,
        to the right of the 'stop' button.
    8.  Click on "freeglut" project in the file view list to open "Build Settings"
    9.  Change "Mac OSX Deployment Target" setting to Mac OSX 10.6
    10. Click on Product->Build from the main menu ( shortcut: command B ).
    11. Using a file browser, navigate to:
         SixenseSDK/src/sixense_simple3d/Xcode/DerrivedData/freeglut/Build/Products/Debug
    12. To run the sixense_simple3d application, double click on sixense_simple3d.

3.3.2.2 Using Xcode on OSX 10.7 Lion
---------------------
    1.  Using a file browser, navigate to the directory where the xcode project exists:
         SixenseSDK/src/sixense_simple3d/Xcode
    2.  Double click on "freeglut.xcodeproj" to open the XCode IDE.
    3.  Set XCode's active scheme to:
         OSX 32 bit - freeglut_i386, or
         OSX 64 bit - freeglut_x86_64
        by selecting the drop down list in the upper left hand corner of the XCode window,
        to the right of the 'stop' button.
    4.  Click on "freeglut" project in the file view list to open "Build Settings"
    5.  Change "Mac OSX Deployment Target" setting to Mac OSX 10.7
    6.  Click on Product->Build from the main menu ( shortcut: command B ).
    7.  Set XCode's active scheme to:
         OSX 32 bit - sixense_simple3d_i386, or
         OSX 64 bit - sixense_simple3d_x86_64
        by selecting the drop down list in the upper left hand corner of the XCode window,
        to the right of the 'stop' button.
    8.  Click on "freeglut" project in the file view list to open "Build Settings"
    9.  Change "Mac OSX Deployment Target" setting to Mac OSX 10.7
    10. Click on Product->Build from the main menu ( shortcut: command B ).
    11. Using a file browser, navigate to:
         SixenseSDK/src/sixense_simple3d/Xcode/DerrivedData/freeglut/Build/Products/Debug
    12. To run the sixense_simple3d application, double click on sixense_simple3d.

4. Packaging Scripts
====================

4.1 Requirements
----------------
    All scripts have been tested with Python 2.5 and Python 2.6.

4.2 install.py
--------------
This file is located in this directory: SixenseSDK/install/install.py
Usage information follows.

4.2.1 Information
-----------------
    Installs the SixenseSDK files onto a designated platform using a config file
    ( default install.cfg ). This script should be run from SixenseSDK/install.  The script
    install.py will install libraries and add new "udev rules" for Linux.  After installing 
    udev rules, the hydra device must be unplugged then plugged back in to allow the new 
    udev rules to take effect.

    The script install.py should only be run from within a SixenseSDK package.

    Usage: install.py [options]

    Options:
      -h, --help            show this help message and exit
      --version             Prints the version and exits
      -c CONFIG, --config=CONFIG
                            Config File to use
      -v, --verbose         Print Extra Information
      -w, --warning         Print Only Warning Information

4.2.1 Information
--------------
    The default configuration script for install.py is called "install.cfg".  This file defines 
    various paths used for the installation process.

5. Developer Notes
==================

5.1 Library Paths
-----------------
    On Linux and OSX, system shared library paths may not be searched automatically for dynamic
    libraries which SixenseSDK installs. SDK installer scripts take action to correct this issue.

5.1.1 Linux
-----------
    All Fedora packages should be updated after an install. This can be done
    with the following command (as root):
    yum update <enter>

    Run install.py script to place all libraries in standard library paths.
    Alternatively, the end user can put SDK shared libraries into /usr/local/lib, add that
    path to /etc/ld.so.conf if needed, then run /sbin/ldconfig

    Most required shared libraries should be pre-installed on your system but some may not be.
    In Fedora, use the system package manager, or, on a command line do the following, substituting
    the desired package name (as root):
    yum install mesa-libGL <enter>
    (here package "mesa-libGL" is installed)

    What packages need to be installed on Fedora15-64 ?
    ( this list was generated using the command: ldd ./sixense_simple3d )

    file: /usr/lib64/libGL.so.1
    package: mesa-libGL-7.11-1.fc15.x86_64

    file: /usr/lib64/libstdc++.so.6
    libstdc++-4.6.1-9.fc15.x86_64

    file: /lib64/libm.so.6
    file: /lib64/libc.so.6
    file: /lib64/libpthread.so.0
    file: /lib64/libdl.so.2
    file: /lib64/librt.so.1
    package: glibc-2.14-5.x86_64

    file: /lib64/libgcc_s.so.1
    package: libgcc-4.6.1-9.fc15.x86_64

    file: /lib64/libusb-1.0.so.0
    package: libusb1-1.0.8-7.fc15.x86_64

    file: /usr/lib64/libX11.so.6
    package: libX11-1.4.3-1.fc15.x86_64

    file: /usr/lib64/libXext.so.6
    package: libXext-1.2.0-2.fc15.x86_64

    file: /usr/lib64/libXdamage.so.1
    package: libXdamage-1.1.3-2.fc15.x86_64

    file: /usr/lib64/libXfixes.so.3
    package: libXfixes-5.0-1.fc15.x86_64

    file: /usr/lib64/libXxf86vm.so.1
    package: libXxf86vm-1.1.1-2.fc15.x86_64

    file: /usr/lib64/libdrm.so.2
    package: libdrm-2.4.26-2.fc15.x86_64

    file: /usr/lib64/libxcb.so.1
    package: libxcb-1.7-2.fc15.x86_64

    file: /usr/lib64/libXau.so.6
    package: libXau-1.0.6-2.fc15.x86_64

    In Ubuntu, or other Debian based Linux distributions, packages can be installed like this:
    sudo apt-get install libusb1.0-0
    (this would install libusb1 on Ubuntu)

5.1.2 OSX
---------
    In OSX, library paths are hardcoded into the binary during linking. Currently, library files
    are hardcoded to these paths:
      libsixense.dylib -> /usr/local/lib
      libsixense_utils.dylib -> /usr/local/lib
      libfreeglut.dylib -> @executable_path(same path as executable)
    To see what library paths are hardcoded to a library or executable, run
      'otool -L (library or executable name)'
    the location of the libraries the executable uses can be changed with a command line tool
    called 'install_name_tool'.  For example, to use the libsixense.dylib in the same path as the
    sixense_simple3d instead of /usr/local/lib, run the command:
    install_name_tool -change /usr/local/lib/libsixense.dylib @executable_path/libsixense.dylib sixense_simple3d

    Note that dynamic libraries may also have hardcoded links to other libraries. for example,
    libsixense_utils.dylib by default will look for libsixense.dylib in /usr/local/lib.  This can
    also be changed with install_name_tool.

5.2 Tested Operating Systems
----------------------------

5.2.1 Linux
----------------------------
    The 32 bit versions of the libsixense libraries and the sixense_simple3d binary have been compiled using 
    gcc 4.4.2 linked to glibc 2.11.1.
    This sixense_simple3d app has been tested on the following systems:
    Fedora 15 32 bit
    Ubuntu 10.04 32 bit 
    Ubuntu 11.10 32 bit

    The 64 bit versions of the libsixense libraries and the sixense_simple3d binary have been compiled using 
    gcc 4.4.2 linked to glibc 2.11.1.
    This sixense_simple3d app has been tested on the following systems:
    Fedora 15    64 bit
    Fedora 16    64 bit
    Ubuntu 11.10 64 bit

5.2.2 OSX
---------
The 64 and 32 bit versions of the libsixense libraries and the sixense_simple3d binary have been tested on:
OSX Lion 10.7.1 (11826), X server version: xorg 1.10.2 

The 32 bit version of the libsixense libraries and the sixense_simple3d binary have been tested on:
OSX Snow Leopard 10.6.8 (10K549), X server version: xquartz 2.3.6 (xorg 1.4.2-apple-56)


