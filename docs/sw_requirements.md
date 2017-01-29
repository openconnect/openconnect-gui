### Software requirements
This chapter summarize actual development environment. Newer or older versions may work - feel free tweak main CMakeLists.txt and dependencies as you need.

#### Windows 10
This is target platform for the product.

- [Qt 5 (incl. mingw32) & QtCreator 4+](http://download.qt.io/official_releases/online_installers/qt-unified-windows-x86-online.exe) and choose:
    - 5.8/MinGW 5.3.0 (32-bit)
    - Tools/MinGW 5.3.0 component
    - Tools/QtCreator component
- [OpenConnect 7.08 for MinGW32] (https://github.com/horar/openconnect/releases/tag/v7.08)
- [spdlog 0.11.0](https://github.com/gabime/spdlog) ... as Git-submodule
- [qt-solutions] (https://github.com/qtproject/qt-solutions.git) ... as Git-submodule
- [NSIS 3.0+](http://nsis.sourceforge.net/Main_Page)
- [CMake 3.6+](https://cmake.org/)
- [Git 2.7+](https://git-scm.com/)

#### GNU/Linux
##### Fedora 25 (MinGW)
Just try install following packages:

    sudo dnf install cmake
    sudo dnf install mingw32-qt5-qtbase
    sudo dnf install mingw32-nsis

##### Fedora 25
This is not main target platform, because openconnect is used by other projects integrated into desktop environments. On the other side - the minimum of required packages to build this GUI should be installed by following commands:

    sudo dnf install cmake
    sudo dnf install qt5-qtbase-devel
    sudo dnf install gnutls-devel openconnect-devel

#### MacOS
- MacOS 10.12+
- Xcode 8.2.1+ incl. command line tools
- [Qt online installer](http://download.qt.io/official_releases/online_installers/qt-unified-mac-x64-online.dmg) and choose:
    - 5.8/"OS X" component
    - Tools/QtCreator component
