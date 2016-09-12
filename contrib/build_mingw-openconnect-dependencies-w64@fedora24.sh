#
# Sample script to create a package from build 'openconnect' project
# incl. all dependencies (hardcoded paths!)
#

rm -rf pkg
mkdir -p pkg/nsis && cd pkg/nsis
cp /usr/x86_64-w64-mingw32/sys-root/mingw/bin/iconv.dll .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/bin/libffi-6.dll .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/bin/libgcc_s_sjlj-1.dll .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/bin/libgmp-10.dll .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/bin/libgnutls-30.dll .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/bin/libhogweed-4-2.dll .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/bin/libintl-8.dll .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/bin/libnettle-6-2.dll .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/bin/libp11-kit-0.dll .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/bin/libtasn1-6.dll .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/bin/libwinpthread-1.dll .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/bin/libxml2-2.dll .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/bin/zlib1.dll .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/bin/libstoken-1.dll .
cp ../../openconnect/build64/.libs/libopenconnect-5.dll .
cp ../../openconnect/build64/.libs/openconnect.exe .
cd ../../

mkdir -p pkg/lib && cd pkg/lib
cp /usr/x86_64-w64-mingw32/sys-root/mingw/lib/libgmp.dll.a .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/lib/libgnutls.dll.a .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/lib/libhogweed.dll.a .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/lib/libnettle.dll.a .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/lib/libp11-kit.dll.a .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/lib/libxml2.dll.a .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/lib/libz.dll.a .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/lib/libstoken.dll.a .
cp ../../openconnect/build64/.libs/libopenconnect.dll.a .
cd ../../

mkdir -p pkg/lib/pkgconfig && cd pkg/lib/pkgconfig
cp /usr/x86_64-w64-mingw32/sys-root/mingw/lib/pkgconfig/gnutls.pc .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/lib/pkgconfig/hogweed.pc .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/lib/pkgconfig/libxml-2.0.pc .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/lib/pkgconfig/nettle.pc .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/lib/pkgconfig/zlib.pc .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/lib/pkgconfig/stoken.pc .
cp ../../../openconnect/build64/openconnect.pc .
cd ../../../

mkdir -p pkg/include && cd pkg/include
cp -R /usr/x86_64-w64-mingw32/sys-root/mingw/include/gnutls/ .
cp -R /usr/x86_64-w64-mingw32/sys-root/mingw/include/libxml2/ .
cp -R /usr/x86_64-w64-mingw32/sys-root/mingw/include/nettle/ .
cp -R /usr/x86_64-w64-mingw32/sys-root/mingw/include/p11-kit-1/p11-kit/ .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/include/gmp.h .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/include/zconf.h .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/include/zlib.h .
cp /usr/x86_64-w64-mingw32/sys-root/mingw/include/stoken.h .
cp ../../openconnect/openconnect.h .
cd ../../

7za a pkg_w64_"$(date)".7z pkg


cd stoken/build64
sudo mingw64-make uninstall

echo "List of system-wide used packages versions:"
rpm -qv \
    mingw64-gnutls \
    mingw64-gmp \
    mingw64-nettle \
    mingw64-p11-kit \
    mingw64-zlib \
    mingw64-libxml2
   