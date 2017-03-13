# --------------------------------------------------------------------------------------------------
# openconnect (libs, headers, etc.)
# --------------------------------------------------------------------------------------------------
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/external/openconnect-devel-${openconnect-TAG}_mingw32.zip)
    set(OPENCONNECT_DEV_URL https://github.com/horar/openconnect/releases/download/${openconnect-TAG})
else()
    message(STATUS "Using local openconnect-devel packages...")
    set(OPENCONNECT_DEV_URL ${CMAKE_SOURCE_DIR}/external)
endif()

ExternalProject_Add(openconnect-devel-${openconnect-TAG}
    PREFIX ${CMAKE_BINARY_DIR}/external
    INSTALL_DIR ${CMAKE_BINARY_DIR}/external
    
    DOWNLOAD_NO_PROGRESS 1
    
    URL ${OPENCONNECT_DEV_URL}/openconnect-devel-${openconnect-TAG}_mingw32.zip

    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
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
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/external/openconnect-${openconnect-TAG}_mingw32.zip)
    set(OPENCONNECT_URL https://github.com/horar/openconnect/releases/download/${openconnect-TAG})
else()
    message(STATUS "Using local openconnect-devel packages...")
    set(OPENCONNECT_URL ${CMAKE_SOURCE_DIR}/external)
endif()

ExternalProject_Add(openconnect-${openconnect-TAG}
    PREFIX ${CMAKE_BINARY_DIR}/external
    INSTALL_DIR ${CMAKE_BINARY_DIR}/external
    
    DOWNLOAD_NO_PROGRESS 1

    URL ${OPENCONNECT_URL}/openconnect-${openconnect-TAG}_mingw32.zip

    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
ExternalProject_Add_Step(openconnect-${openconnect-TAG} deploy_libs
    COMMAND ${CMAKE_COMMAND} -E copy_directory . <INSTALL_DIR>/lib
    COMMENT "...deploing openconnect-${openconnect-TAG} libraries"
    WORKING_DIRECTORY <SOURCE_DIR>
    DEPENDEES install
)

add_executable(openconnect::app IMPORTED)
set_property(TARGET openconnect::app PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/external/lib/openconnect.exe)

add_library(openconnect::gmp SHARED IMPORTED)
set_property(TARGET openconnect::gmp PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/external/lib/libgmp-10.dll)
set_property(TARGET openconnect::gmp PROPERTY IMPORTED_IMPLIB ${CMAKE_BINARY_DIR}/external/lib/libgmp.dll.a)

add_library(openconnect::gnutls SHARED IMPORTED)
set_property(TARGET openconnect::gnutls PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/external/lib/libgnutls-30.dll)
set_property(TARGET openconnect::gnutls PROPERTY IMPORTED_IMPLIB ${CMAKE_BINARY_DIR}/external/lib/libgnutls.dll.a)

add_library(openconnect::hogweed SHARED IMPORTED)
set_property(TARGET openconnect::hogweed PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/external/lib/libhogweed-4.dll)
set_property(TARGET openconnect::hogweed PROPERTY IMPORTED_IMPLIB ${CMAKE_BINARY_DIR}/external/lib/libhogweed.dll.a)

add_library(openconnect::nettle SHARED IMPORTED)
set_property(TARGET openconnect::nettle PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/external/lib/libnettle-6.dll)
set_property(TARGET openconnect::nettle PROPERTY IMPORTED_IMPLIB ${CMAKE_BINARY_DIR}/external/lib/libnettle.dll.a)

add_library(openconnect::openconnect SHARED IMPORTED)
set_property(TARGET openconnect::openconnect PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/external/lib/libopenconnect-5.dll)
set_property(TARGET openconnect::openconnect PROPERTY IMPORTED_IMPLIB ${CMAKE_BINARY_DIR}/external/lib/libopenconnect.dll.a)

add_library(openconnect::p11-kit SHARED IMPORTED)
set_property(TARGET openconnect::p11-kit PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/external/lib/libp11-kit-0.dll)
set_property(TARGET openconnect::p11-kit PROPERTY IMPORTED_IMPLIB ${CMAKE_BINARY_DIR}/external/lib/libp11-kit.dll.a)

add_library(openconnect::stoken SHARED IMPORTED)
set_property(TARGET openconnect::stoken PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/external/lib/libstoken-1.dll)
set_property(TARGET openconnect::stoken PROPERTY IMPORTED_IMPLIB ${CMAKE_BINARY_DIR}/external/lib/libstoken.dll.a)

add_library(openconnect::xml2 SHARED IMPORTED)
set_property(TARGET openconnect::xml2 PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/external/lib/libxml2-2.dll)
set_property(TARGET openconnect::xml2 PROPERTY IMPORTED_IMPLIB ${CMAKE_BINARY_DIR}/external/lib/libxml2.dll.a)


install(FILES  ${CMAKE_BINARY_DIR}/external/lib/openconnect.exe
    DESTINATION .
    COMPONENT App_Console
)
