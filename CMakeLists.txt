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

find_package(Qt5Core 5.5 REQUIRED)
find_package(Qt5Gui 5.5 REQUIRED)
find_package(Qt5Widgets 5.5 REQUIRED)
find_package(Qt5Network 5.5 REQUIRED)

# paths to 3rd party libraries
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/include)
link_directories(SYSTEM ${CMAKE_SOURCE_DIR}/lib)

add_subdirectory(src)
add_subdirectory(nsis)

set(CPACK_GENERATOR "NSIS")

include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PRODUCT_NAME_LONG})
set(CPACK_PACKAGE_VENDOR ${PRODUCT_NAME_COMPANY})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PRODUCT_NAME_SHORT} ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")

set(CPACK_PACKAGE_EXECUTABLES "${PROJECT_NAME}" "${PRODUCT_NAME_LONG}")
set(CPACK_CREATE_DESKTOP_LINKS  "${PROJECT_NAME}")

set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\${PROJECT_NAME}.exe")
set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}")
set(CPACK_NSIS_HELP_LINK "http:\\\\\\\\github.com\\\\openconnect\\\\openconnect-gui\\\\wiki")
set(CPACK_NSIS_URL_INFO_ABOUT "https:\\\\\\\\github.com\\\\openconnect\\\\openconnect-gui\\\\issues")
set(CPACK_NSIS_COMPRESSOR "/SOLID lzma")
set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\src\\\\${PROJECT_NAME}.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\src\\\\${PROJECT_NAME}.ico")

set(CPACK_NSIS_MUI_FINISHPAGE_RUN "${PROJECT_NAME}.exe")

set(CPACK_NSIS_MENU_LINKS
	"https://github.com/openconnect/openconnect-gui/wiki" "Homepage"
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
list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS " ExecWait '\\\"$INSTDIR\\\\Drivers\\\\vcredist_x86.exe\\\" /S /q:a /norestart'")
list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS " ExecWait '\\\"$INSTDIR\\\\Drivers\\\\tap-windows.exe\\\" /S /q:a'")
string(REPLACE ";" "\n" CPACK_NSIS_EXTRA_INSTALL_COMMANDS "${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}")

# NSIS'es Runtime-group
set(CPACK_COMPONENT_GROUP_RUNTIME_DESCRIPTION "Main application and network configuration script")
set(CPACK_COMPONENT_GROUP_DRIVERS_DESCRIPTION "Drivers")

# NSIS'es install types lists
set(CPACK_ALL_INSTALL_TYPES Full)
set(CPACK_INSTALL_TYPE_FULL_DISPLAY_NAME "Full installation")
set(CPACK_INSTALL_TYPE_APPONLY_DISPLAY_NAME "Application only")

include(CPack)
