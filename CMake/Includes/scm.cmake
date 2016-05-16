add_custom_command(
    OUTPUT scm_version.cmake
    COMMAND ${CMAKE_COMMAND}
        -D GIT_EXECUTABLE=${GIT_EXECUTABLE}
        -D GIT_ROOT_DIR=${PROJECT_SOURCE_DIR}
        -D SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
        -D BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}
        -P ${PROJECT_SOURCE_DIR}/CMake/Includes/scm-generate_version.cmake
    COMMENT "Automatic version strings assembling"
    VERBATIM
)

add_custom_command(
    OUTPUT ${PROJECT_NAME}.cpp
    DEPENDS scm_version.cmake
    COMMAND ${CMAKE_COMMAND}
        -D PROJECT_NAME=${PROJECT_NAME}
        -D SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
        -D BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}

        -D PRODUCT_NAME_SHORT=${PRODUCT_NAME_SHORT}
        -D PRODUCT_NAME_LONG=${PRODUCT_NAME_LONG}
        -D PRODUCT_NAME_COMPANY=${PRODUCT_NAME_COMPANY}
        -D PRODUCT_NAME_COMPANY_DOMAIN=${PRODUCT_NAME_COMPANY_DOMAIN}
        -D PRODUCT_NAME_COPYRIGHT=${PRODUCT_NAME_COPYRIGHT}

        -P ${PROJECT_SOURCE_DIR}/CMake/Includes/scm-generate_source.cmake
    COMMENT "Automatic source code generation"
    VERBATIM
)
#if(WIN32 AND MINGW)
    add_custom_command(
        OUTPUT ${PROJECT_NAME}.rc
        DEPENDS scm_version.cmake
        COMMAND ${CMAKE_COMMAND}
            -D PROJECT_NAME=${PROJECT_NAME}
            -D SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
            -D BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}

            -D PRODUCT_NAME_SHORT=${PRODUCT_NAME_SHORT}
            -D PRODUCT_NAME_LONG=${PRODUCT_NAME_LONG}
            -D PRODUCT_NAME_COMPANY=${PRODUCT_NAME_COMPANY}
            -D PRODUCT_NAME_COMPANY_DOMAIN=${PRODUCT_NAME_COMPANY_DOMAIN}
            -D PRODUCT_NAME_COPYRIGHT=${PRODUCT_NAME_COPYRIGHT}

            -D PROJ_UAC_ON=${PROJ_UAC_ON}

            -P ${PROJECT_SOURCE_DIR}/CMake/Includes/scm-generate_resource.cmake
        COMMENT "Automatic resource files generation"
        VERBATIM
    )
#endif()

## ???
#add_custom_target(generate_cmake ALL
#	DEPENDS ${PROJECT_NAME}.cpp ${PROJECT_NAME}.rc
#	#DEPENDS scm_version.cmake
##	DEPENDS version.cmake config.h config.cpp #openconnect-gui.rc
#	COMMENT "generate '*'"
#	VERBATIM
#)
