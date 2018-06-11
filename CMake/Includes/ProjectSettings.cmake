option(PROJ_GNUTLS_DEBUG "Enable GnuTLS debug mode" OFF)
if(MINGW)
    option(PROJ_UAC_ON "Enable UAC (don't turn it off in production)" ON)

    set(DEFAULT_VPNC_SCRIPT "vpnc-script.js")
elseif(APPLE)
    set(DEFAULT_VPNC_SCRIPT "../Resources/vpnc-script")
else()
    set(DEFAULT_VPNC_SCRIPT "/etc/vpnc/vpnc-script")
endif()
option(PROJ_PKCS11 "Enable PKCS11" ON)
option(PROJ_INI_SETTINGS "Store the settings in INI files." OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options("-Wall")
add_compile_options("-Wextra")
add_compile_options("-Wpedantic")
#add_compile_options("-Weffc++")
#add_compile_options("-Werror")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
