cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

if(APPLE)
    if(NOT CMAKE_OSX_SYSROOT)
        set(CMAKE_OSX_SYSROOT "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk" CACHE STRING "" FORCE)
    endif()

     if(NOT CMAKE_OSX_DEPLOYMENT_TARGET)
        set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9" CACHE STRING "" FORCE)
    endif()
endif()

project(openconnect-gui
    VERSION 1.5.1
    LANGUAGES CXX
)

message(STATUS "Building ${PROJECT_NAME} v${PROJECT_VERSION} [cmake]")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/Modules/")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/Includes/")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}/src/")

if(MINGW)
    include(ProjectExternals)
endif()

set(PRODUCT_NAME_SHORT "OpenConnect-GUI")
set(PRODUCT_NAME_LONG "OpenConnect-GUI VPN client")
set(PRODUCT_NAME_COMPANY "OpenConnect-GUI Team")
set(PRODUCT_NAME_COMPANY_DOMAIN "openconnect.github.io")
set(PRODUCT_NAME_COPYRIGHT_SHORT "Copyright (C) 2016-2017 OpenConnect-GUI Team")
set(PRODUCT_NAME_COPYRIGHT_FULL "Copyright (C) 2014-2015 Red Hat Inc.<br>Copyright (C) 2016-2017 OpenConnect-GUI Team")

option(PROJ_GNUTLS_DEBUG "Enable GnuTLS debug mode" OFF)
if(WIN32 AND MINGW)
    option(PROJ_UAC_ON "Enable UAC (don't turn it off in production)" ON)

    set(DEFAULT_VPNC_SCRIPT "vpnc-script.js")
else(APPLE)
    set(DEFAULT_VPNC_SCRIPT "../Resources/vpnc-script")
else()
    set(DEFAULT_VPNC_SCRIPT "/etc/vpnc/vpnc-script")
endif()
option(PROJ_PKCS11 "Enable PKCS11" ON)
option(PROJ_INI_SETTINGS "Store the settings in INI files." OFF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options("-Wall")
add_compile_options("-Wextra")
add_compile_options("-pedantic")
#add_compile_options("-Weffc++")
#add_compile_options("-Werror")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(clang-format)

find_package(Qt5 5.6 REQUIRED COMPONENTS Core Gui Widgets Network)
if(MINGW)
    get_target_property(_qwindows_dll Qt5::QWindowsIntegrationPlugin LOCATION)
endif()
if(APPLE)
    get_target_property(_qcocoa_dylib Qt5::QCocoaIntegrationPlugin LOCATION)
endif()

if(UNIX)
    find_package(GnuTLS)
    include_directories(SYSTEM ${GNUTLS_INCLUDE_DIR})

    find_package(OpenConnect REQUIRED)
    link_directories(${OPENCONNECT_LIBRARY_DIRS})
    include_directories(SYSTEM ${OPENCONNECT_INCLUDE_DIRS})

    if(APPLE)
        find_library(SECURITY_LIBRARY Security REQUIRED)
        if(SECURITY_LIBRARY)
            message(STATUS "Framework 'Security' found at ${SECURITY_LIBRARY}")

            link_directories(${SECURITY_LIBRARY_DIRS})
            include_directories(SYSTEM ${SECURITY_LIBRARY_INCLUDE_DIRS})
        else()
            message(FATAL_ERROR "Framework 'Security' not found!")
        endif()
        mark_as_advanced(SECURITY_LIBRARY)
    endif()
endif()

if(MINGW)
    add_subdirectory(external)
    add_subdirectory(nsis)
endif()
add_subdirectory(src)

install(FILES LICENSE.txt
    DESTINATION .
    COMPONENT App
)

include(git_revision_main)
include(build_timestamp_main)
include(ProjectPackage)
