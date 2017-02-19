set(openconnect-TAG v7.08)
set(vpnc-scripts-TAG master)
#set(spdlog-TAG v0.11.0)
set(spdlog-TAG master)
set(qt-solutions-TAG master)
set(tap-driver-TAG 9.21.2)


include(ExternalProject)

# --------------------------------------------------------------------------------------------------
# openconnect (libs, headers, etc.)
# --------------------------------------------------------------------------------------------------
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/external/openconnect-devel-${openconnect-TAG}_mingw32.zip)
    set(OPENCONNECT_URL https://github.com/horar/openconnect/releases/download/${openconnect-TAG})
else()
    message(STATUS "Using local openconnect-devel packages...")
    set(OPENCONNECT_URL ${CMAKE_SOURCE_DIR}/external)
endif()

ExternalProject_Add(openconnect-devel-${openconnect-TAG}
    PREFIX ${CMAKE_BINARY_DIR}/external
    INSTALL_DIR ${CMAKE_SOURCE_DIR}/external
    
    DOWNLOAD_NO_PROGRESS 1
    
    URL ${OPENCONNECT_URL}/openconnect-devel-${openconnect-TAG}_mingw32.zip

    BUILD_COMMAND ""
    CONFIGURE_COMMAND ""
    INSTALL_COMMAND ""
)
ExternalProject_Add_Step(openconnect-devel-${openconnect-TAG} deploy_headers
    COMMAND ${CMAKE_COMMAND} -E copy_directory include <INSTALL_DIR>/include
    COMMENT "...deploing openconnect-${openconnect-TAG} headers"
    WORKING_DIRECTORY <SOURCE_DIR>
    DEPENDEES install
)
ExternalProject_Add_Step(openconnect-devel-${openconnect-TAG} deploy_libraries
    COMMAND ${CMAKE_COMMAND} -E copy_directory lib <INSTALL_DIR>/lib
    COMMENT "...deploing openconnect-${openconnect-TAG} libraries"
    WORKING_DIRECTORY <SOURCE_DIR>
    DEPENDEES install
    ALWAYS 0
)

# --------------------------------------------------------------------------------------------------
# openconnect (util & libs)
# --------------------------------------------------------------------------------------------------
ExternalProject_Add(openconnect-${openconnect-TAG}
    PREFIX ${CMAKE_BINARY_DIR}/external
    INSTALL_DIR ${CMAKE_SOURCE_DIR}/external
    
    URL ${OPENCONNECT_URL}/openconnect-${openconnect-TAG}_mingw32.zip

    BUILD_COMMAND ""
    CONFIGURE_COMMAND ""
    INSTALL_COMMAND ""
)
ExternalProject_Add_Step(openconnect-${openconnect-TAG} deploy_libs
    COMMAND ${CMAKE_COMMAND} -E copy_directory . <INSTALL_DIR>/lib
    COMMENT "...deploing openconnect-${openconnect-TAG} libraries"
    WORKING_DIRECTORY <SOURCE_DIR>
    DEPENDEES install
)

add_library(libgmp SHARED IMPORTED)
set_property(TARGET libgmp PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/external/lib/libgmp-10.dll)
set_property(TARGET libgmp PROPERTY IMPORTED_IMPLIB ${CMAKE_SOURCE_DIR}/external/lib/libgmp.dll.a)

add_library(libgnutls SHARED IMPORTED)
set_property(TARGET libgnutls PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/external/lib/libgnutls-30.dll)
set_property(TARGET libgnutls PROPERTY IMPORTED_IMPLIB ${CMAKE_SOURCE_DIR}/external/lib/libgnutls.dll.a)

add_library(libhogweed SHARED IMPORTED)
set_property(TARGET libhogweed PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/external/lib/libhogweed-4.dll)
set_property(TARGET libhogweed PROPERTY IMPORTED_IMPLIB ${CMAKE_SOURCE_DIR}/external/lib/libhogweed.dll.a)

add_library(libnettle SHARED IMPORTED)
set_property(TARGET libnettle PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/external/lib/libnettle-6.dll)
set_property(TARGET libnettle PROPERTY IMPORTED_IMPLIB ${CMAKE_SOURCE_DIR}/external/lib/libnettle.dll.a)

add_library(libopenconnect SHARED IMPORTED)
set_property(TARGET libopenconnect PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/external/lib/libopenconnect-5.dll)
set_property(TARGET libopenconnect PROPERTY IMPORTED_IMPLIB ${CMAKE_SOURCE_DIR}/external/lib/libopenconnect.dll.a)

add_library(libp11-kit SHARED IMPORTED)
set_property(TARGET libp11-kit PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/external/lib/libp11-kit-0.dll)
set_property(TARGET libp11-kit PROPERTY IMPORTED_IMPLIB ${CMAKE_SOURCE_DIR}/external/lib/libp11-kit.dll.a)

add_library(libstoken SHARED IMPORTED)
set_property(TARGET libstoken PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/external/lib/libstoken-1.dll)
set_property(TARGET libstoken PROPERTY IMPORTED_IMPLIB ${CMAKE_SOURCE_DIR}/external/lib/libstoken.dll.a)

add_library(libxml2 SHARED IMPORTED)
set_property(TARGET libxml2 PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/external/lib/libxml2-2.dll)
set_property(TARGET libxml2 PROPERTY IMPORTED_IMPLIB ${CMAKE_SOURCE_DIR}/external/lib/libxml2.dll.a)


# --------------------------------------------------------------------------------------------------
# spdlog
# --------------------------------------------------------------------------------------------------
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/external/include)
ExternalProject_Add(spdlog-${spdlog-TAG}
    PREFIX ${CMAKE_BINARY_DIR}/external
    
    INSTALL_DIR ${CMAKE_SOURCE_DIR}/external
    
    UPDATE_DISCONNECTED 0
    UPDATE_COMMAND ""
    
    #DOWNLOAD_NO_PROGRESS 1
    
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG ${spdlog-TAG}
    GIT_SHALLOW 1

    # CMake generator
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> -DBUILD_TESTING=off -DSPDLOG_BUILD_TESTING=off
)

