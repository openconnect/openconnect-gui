#
# Sample script to checkout & build 'openconnect' project
# with mingw32 on MSYS2 toolchain
#
# It should be used only as illustration how to build application
# and create an installer package
#
# (c) 2018, Lubomir Carik
#

[ "$MSYSTEM" != "MINGW32" ] && exit -1
echo "Starting under MINGW32 build environment..."

export OC_TAG=v7.08
export STOKEN_TAG=v0.92

pacman --needed --noconfirm -S \
    mingw-w64-i686-gnutls \
    mingw-w64-i686-libidn2 \
    mingw-w64-i686-libunistring \
    mingw-w64-i686-nettle \
    mingw-w64-i686-gmp \
    mingw-w64-i686-p11-kit \
    mingw-w64-i686-zlib \
    mingw-w64-i686-libxml2 \
    mingw-w64-i686-zlib \
    mingw-w64-i686-libxml2 \
    mingw-w64-i686-lz4 \
    mingw-w64-i686-libproxy


[ -d work ] || mkdir work
cd work

[ -d stoken ] ||  git clone https://github.com/cernekee/stoken
cd stoken
git checkout -b ${STOKEN_TAG} ${STOKEN_TAG}
./autogen.sh
[ -d build32 ] || mkdir build32
cd build32
git clean -fdx
../configure --disable-dependency-tracking --without-tomcrypt --without-gtk
mingw32-make -j4
mingw32-make install
cd ../../

[ -d openconnect ] || git clone git://git.infradead.org/users/dwmw2/openconnect.git
cd openconnect
git reset --hard
git checkout -b ${OC_TAG} ${OC_TAG}
./autogen.sh
[ -d build32 ] || mkdir build32
cd build32
git clean -fdx
../configure --disable-dependency-tracking --with-gnutls --without-openssl --without-libpskc --with-vpnc-script=vpnc-script-win.js
mingw32-make -j4
cd ../../


#
# Sample script to create a package from build 'openconnect' project
# incl. all dependencies (hardcoded paths!)
#

export MINGW_PREFIX=/mingw32

rm -rf pkg
mkdir -p pkg/nsis && cd pkg/nsis
#cp ${MINGW_PREFIX}/bin/iconv.dll .
cp ${MINGW_PREFIX}/bin/libffi-6.dll .
cp ${MINGW_PREFIX}/bin/libgcc_*-1.dll .
cp ${MINGW_PREFIX}/bin/libgmp-10.dll .
cp ${MINGW_PREFIX}/bin/libgnutls-30.dll .
cp ${MINGW_PREFIX}/bin/libhogweed-4.dll .
cp ${MINGW_PREFIX}/bin/libintl-8.dll .
cp ${MINGW_PREFIX}/bin/libnettle-6.dll .
cp ${MINGW_PREFIX}/bin/libp11-kit-0.dll .
cp ${MINGW_PREFIX}/bin/libtasn1-6.dll .
cp ${MINGW_PREFIX}/bin/libwinpthread-1.dll .
cp ${MINGW_PREFIX}/bin/libxml2-2.dll .
cp ${MINGW_PREFIX}/bin/zlib1.dll .
cp ${MINGW_PREFIX}/bin/libstoken-1.dll .
cp ${MINGW_PREFIX}/bin/libproxy-1.dll .
cp ${MINGW_PREFIX}/bin/liblz4.dll .
cp ${MINGW_PREFIX}/bin/libiconv-2.dll .
cp ${MINGW_PREFIX}/bin/libunistring-2.dll .
cp ${MINGW_PREFIX}/bin/libidn2-0.dll .
cp ${MINGW_PREFIX}/bin/libstdc++-6.dll .
cp ${MINGW_PREFIX}/bin/liblzma-5.dll .
cp ../../openconnect/build32/.libs/libopenconnect-5.dll .
cp ../../openconnect/build32/.libs/openconnect.exe .
curl -v -o vpnc-script-win.js http://git.infradead.org/users/dwmw2/vpnc-scripts.git/blob_plain/HEAD:/vpnc-script-win.js
cd ../../

