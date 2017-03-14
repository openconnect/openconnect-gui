# - Try to find OpenConnect
# Once done this will define
#
#  OPENCONNECT_FOUND - system has OpenConnect
#  OPENCONNECT_INCLUDE_DIRS - the OpenConnect include directories
#  OPENCONNECT_LIBRARIES - the libraries needed to use OpenConnect
#  OPENCONNECT_CFLAGS - Compiler switches required for using OpenConnect
#  OPENCONNECT_VPNC_SCRIPT - location of 'vpnc-script' of OpenConnect
#  OPENCONNECT_VERSION - version number of OpenConnect

# Copyright (c) 2011, Ilia Kats <ilia-kats@gmx.net>
# Copyright (c) 2016, Lubomir Carik <lubomir.carik@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


if(OPENCONNECT_INCLUDE_DIRS)
	# in cache already
	set(OpenConnect_FIND_QUIETLY TRUE)
endif(OPENCONNECT_INCLUDE_DIRS)

if(NOT WIN32)
	# use pkg-config to get the directories and then use these values
	# in the FIND_PATH() and FIND_LIBRARY() calls
	find_package(PkgConfig REQUIRED)
	pkg_search_module(OPENCONNECT openconnect)
	if(OPENCONNECT_FOUND)
		find_program(OPENCONNECT_EXECUTABLE
			NAMES openconnect
		)
		if(OPENCONNECT_EXECUTABLE)
			if(APPLE)
				# Homebrew custom openconnect (bottle) installation
				execute_process(
					COMMAND dirname ${OPENCONNECT_EXECUTABLE}
					OUTPUT_VARIABLE output
					ERROR_VARIABLE output
				)
				string(STRIP ${output} output)
				set(OPENCONNECT_VPNC_SCRIPT "${output}/../etc/vpnc-script")
			else()
				execute_process(
					COMMAND ${OPENCONNECT_EXECUTABLE} -help
					OUTPUT_VARIABLE output
					ERROR_VARIABLE output
				)
				string(REGEX MATCH "default: \".*\"" vpnc-script-location "${output}")
				string(REPLACE "default: " "" vpnc-script-location ${vpnc-script-location})
				string(REPLACE "\"" "" OPENCONNECT_VPNC_SCRIPT ${vpnc-script-location})
			endif()
		endif()
	endif()
endif(NOT WIN32)

if(OPENCONNECT_FOUND)
	if(NOT OpenConnect_FIND_QUIETLY)
		message(STATUS "Found OpenConnect ${OPENCONNECT_VERSION}: ${OPENCONNECT_LIBRARIES}")
		if(OPENCONNECT_VPNC_SCRIPT)
			message(STATUS "Found vpnc-script ${OPENCONNECT_VPNC_SCRIPT}")
		endif()
	endif()
else()
	if(OpenConnect_FIND_REQUIRED)
		message(FATAL_ERROR "Could NOT find OpenConnect, check FindPkgConfig output above!")
	endif()
endif()

mark_as_advanced(OPENCONNECT_INCLUDE_DIRS OPENCONNECT_LIBRARIES OPENCONNECT_STATIC_LIBRARIES)
