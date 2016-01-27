cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)

# 3rd party libraries
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/include)
link_directories(SYSTEM ${CMAKE_SOURCE_DIR}/lib)

add_subdirectory(src)
