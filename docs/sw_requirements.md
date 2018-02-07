### Software requirements
This chapter summarize actual development environment. Newer or older versions may work - feel free tweak main CMakeLists.txt and dependencies as you need.

#### Windows 10
This is target platform for the product.

- [Qt 5 (incl. mingw32) & QtCreator 4+](http://download.qt.io/official_releases/online_installers/qt-unified-windows-x86-online.exe) and choose:
    - 5.10.0/MinGW 5.3.0 (32-bit)
    - Tools/MinGW 5.3.0 component
    - Tools/QtCreator component
    - Tools/QtCreator CDB Debugger support
- [NSIS 3.0+](http://nsis.sourceforge.net/Main_Page)
- [CMake 3.6+](https://cmake.org/)
- [Git 2.7+](https://git-scm.com/)

##### Externals

These projects will be downloaded automatically by CMake.

- [OpenConnect for MinGW32](https://github.com/horar/openconnect/releases)
- [TAP-windoes](https://openvpn.net/index.php/open-source/downloads.html)
- [spdlog](https://github.com/gabime/spdlog)
- [qt-solutions](https://github.com/qtproject/qt-solutions.git)


#### GNU/Linux
##### Fedora 27 (MinGW32)
Just try install following packages:

    sudo dnf install -y git
    sudo dnf install -y cmake
    sudo dnf install -y mingw32-qt5-qtbase
    sudo dnf install -y mingw32-nsis

##### Fedora 27
This is not main target platform, because openconnect is used by other projects integrated into desktop environments. On the other side - the minimum of required packages to build this GUI should be installed by following commands:

    sudo dnf install -y git gitflow
    sudo dnf install -y cmake
    sudo dnf install -y qt5-qtbase-devel gcc-c++
    sudo dnf install -y gnutls-devel openconnect-devel
    ~~sudo dnf install -y spdlog-devel~~

#### MacOS
- MacOS 10.10+
- Xcode 9.2+ incl. command line tools
- [Qt online installer](http://download.qt.io/official_releases/online_installers/qt-unified-mac-x64-online.dmg) and choose:
    - 5.10.0/"OS X" component
    - Tools/QtCreator component
