set(openconnect-TAG "v8.02" CACHE STRING "Please specify openconnect version")
set(openconnect-TAG_CHOICES "v8.02" "v7.08" "master")
set_property(CACHE openconnect-TAG PROPERTY STRINGS ${openconnect-TAG_CHOICES})
if(NOT openconnect-TAG IN_LIST openconnect-TAG_CHOICES)
  message(FATAL_ERROR "Specify 'openconnect-TAG'. Must be one of ${openconnect-TAG_CHOICES}")
endif()

set(vpnc-scripts-TAG master)
set(spdlog-TAG v1.3.0)
set(qt-solutions-TAG master)
set(tap-driver-TAG 9.21.2)

if(CMAKE_CROSSCOMPILING AND MINGW)
    # Fedora mingw32/mingw64
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(CMAKE_CROSS_COMMAND mingw64-cmake)
    else()
        set(CMAKE_CROSS_COMMAND mingw32-cmake)
    endif()
else()
    # Windows mingw32 & macOS & native GNU/Linux
    set(CMAKE_CROSS_COMMAND ${CMAKE_COMMAND})
endif()
message(STATUS "Using '${CMAKE_CROSS_COMMAND}' as CMake...")


include(ExternalProject)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/external/include)

include(ProjectExternals_spdlog)
include(ProjectExternals_qt-solutions)
if(MINGW)
    include(ProjectExternals_openconnect)
    include(ProjectExternals_vpnc-scripts-win)
    include(ProjectExternals_tap-windows)
endif()

