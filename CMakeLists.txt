cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(openconnect-gui
    LANGUAGES CXX
)

set(PRODUCT_NAME_SHORT "OpenConnect")
set(PRODUCT_NAME_LONG "OpenConnect VPN client")
set(PRODUCT_NAME_COMPANY "Red Hat")
set(PRODUCT_NAME_COMPANY_DOMAIN "redhat.com")
set(PRODUCT_NAME_COPYRIGHT "Copyright (c) 2015-2016 RedHat")

option(PROJ_GNUTLS_DEBUG "Enable GnuTLS debug mode" OFF)
if(WIN32 AND MINGW)
    option(PROJ_UAC_ON "Enable UAC (don't turn it off in production)" ON)

    set(DEFAULT_VPNC_SCRIPT "vpnc-script.js")
else(APPLE)
    set(DEFAULT_VPNC_SCRIPT "../Resources/vpnc-script")
else()
    set(DEFAULT_VPNC_SCRIPT "/etc/vpnc/vpnc-script")
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options("-Wall")
#add_compile_options("-Wextra")
add_compile_options("-pedantic")
#add_compile_options("-Weffc++")
#add_compile_options("-Werror")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/Modules/")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/Includes/")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}/src/")

find_package(Qt5Core 5.6 REQUIRED)
find_package(Qt5Gui 5.6 REQUIRED)
find_package(Qt5Widgets 5.6 REQUIRED)
find_package(Qt5Network 5.6 REQUIRED)
if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/.git)
	find_package(Git 2.0 REQUIRED)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# paths to 3rd party libraries
if(WIN32 AND MINGW)
    include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/include)
    link_directories(SYSTEM ${CMAKE_SOURCE_DIR}/lib)
else()
    find_package(GnuTLS)
    include_directories(SYSTEM ${GNUTLS_INCLUDE_DIR})

    find_package(OpenConnect REQUIRED)
    link_directories(${OPENCONNECT_LIBRARY_DIRS})
    include_directories(SYSTEM ${OPENCONNECT_INCLUDE_DIRS})
endif()

add_subdirectory(src)

