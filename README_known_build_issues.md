#### Known build issues
- in openconnect.h, add on line 466 this (should be fixed in openconnect project):
	
	\#include <winsock2.h>

- update 3rd party DLL's in build folder i.e. copy all DLL's from nsis folder
into build's bin folder (this may/will be fixed later automatically bu CMake)
