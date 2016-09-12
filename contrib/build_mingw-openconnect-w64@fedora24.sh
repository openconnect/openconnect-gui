#
# Sample script to checkout & build 'openconnect' project
# with mingw64 toolchain
#

sudo dnf install mingw64-gnutls mingw64-libxml2
sudo dnf install gcc libtool
sudo dnf install git p7zip

mkdir work && cd work

git clone https://github.com/cernekee/stoken
cd stoken
git checkout v0.90
./autogen.sh
mkdir build64 && cd build64
mingw64-configure
mingw64-make -j4
sudo mingw64-make install
cd ../../

git clone git://git.infradead.org/users/dwmw2/openconnect.git
cd openconnect
git checkout v7.06
./autogen.sh
mkdir build64 && cd build64
mingw64-configure --with-vpnc-script=vpnc-script.js
mingw64-make -j4
cd ../../
