#
# Sample script to checkout & build 'openconnect' project
# with mingw32 toolchain
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