add_library(spdlog::spdlog INTERFACE IMPORTED)
set_target_properties(spdlog::spdlog PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/external/include"
)


# --------------------------------------------------------------------------------------------------
# qt-solutions
# --------------------------------------------------------------------------------------------------
ExternalProject_Add(qt-solutions-${qt-solutions-TAG}
    PREFIX ${CMAKE_BINARY_DIR}/external/
    INSTALL_DIR ${CMAKE_SOURCE_DIR}/external
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
    
    UPDATE_DISCONNECTED 0
    UPDATE_COMMAND ""
    
    GIT_REPOSITORY https://code.qt.io/qt-solutions/qt-solutions.git
    GIT_TAG ${qt-solutions-TAG}
    GIT_SHALLOW 1
    
    BUILD_IN_SOURCE 1
        
    PATCH_COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/external/CMakeLists_qt-solutions.txt ${CMAKE_BINARY_DIR}/external/src/qt-solutions-master/CMakeLists.txt
    #CONFIGURE_COMMAND ""
    #BUILD_COMMAND ""
    #INSTALL_COMMAND ""
)




# --------------------------------------------------------------------------------------------------
# vpnc-scripts
# --------------------------------------------------------------------------------------------------
ExternalProject_Add(vpnc-scripts-${vpnc-scripts-TAG}
    PREFIX ${CMAKE_BINARY_DIR}/external/

    UPDATE_DISCONNECTED 0
    UPDATE_COMMAND ""

    GIT_REPOSITORY git://git.infradead.org/users/dwmw2/vpnc-scripts.git
    GIT_TAG ${vpnc-scripts-TAG}
    GIT_SHALLOW 1
    
    BUILD_IN_SOURCE 1

    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_if_different vpnc-script-win.js ${CMAKE_SOURCE_DIR}/external/vpnc-script.js
)


# --------------------------------------------------------------------------------------------------
# tap-windows drivers
# --------------------------------------------------------------------------------------------------
ExternalProject_Add(tap-windows
    PREFIX ${CMAKE_BINARY_DIR}/external
    INSTALL_DIR ${CMAKE_SOURCE_DIR}/external
    
    UPDATE_DISCONNECTED 0
    UPDATE_COMMAND ""

    DOWNLOAD_NO_PROGRESS 1
    DOWNLOAD_NO_EXTRACT 1

    URL https://swupdate.openvpn.org/community/releases/tap-windows-${tap-driver-TAG}.exe

    BUILD_IN_SOURCE 1

    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_if_different <DOWNLOADED_FILE> ${CMAKE_SOURCE_DIR}/external/tap-windows.exe
)
