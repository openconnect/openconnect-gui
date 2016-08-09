list(APPEND CMAKE_MODULE_PATH "${SOURCE_DIR}")
include(scm_version)

configure_file(
    ${SOURCE_DIR}/${PROJECT_NAME}.cpp.in
    ${PROJECT_NAME}.cpp
    @ONLY
)
