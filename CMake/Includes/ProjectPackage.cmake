#include(InstallRequiredSystemLibraries)

# common CPacke variables
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR ${PRODUCT_NAME_COMPANY})
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
#set(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_SOURCE_DIR}/README)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PRODUCT_NAME_LONG})
#set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PRODUCT_NAME_SHORT}")
if(APPLE)
    set(CPACK_PACKAGE_ICON ${CMAKE_CURRENT_SOURCE_DIR}/src/images/mono_lock.icns)
endif()
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt)
#set(CPACK_RESOURCE_FILE_README ...)
#set(CPACK_RESOURCE_FILE_WELCOME ...)
if(WIN32 AND MINGW)
    if(CMAKE_CROSSCOMPILING AND MINGW)
        set(CPACK_GENERATOR "ZIP")
    else()
        set(CPACK_GENERATOR "NSIS")
    endif()

    set(CPACK_NSIS_INSTALLED_ICON_NAME "${PROJECT_NAME}.exe")
    set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}")
    set(CPACK_NSIS_HELP_LINK "https:\\\\\\\\github.com\\\\openconnect\\\\openconnect-gui\\\\wiki\\\\FAQ")
    set(CPACK_NSIS_URL_INFO_ABOUT "https:\\\\\\\\openconnect.github.io\\\\openconnect-gui")
    set(CPACK_NSIS_COMPRESSOR "/SOLID lzma")
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL OFF)

    set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\src\\\\${PROJECT_NAME}.ico")
    set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\src\\\\${PROJECT_NAME}.ico")

    set(CPACK_PACKAGE_ICON ${CMAKE_CURRENT_SOURCE_DIR}\\\\nsis\\\\images\\\\header-openconnect.bmp)
    set(CPACK_NSIS_MUI_WELCOMEFINISHPAGE_BITMAP ${CMAKE_CURRENT_SOURCE_DIR}\\\\nsis\\\\images\\\\install-openconnect.bmp)
    set(CPACK_NSIS_MUI_UNWELCOMEFINISHPAGE_BITMAP ${CMAKE_CURRENT_SOURCE_DIR}\\\\nsis\\\\images\\\\install-openconnect.bmp)

    set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
    set(CPACK_NSIS_MUI_FINISHPAGE_RUN "${PROJECT_NAME}.exe")

    set(CPACK_NSIS_MENU_LINKS
        "https://openconnect.github.io/openconnect-gui/" "Homepage"
        "https://github.com/openconnect/openconnect-gui/issues" "Issues"
        "https://github.com/openconnect/openconnect-gui/wiki/FAQ" "FAQ"
    )

    # NSIS'es list of all components
    set(CPACK_COMPONENTS_ALL App App_Console vpnc_script TAP_drivers VcRedist_libs)

    set(CPACK_COMPONENT_APP_REQUIRED on)
    set(CPACK_COMPONENT_APP_DISPLAY_NAME "${PRODUCT_NAME_SHORT}")
    set(CPACK_COMPONENT_APP_DESCRIPTION "${PRODUCT_NAME_LONG} GUI")
    set(CPACK_COMPONENT_APP_GROUP "Application")
    set(CPACK_COMPONENT_APP_INSTALL_TYPES Full AppOnly Standard)

    set(CPACK_COMPONENT_VPNC_SCRIPT_REQUIRED on)
    set(CPACK_COMPONENT_VPNC_SCRIPT_DISPLAY_NAME "vpnc-script.js")
    set(CPACK_COMPONENT_VPNC_SCRIPT_DESCRIPTION "vpnc helper script to set the routing and name service up")
    set(CPACK_COMPONENT_VPNC_SCRIPT_GROUP "Application")
    set(CPACK_COMPONENT_VPNC_SCRIPT_INSTALL_TYPES Full AppOnly Standard)

    set(CPACK_COMPONENT_TAP_DRIVERS_REQUIRED off)
    set(CPACK_COMPONENT_TAP_DRIVERS_DISPLAY_NAME "TAP driver")
    set(CPACK_COMPONENT_TAP_DRIVERS_DESCRIPTION "NDIS 6 driver for Windows (in case you're installing on Windows XP, \
    disable this option and install NDIS 5 drivers manually from https://openvpn.net)")
    set(CPACK_COMPONENT_TAP_DRIVERS_GROUP "Drivers")
    set(CPACK_COMPONENT_TAP_DRIVERS_INSTALL_TYPES Full Standard)

    set(CPACK_COMPONENT_VCREDIST_LIBS_REQUIRED off)
    set(CPACK_COMPONENT_VCREDIST_LIBS_DISPLAY_NAME "vcredist")
    set(CPACK_COMPONENT_VCREDIST_LIBS_DESCRIPTION "Visual C++ Redistributable Package for Visual Studio 2015")
    set(CPACK_COMPONENT_VCREDIST_LIBS_GROUP "Drivers")
    set(CPACK_COMPONENT_VCREDIST_LIBS_INSTALL_TYPES Full Standard)

    set(CPACK_COMPONENT_APP_CONSOLE_DISABLED on)
    set(CPACK_COMPONENT_APP_CONSOLE_REQUIRED off)
    set(CPACK_COMPONENT_APP_CONSOLE_DISPLAY_NAME "${PRODUCT_NAME_SHORT}")
    set(CPACK_COMPONENT_APP_CONSOLE_DESCRIPTION "${PRODUCT_NAME_LONG} (console)")
    set(CPACK_COMPONENT_APP_CONSOLE_GROUP "Application")
    set(CPACK_COMPONENT_APP_CONSOLE_INSTALL_TYPES Full)

    # custom install command to populate vcredist & NDIS drivers
    # vcredis: http://asawicki.info/news_1597_installing_visual_c_redistributable_package_from_command_line.html
    list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS " ExecWait '\\\"$INSTDIR\\\\Drivers\\\\vcredist_x86.exe\\\" /install /quiet /norestart'")
    list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS " ExecWait '\\\"$INSTDIR\\\\Drivers\\\\tap-windows.exe\\\" /S /norestart'")
    string(REPLACE ";" "\n" CPACK_NSIS_EXTRA_INSTALL_COMMANDS "${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}")

    # NSIS'es Runtime-group
    set(CPACK_COMPONENT_GROUP_APPLICATION_DESCRIPTION "Main application and network configuration script")
    set(CPACK_COMPONENT_GROUP_APPLICATION_EXPANDED on)
    set(CPACK_COMPONENT_GROUP_DRIVERS_DESCRIPTION "Drivers")
    set(CPACK_COMPONENT_GROUP_DRIVERS_EXPANDED on)

    # NSIS'es install types lists
    set(CPACK_ALL_INSTALL_TYPES Full)
    set(CPACK_INSTALL_TYPE_FULL_DISPLAY_NAME "Full installation")
    set(CPACK_INSTALL_TYPE_APPONLY_DISPLAY_NAME "Application only")
    set(CPACK_INSTALL_TYPE_STANDARD_DISPLAY_NAME "Standard installation")

    # source code packaging
    #set(CPACK_SOURCE_PACKAGE_FILE_NAME )
    set(CPACK_SOURCE_GENERATOR "7Z")
    set(CPACK_SOURCE_IGNORE_FILES "build-release/;/\.git/;\.swp$;\.gitignore$")
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
