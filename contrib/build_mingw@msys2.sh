#
# Sample build script & release package preapration
#
# It should be used only as illustration how to build application
# and create an installer package
#
# (c) 2016-2018, Lubomir Carik
#

echo "======================================================================="
echo " Preparing sandbox..."
echo "======================================================================="
mkdir -pv work/build-release-$MSYSTEM

echo "======================================================================="
echo " Generating project..."
echo "======================================================================="
cd work/build-release-$MSYSTEM
cmake -G "MSYS Makefiles" \
    -DCMAKE_BUILD_TYPE=Release \
    ../../..

echo "======================================================================="
echo " Compiling..."
echo "======================================================================="
make -j5

# echo "======================================================================="
# echo " LC: Bundling... (dynamic Qt only)"
# echo "======================================================================="
# rd /s /q out
# md out
# windeployqt ^
#   src\openconnect-gui.exe ^
#   --verbose 1 ^
#   --compiler-runtime ^
#   --release ^
#   --force ^
#   --no-webkit2 ^
#   --no-quick-import ^
#   --no-translations

echo "======================================================================="
echo " Packaging..."
echo "======================================================================="
cmake .
make package VERBOSE=1
# make package_source VERBOSE=1

mv -vf *.exe ../..
mv -vf *.exe.sha512 ../..

cd ..