if(EXISTS ${CMAKE_BINARY_DIR}/src/scm_version.cmake)
	message(STATUS "Setting package...")
	include(${CMAKE_BINARY_DIR}/src/scm_version.cmake)
	
	message(STATUS "Version: ${PROJECT_VERSION}")

	if(WIN32 AND MINGW)
		add_subdirectory(nsis)
	endif()

	include(InstallRequiredSystemLibraries)

	# common CPacke variables
	set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
	set(CPACK_PACKAGE_VENDOR ${PRODUCT_NAME_COMPANY})
	set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
	set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
	set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
	#set(CPACK_PACKAGE_DESCRIPTION_FILE  ${CMAKE_SOURCE_DIR}/README)
	set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PRODUCT_NAME_LONG})
	#set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME})
	set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PRODUCT_NAME_SHORT} ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
	if(APPLE)
		set(CPACK_PACKAGE_ICON ${CMAKE_CURRENT_SOURCE_DIR}/src/images/mono_lock.icns)
	endif()
	set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/COPYING)
	#set(CPACK_RESOURCE_FILE_README ...)
	#set(CPACK_RESOURCE_FILE_WELCOME ...)
	if(WIN32 AND MINGW)
		set(CPACK_GENERATOR "NSIS")

		set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\${PROJECT_NAME}.exe")
		set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}")
		set(CPACK_NSIS_HELP_LINK "https:\\\\\\\\github.com\\\\openconnect\\\\openconnect-gui\\\\issues")
		set(CPACK_NSIS_URL_INFO_ABOUT "https:\\\\\\\\openconnect.github.io\\\\openconnect-gui")
		set(CPACK_NSIS_COMPRESSOR "/SOLID lzma")
		set(CPACK_NSIS_MODIFY_PATH ON)
		set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

		set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\src\\\\${PROJECT_NAME}.ico")
		set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\src\\\\${PROJECT_NAME}.ico")

		set(CPACK_NSIS_MUI_FINISHPAGE_RUN "${PROJECT_NAME}.exe")

		set(CPACK_NSIS_MENU_LINKS
			"https://openconnect.github.io/openconnect-gui/" "Homepage"
		)

		# NSIS'es list of all components
		set(CPACK_COMPONENTS_ALL App vpnc_script TAP_drivers VcRedist_drivers)

		set(CPACK_COMPONENT_APP_REQUIRED on)
		set(CPACK_COMPONENT_APP_DISPLAY_NAME "${PRODUCT_NAME_SHORT}")
		set(CPACK_COMPONENT_APP_DESCRIPTION "${PRODUCT_NAME_LONG}")
		set(CPACK_COMPONENT_APP_GROUP "Runtime")
		set(CPACK_COMPONENT_APP_INSTALL_TYPES Full AppOnly)

		set(CPACK_COMPONENT_VPNC_SCRIPT_REQUIRED on)
		set(CPACK_COMPONENT_VPNC_SCRIPT_DISPLAY_NAME "vpnc-script.js")
		set(CPACK_COMPONENT_VPNC_SCRIPT_DESCRIPTION "vpnc helper script to set the routing and name service up")
		set(CPACK_COMPONENT_VPNC_SCRIPT_GROUP "Runtime")
		set(CPACK_COMPONENT_VPNC_SCRIPT_INSTALL_TYPES Full AppOnly)

		set(CPACK_COMPONENT_TAP_DRIVERS_DISPLAY_NAME "TAP driver (NDIS 6)")
		set(CPACK_COMPONENT_TAP_DRIVERS_DESCRIPTION "NDIS 6 driver for Windows (in case you're installing on Windows XP, \
		disable this option and install NDIS 5 drivers manually from https://openvpn.net)")
		set(CPACK_COMPONENT_TAP_DRIVERS_GROUP "Drivers")
		set(CPACK_COMPONENT_TAP_DRIVERS_INSTALL_TYPES Full)

		set(CPACK_COMPONENT_VCREDIST_DRIVERS_DISPLAY_NAME "vcredist for msvc2013 (x86)")
		set(CPACK_COMPONENT_VCREDIST_DRIVERS_DESCRIPTION "Visual C++ Redistributable Packages for Visual Studio 2013")
		set(CPACK_COMPONENT_VCREDIST_DRIVERS_GROUP "Drivers")
		set(CPACK_COMPONENT_VCREDIST_DRIVERS_INSTALL_TYPES Full)

		# custom install command to populate vcredist & NDIS drivers
		# vcredis: http://asawicki.info/news_1597_installing_visual_c_redistributable_package_from_command_line.html
		list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS " ExecWait '\\\"$INSTDIR\\\\Drivers\\\\vcredist_x86.exe\\\" /install /quiet /norestart'")
		list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS " ExecWait '\\\"$INSTDIR\\\\Drivers\\\\tap-windows.exe\\\" /S /norestart'")
		string(REPLACE ";" "\n" CPACK_NSIS_EXTRA_INSTALL_COMMANDS "${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}")

		# NSIS'es Runtime-group
		set(CPACK_COMPONENT_GROUP_RUNTIME_DESCRIPTION "Main application and network configuration script")
		set(CPACK_COMPONENT_GROUP_RUNTIME_EXPANDED on)
		set(CPACK_COMPONENT_GROUP_DRIVERS_DESCRIPTION "Drivers")
		set(CPACK_COMPONENT_GROUP_DRIVERS_EXPANDED on)

		# NSIS'es install types lists
		set(CPACK_ALL_INSTALL_TYPES Full)
		set(CPACK_INSTALL_TYPE_FULL_DISPLAY_NAME "Full installation")
		set(CPACK_INSTALL_TYPE_APPONLY_DISPLAY_NAME "Application only")
	else()
		set(CPACK_GENERATOR "DragNDrop")

		#set(CPACK_DMG_VOLUME_NAME ...)
		#set(CPACK_DMG_FORMAT ...)
		#set(CPACK_DMG_DS_STORE ...)
		#set(CPACK_DMG_DS_STORE_SETUP_SCRIPT ...)
		#set(CPACK_DMG_BACKGROUND_IMAGE ...)
		#set(CPACK_DMG_SLA_DIR ...)
		#set(CPACK_DMG_SLA_LANGUAGES ...)
	endif()
	set(CPACK_PACKAGE_EXECUTABLES "${PROJECT_NAME}" "${PRODUCT_NAME_LONG}")
	set(CPACK_CREATE_DESKTOP_LINKS  "${PROJECT_NAME}")

	include(CPack)
endif()
