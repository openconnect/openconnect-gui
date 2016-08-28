# OpenConnect-GUI

[![Join the chat at https://gitter.im/openconnect-gui/Lobby](https://badges.gitter.im/openconnect-gui/Lobby.svg)](https://gitter.im/openconnect-gui/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

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
1. images/screenshots preaparation:
	- prepare desired screen shots (common width is 650px)
	- checkout project into 'gh-pages' branch
	- create or upload images to e.g. screenshots folder
	- commit & submit - and continue with web-text changes...
2. web page text:
	- open GitHub project settings
	- look for GitHub pages and click "Launch automatic page generator"
	- edit 'markdown' page content
	- click "Continue to layouts"
	- choose 'Tactile' style in list
	- review your changes and finish with clicking to "Publish page"


## Other
### OpenConnect compilation
The 'openconnect' binary and libraries incl. dependent libraries used in this project are actually build on Fedora 24 via following commands:

	#
	# Sample script to checkout & build 'openconnect' project
	# with ming32 toolchain
	#

	sudo dnf install mingw32-gnutls mingw32-libxml2
	sudo dnf install gcc libtool
	sudo dnf install git p7zip
	
	mkdir work && cd work
	
	git clone https://github.com/cernekee/stoken
	cd stoken
	git checkout v0.90
	./autogen.sh
	mkdir build32 && cd build32
	mingw32-configure
	mingw32-make -j4
	sudo mingw32-make install
	cd ../../
	
	git clone git://git.infradead.org/users/dwmw2/openconnect.git
	cd openconnect
	git checkout v7.06
	./autogen.sh
	mkdir build32 && cd build32
	mingw32-configure --with-vpnc-script=vpnc-script.js
	mingw32-make -j4
	cd ../../

and package to deploy on windows development machine via these commands:

	#
	# Sample script to create a package from build 'openconnect' project
	# incl. all dependencies (hardcoded paths!)
	#

	rm -rf pkg
	mkdir -p pkg/nsis && cd pkg/nsis
	cp /usr/i686-w64-mingw32/sys-root/mingw/bin/iconv.dll .
	cp /usr/i686-w64-mingw32/sys-root/mingw/bin/libffi-6.dll .
	cp /usr/i686-w64-mingw32/sys-root/mingw/bin/libgcc_s_sjlj-1.dll .
	cp /usr/i686-w64-mingw32/sys-root/mingw/bin/libgmp-10.dll .
	cp /usr/i686-w64-mingw32/sys-root/mingw/bin/libgnutls-30.dll .
	cp /usr/i686-w64-mingw32/sys-root/mingw/bin/libhogweed-4-2.dll .
	cp /usr/i686-w64-mingw32/sys-root/mingw/bin/libintl-8.dll .
	cp /usr/i686-w64-mingw32/sys-root/mingw/bin/libnettle-6-2.dll .
	cp /usr/i686-w64-mingw32/sys-root/mingw/bin/libp11-kit-0.dll .
	cp /usr/i686-w64-mingw32/sys-root/mingw/bin/libtasn1-6.dll .
	cp /usr/i686-w64-mingw32/sys-root/mingw/bin/libwinpthread-1.dll .
	cp /usr/i686-w64-mingw32/sys-root/mingw/bin/libxml2-2.dll .
	cp /usr/i686-w64-mingw32/sys-root/mingw/bin/zlib1.dll .
	cp /usr/i686-w64-mingw32/sys-root/mingw/bin/libstoken-1.dll .
	cp ../../openconnect/build32/.libs/libopenconnect-5.dll .
	cp ../../openconnect/build32/.libs/openconnect.exe .
	cd ../../

	mkdir -p pkg/lib && cd pkg/lib
	cp /usr/i686-w64-mingw32/sys-root/mingw/lib/libgmp.dll.a .
	cp /usr/i686-w64-mingw32/sys-root/mingw/lib/libgnutls.dll.a .
	cp /usr/i686-w64-mingw32/sys-root/mingw/lib/libhogweed.dll.a .
	cp /usr/i686-w64-mingw32/sys-root/mingw/lib/libnettle.dll.a .
	cp /usr/i686-w64-mingw32/sys-root/mingw/lib/libp11-kit.dll.a .
	cp /usr/i686-w64-mingw32/sys-root/mingw/lib/libxml2.dll.a .
	cp /usr/i686-w64-mingw32/sys-root/mingw/lib/libz.dll.a .
	cp /usr/i686-w64-mingw32/sys-root/mingw/lib/libstoken.dll.a .
	cp ../../openconnect/build32/.libs/libopenconnect.dll.a .
	cd ../../

	mkdir -p pkg/lib/pkgconfig && cd pkg/lib/pkgconfig
	cp /usr/i686-w64-mingw32/sys-root/mingw/lib/pkgconfig/gnutls.pc .
	cp /usr/i686-w64-mingw32/sys-root/mingw/lib/pkgconfig/hogweed.pc .
	cp /usr/i686-w64-mingw32/sys-root/mingw/lib/pkgconfig/libxml-2.0.pc .
	cp /usr/i686-w64-mingw32/sys-root/mingw/lib/pkgconfig/nettle.pc .
	cp /usr/i686-w64-mingw32/sys-root/mingw/lib/pkgconfig/zlib.pc .
	cp /usr/i686-w64-mingw32/sys-root/mingw/lib/pkgconfig/stoken.pc .
	cp ../../../openconnect/build32/openconnect.pc .
	cd ../../../
	
	mkdir -p pkg/include && cd pkg/include
	cp -R /usr/i686-w64-mingw32/sys-root/mingw/include/gnutls/ .
	cp -R /usr/i686-w64-mingw32/sys-root/mingw/include/libxml2/ .
	cp -R /usr/i686-w64-mingw32/sys-root/mingw/include/nettle/ .
	cp -R /usr/i686-w64-mingw32/sys-root/mingw/include/p11-kit-1/p11-kit/ .
	cp /usr/i686-w64-mingw32/sys-root/mingw/include/gmp.h .
	cp /usr/i686-w64-mingw32/sys-root/mingw/include/zconf.h .
	cp /usr/i686-w64-mingw32/sys-root/mingw/include/zlib.h .
	cp /usr/i686-w64-mingw32/sys-root/mingw/include/stoken.h .
	cp ../../openconnect/openconnect.h .
	cd ../../
	
	7za a pkg_"$(date)".7z pkg
	
	cd stoken/build32
	sudo mingw32-make uninstall
	
	echo "List of system-wide used packages versions:"
	rpm -qv \
	    mingw32-gnutls \
	    mingw32-gmp \
	    mingw32-nettle \
	    mingw32-p11-kit \
	    mingw32-zlib \
	    mingw32-libxml2

These commands and paths may vary...

# License
The content of this project itself is licensed under the [GNU General Public License v2](LICENSE.txt)
