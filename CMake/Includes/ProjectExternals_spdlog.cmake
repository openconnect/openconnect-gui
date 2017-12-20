# --------------------------------------------------------------------------------------------------
# spdlog
# --------------------------------------------------------------------------------------------------
ExternalProject_Add(spdlog-${spdlog-TAG}
    PREFIX ${CMAKE_BINARY_DIR}/external
    
    INSTALL_DIR ${CMAKE_BINARY_DIR}/external
    
    UPDATE_DISCONNECTED 0
    UPDATE_COMMAND ""
    
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG ${spdlog-TAG}
    GIT_SHALLOW 1

    CMAKE_ARGS "${CMAKE_ARGS};-DCMAKE_BUILD_TYPE=Release;-DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>;-DBUILD_TESTING=off;-DSPDLOG_BUILD_TESTING=off"
    CMAKE_COMMAND ${CMAKE_CROSS_COMMAND}
)

add_library(spdlog::spdlog INTERFACE IMPORTED)
set_target_properties(spdlog::spdlog PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_BINARY_DIR}/external/include"
)