mkdir -p pkg/lib && cd pkg/lib
cp ${MINGW_PREFIX}/lib/libgmp.dll.a .
cp ${MINGW_PREFIX}/lib/libgnutls.dll.a .
cp ${MINGW_PREFIX}/lib/libhogweed.dll.a .
cp ${MINGW_PREFIX}/lib/libnettle.dll.a .
cp ${MINGW_PREFIX}/lib/libp11-kit.dll.a .
cp ${MINGW_PREFIX}/lib/libxml2.dll.a .
cp ${MINGW_PREFIX}/lib/libz.dll.a .
cp ${MINGW_PREFIX}/lib/libstoken.dll.a .
cp ${MINGW_PREFIX}/lib/libproxy.dll.a .
cp ${MINGW_PREFIX}/lib/liblz4.dll.a .
cp ${MINGW_PREFIX}/lib/libiconv.dll.a .
cp ${MINGW_PREFIX}/lib/libunistring.dll.a .
cp ${MINGW_PREFIX}/lib/libidn2.dll.a .
cp ${MINGW_PREFIX}/lib/liblzma.dll.a .
cp ../../openconnect/build32/.libs/libopenconnect.dll.a .
cd ../../

mkdir -p pkg/lib/pkgconfig && cd pkg/lib/pkgconfig
cp ${MINGW_PREFIX}/lib/pkgconfig/gnutls.pc .
cp ${MINGW_PREFIX}/lib/pkgconfig/hogweed.pc .
cp ${MINGW_PREFIX}/lib/pkgconfig/libxml-2.0.pc .
cp ${MINGW_PREFIX}/lib/pkgconfig/nettle.pc .
cp ${MINGW_PREFIX}/lib/pkgconfig/zlib.pc .
cp ${MINGW_PREFIX}/lib/pkgconfig/stoken.pc .
cp ../../../openconnect/build32/openconnect.pc .
cd ../../../

mkdir -p pkg/include && cd pkg/include
cp -R ${MINGW_PREFIX}/include/gnutls/ .
cp -R ${MINGW_PREFIX}/include/libxml2/ .
cp -R ${MINGW_PREFIX}/include/nettle/ .
cp -R ${MINGW_PREFIX}/include/p11-kit-1/p11-kit/ .
cp ${MINGW_PREFIX}/include/gmp.h .
cp ${MINGW_PREFIX}/include/zconf.h .
cp ${MINGW_PREFIX}/include/zlib.h .
cp ${MINGW_PREFIX}/include/stoken.h .
cp ../../openconnect/openconnect.h .
cd ../../

export MINGW_PREFIX=

cd pkg/nsis
7za a -tzip -mx=9 -sdel ../../openconnect-${OC_TAG}_mingw32.zip *
cd ../
rmdir -v nsis
7za a -tzip -mx=9 -sdel ../openconnect-devel-${OC_TAG}_mingw32.zip *
cd ../
rmdir -v pkg


#cd stoken/build32
#sudo mingw32-make uninstall

echo "List of system-wide used packages versions:" \
    > openconnect-${OC_TAG}_mingw32.txt
echo "openconnect-${OC_TAG}" \
    >> openconnect-${OC_TAG}_mingw32.txt
echo "stoken-${STOKEN_TAG}" \
    >> openconnect-${OC_TAG}_mingw32.txt
pacman -Q \
    mingw-w64-i686-gnutls \
    mingw-w64-i686-libidn2 \
    mingw-w64-i686-libunistring \
    mingw-w64-i686-nettle \
    mingw-w64-i686-gmp \
    mingw-w64-i686-p11-kit \
    mingw-w64-i686-libxml2 \
    mingw-w64-i686-zlib \
    mingw-w64-i686-libxml2 \
    mingw-w64-i686-lz4 \
    mingw-w64-i686-libproxy \
    >> openconnect-${OC_TAG}_mingw32.txt

sha512sum.exe openconnect-${OC_TAG}_mingw32.zip > openconnect-${OC_TAG}_mingw32.zip.sha512
sha512sum.exe openconnect-devel-${OC_TAG}_mingw32.zip > openconnect-devel-${OC_TAG}_mingw32.zip.sha512

mv -v openconnect-*.zip openconnect-*.txt openconnect-*.zip.sha512 ..
