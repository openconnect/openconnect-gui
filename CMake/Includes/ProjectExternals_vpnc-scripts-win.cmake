# --------------------------------------------------------------------------------------------------
# vpnc-scripts
# --------------------------------------------------------------------------------------------------
ExternalProject_Add(vpnc-scripts-${vpnc-scripts-TAG}
    PREFIX ${CMAKE_BINARY_DIR}/external/

    UPDATE_DISCONNECTED 0
    UPDATE_COMMAND ""

    #GIT_REPOSITORY git://git.infradead.org/users/dwmw2/vpnc-scripts.git
    GIT_REPOSITORY https://github.com/horar/vpnc-scripts.git
    GIT_TAG ${vpnc-scripts-TAG}
    GIT_SHALLOW 1
    
    BUILD_IN_SOURCE 1

    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_if_different vpnc-script-win.js ${CMAKE_BINARY_DIR}/external/vpnc-script.js
)

install(FILES  ${CMAKE_BINARY_DIR}/external/vpnc-script.js
   DESTINATION .
   COMPONENT vpnc_script
)
