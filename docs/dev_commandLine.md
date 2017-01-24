### Compilation & package preparation

Hints related to command line compilation and package preparation
may be found in [shell script](build_mingw@win.cmd) in project root.

#### Hints For macOS

```
# Install dependencies
brew install qt5
brew install openconnect

cd openconnect-gui

# Make sure all git submodules are up-to-date
git submodule init
git submodule update

# Build it
mkdir build-release
cd build-release
Qt5_DIR=/usr/local/opt/qt5/lib/cmake/Qt5 cmake -DCMAKE_BUILD_TYPE=Release ..
make
```
