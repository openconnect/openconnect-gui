cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(openconnect-gui)

set(${PROJECT_NAME}_HDRS
    mainwindow.h
    vpninfo.h
    storage.h
    editdialog.h
    common.h
    keypair.h
    key.h
    cert.h
    logdialog.h
    gtdb.h
    dialogs.h
    cryptdata.h
)
set(${PROJECT_NAME}_SRCS
    main.cpp
    mainwindow.cpp
    vpninfo.cpp
    storage.cpp
    editdialog.cpp
    keypair.cpp
    key.cpp
    cert.cpp
    logdialog.cpp
    gtdb.cpp
    cryptdata.cpp
)
set(${PROJECT_NAME}_FORMS
    mainwindow.ui
    editdialog.ui
    logdialog.ui
)
set(${PROJECT_NAME}_RESOURCES
    resources.qrc
)
if(WIN32 AND MINGW)
	list(APPEND ${PROJECT_NAME}_RESOURCES openconnect-gui.rc logo.ico)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)

include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/include)
link_directories(SYSTEM ${CMAKE_SOURCE_DIR}/lib)

add_executable(${PROJECT_NAME} WIN32
	${${PROJECT_NAME}_SRCS}
	${${PROJECT_NAME}_HDRS}
	${${PROJECT_NAME}_FORMS}
	${${PROJECT_NAME}_RESOURCES}
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 98)

target_link_libraries (${PROJECT_NAME}
	Qt5::Core
	Qt5::Gui
	Qt5::Widgets
	Qt5::Network

	# system libraries
	wsock32

	# 3rd party libraries
	libgmp.dll.a
	libgnutls-xssl.dll.a
	libgnutls.dll.a
	libhogweed.dll.a
	libnettle.dll.a
	libopenconnect.dll.a
	libp11-kit.dll.a
	libstoken.dll.a
	libxml2.dll.a
	libz.dll.a
)
