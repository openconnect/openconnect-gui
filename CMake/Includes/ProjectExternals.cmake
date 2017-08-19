set(openconnect-TAG v7.08)
set(vpnc-scripts-TAG master)
set(spdlog-TAG v0.14.0)
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

