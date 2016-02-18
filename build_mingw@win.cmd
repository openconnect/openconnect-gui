@echo off

echo "======================================================================="
echo " Preparing sandbox..."
echo "======================================================================="
rmdir /s /q build-release
md build-release

echo "======================================================================="
echo " Preparing environment..."
echo "======================================================================="
REM look for "Qt 5.5 for Desktop (MinGW 4.9.2 32 bit)" StartMenu item
REM and check 'qtenv2.bat'
echo Setting up environment for Qt usage...
set PATH=C:\Dev\Qt\5.5\mingw492_32\bin;C:\Dev\Qt\Tools\mingw492_32\bin;%PATH%
echo Setting up environment for CMake usage...
set PATH="C:\Program Files (x86)\CMake\bin";%PATH%

echo "======================================================================="
echo " Generating project..."
echo "======================================================================="
cd build-release
cmake -G "MinGW Makefiles" ^
	-DCMAKE_BUILD_TYPE=Release ^
	..\

echo "======================================================================="
echo " Compiling..."
echo "======================================================================="
mingw32-make -j10

REM echo "======================================================================="
REM echo " LC: Bundling... (dynamic Qt only)"
REM echo "======================================================================="
REM rd /s /q out
REM md out
REM windeployqt ^
REM 	src\openconnect-gui.exe ^
REM 	--verbose 1 ^
REM 	--compiler-runtime ^
REM 	--release ^
REM 	--force ^
REM 	--no-webkit2 ^
REM 	--no-quick-import ^
REM 	--no-translations

echo "======================================================================="
echo " Packaging..."
echo "======================================================================="
mingw32-make package VERBOSE=1


cd ..