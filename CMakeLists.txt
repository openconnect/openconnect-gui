cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(openconnect-gui
    VERSION 1.3.0.0
    LANGUAGES CXX
)

set(PRODUCT_NAME_SHORT "OpenConnect")
set(PRODUCT_NAME_LONG "OpenConnect VPN client")
set(PRODUCT_NAME_COMPANY "Red Hat")
set(PRODUCT_NAME_COMPANY_DOMAIN "redhat.com")
set(PRODUCT_NAME_COPYRIGHT "Copyright (c) 2015-2016 RedHat")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)

# paths to 3rd party libraries
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/include)
link_directories(SYSTEM ${CMAKE_SOURCE_DIR}/lib)

add_subdirectory(src)
