# OpenConnect-GUI

[![Join the chat at https://gitter.im/openconnect-gui/Lobby](https://badges.gitter.im/openconnect-gui/Lobby.svg)](https://gitter.im/openconnect-gui/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge) 
[![Build Status](https://travis-ci.org/openconnect/openconnect-gui.svg?branch=master)](https://travis-ci.org/openconnect/openconnect-gui)
[![Stories in Ready](https://badge.waffle.io/openconnect/openconnect-gui.svg?label=ready&title=Ready)](http://waffle.io/openconnect/openconnect-gui)

This is the GUI client for openconnect VPN.
This client is in beta testing phase. It cannot be assumed to provide
the required security.

Look to [OpenConnect-GUI](http://openconnect.github.io/openconnect-gui/) project webpage for detailed description and screen shots.

## Development info

### Software requirements
#### Windows 10
* Qt 5.6.x (incl. mingw32) & QtCreator 4+
* NSIS 3.0+
* CMake 3.6+

Newer or older versions may work... feel free tweak main CMakeLists.txt as you need.

#### Fedora 24
Minimum of required packages to build this GUI should be installed by following commands:

    sudo dnf install cmake
    sudo dnf install qt5-qtbase-devel
    sudo dnf install gnutls-devel openconnect-devel

##### Fedora 24 (MinGW)
Just try install following packages:

    sudo dnf install cmake
    sudo dnf install mingw32-qt5-qtbase

### Compilation & package preparation
Hints related to command line compilation and release package preparation
may be found in 'build_mingw@win.cmd' shell script in project root.

Project use git-flow technique i.e. the master contains stable/released versions
and main programming activities are in 'develop' or 'feature/*' branches.
Version string auto=generated on compilation from Git info into following format:
	
	<major>.<minor>.<patch>[-rev_count-sha1][-dirty]

### Development with QtCreator
- checkout project from GitHub
- start QtCreator
- choose session if make sense
- open CMakeLists.txt from project root
- update desired Qt 5.6 version build types and click "Configure"
- open 'Project' tab on left side of QtCreator with CMake configuration
- change
	- 'PROJ_UAC_ON' to 'off' because QtCreator is not able to start app with UAC (?) :/
	- if you like; change 'PROJ_INI_SETTINGS' to 'off' if you want store all settings into file instead of Registry
- click 'Apply Configuration Changes' and then switch again to 'Edit' tab on let side of QtCreator
- build the project (optionally setup MAKEFLAGS in Projects settings if you like for faster build)

#### Known build issues
- in openconnect.h, add on line 466 this (should be fixed in openconnect project):
	#include <winsock2.h>
- update 3rd party DLL's in build folder i.e. copy all DLL's from nsis folder
into build's bin folder (this may/will be fixed later automatically bu Cmake)

### Webpage maintenance
Few hints how to update OpenConnect-GUI web page screen shots and content
may be found in [webpage maintenance](README_webpage.md) document.

## Other
### OpenConnect compilation
The 'openconnect' binary and libraries incl. dependent libraries
used in this project are actually build on Fedora 24
via [this script](contrib/build_mingw-openconnect@fedora24.sh).

All libraries, headers and other dependencies needed to deploy on windows
developmenet machine may be packaged
via [this script](contrib/build_mingw-openconnect-dependencies@fedora24.sh).

These commands and paths may vary...

# License
The content of this project itself is licensed under the [GNU General Public License v2](LICENSE.txt)
