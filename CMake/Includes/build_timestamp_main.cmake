# Generate build timestamp string

add_custom_target(build_timestamp ALL)
add_custom_command(
    TARGET build_timestamp
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src
    COMMAND ${CMAKE_COMMAND}
        -DINPUT_DIR=${CMAKE_CURRENT_SOURCE_DIR}/src
        -DOUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}/src
        
        -DBUILD_TYPE=${CMAKE_BUILD_TYPE}
        
        -P ${CMAKE_SOURCE_DIR}/CMake/Includes/build_timestamp_generate.cmake
        COMMENT "Generating build timestamp...." VERBATIM
)

set_source_files_properties(
    ${CMAKE_CURRENT_BINARY_DIR}/src/timestamp.cpp PROPERTIES
    GENERATED TRUE
#   HEADER_FILE_ONLY TRUE
)

# explicitly say that the executable depends on the build_timestamp
add_dependencies(${PROJECT_NAME} build_timestamp)
