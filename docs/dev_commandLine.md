### Compilation & package preparation

Hints related to command line compilation and package preparation
may be found in [shell script](../contrib/build_mingw@win.cmd) in project root.

#### Hints For macOS

Install [Homebrew](https://docs.brew.sh/) system-wide:
```
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"

```
or localy:
```
mkdir homebrew && curl -L https://github.com/Homebrew/brew/tarball/master | tar xz --strip 1 -C homebrew
```
and continue:

```
# Install dependencies
brew install qt5
brew install openconnect

cd openconnect-gui

# Build it
mkdir build-release
cd build-release
Qt5_DIR=/usr/local/opt/qt5/lib/cmake/Qt5 cmake -DCMAKE_BUILD_TYPE=Release ..
make
```
