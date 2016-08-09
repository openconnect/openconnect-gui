list(APPEND CMAKE_MODULE_PATH "${SOURCE_DIR}")
include(scm_version)

#if(WIN32 AND MINGW)
    if(PROJ_UAC_ON)
        set(UAC_FLAG "")
    else()
        set(UAC_FLAG "//")
    endif()

    configure_file(
        ${SOURCE_DIR}/${PROJECT_NAME}.rc.in
        ${BINARY_DIR}/${PROJECT_NAME}.rc
        @ONLY
    )
#endif()
