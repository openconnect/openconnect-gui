# - Try to find OpenConnect
# Once done this will define
#
#  OPENCONNECT_FOUND - system has OpenConnect
#  OPENCONNECT_INCLUDE_DIRS - the OpenConnect include directories
#  OPENCONNECT_LIBRARIES - the libraries needed to use OpenConnect
#  OPENCONNECT_CFLAGS - Compiler switches required for using OpenConnect
#  OPENCONNECT_VERSION - version number of OpenConnect

# Copyright (c) 2011, Ilia Kats <ilia-kats@gmx.net>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


IF (OPENCONNECT_INCLUDE_DIRS)
    # in cache already
    SET(OpenConnect_FIND_QUIETLY TRUE)
ENDIF (OPENCONNECT_INCLUDE_DIRS)

IF (NOT WIN32)
    # use pkg-config to get the directories and then use these values
    # in the FIND_PATH() and FIND_LIBRARY() calls
    find_package(PkgConfig)
    pkg_search_module(OPENCONNECT openconnect)
ENDIF (NOT WIN32)

IF (OPENCONNECT_FOUND)
    IF (NOT OpenConnect_FIND_QUIETLY)
        MESSAGE(STATUS "Found OpenConnect ${OPENCONNECT_VERSION}: ${OPENCONNECT_LIBRARIES}")
    ENDIF (NOT OpenConnect_FIND_QUIETLY)
ELSE (OPENCONNECT_FOUND)
    IF (OpenConnect_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could NOT find OpenConnect, check FindPkgConfig output above!")
    ENDIF (OpenConnect_FIND_REQUIRED)
ENDIF (OPENCONNECT_FOUND)

MARK_AS_ADVANCED(OPENCONNECT_INCLUDE_DIRS OPENCONNECT_LIBRARIES OPENCONNECT_STATIC_LIBRARIES)
