# Generate version string via Git description

if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/.git)
    find_package(Git 2.7 REQUIRED)
endif()

add_custom_target(git_revision ALL)
add_custom_command(
    TARGET git_revision
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src

    COMMAND ${CMAKE_COMMAND}
        -DGIT_ROOT_DIR=${CMAKE_SOURCE_DIR}
        -DGIT_EXECUTABLE=${GIT_EXECUTABLE}
        
        -DINPUT_DIR=${CMAKE_CURRENT_SOURCE_DIR}/src
        -DOUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}/src
        
        -DPROJECT_NAME=${PROJECT_NAME}
        -DPROJECT_VERSION=${PROJECT_VERSION}
        -DPROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        -DPROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR}
        -DPROJECT_VERSION_PATCH=${PROJECT_VERSION_PATCH}
        -DPROJECT_VERSION_TWEAK=${PROJECT_VERSION_TWEAK}

        -DPRODUCT_NAME_COMPANY=${PRODUCT_NAME_COMPANY}
        -DPRODUCT_NAME_SHORT=${PRODUCT_NAME_SHORT}
        -DPRODUCT_NAME_LONG=${PRODUCT_NAME_LONG}
        -DPRODUCT_NAME_COPYRIGHT_SHORT=${PRODUCT_NAME_COPYRIGHT_SHORT}
        -DPRODUCT_NAME_COPYRIGHT_FULL=${PRODUCT_NAME_COPYRIGHT_FULL}
        -DPRODUCT_NAME_COMPANY_DOMAIN=${PRODUCT_NAME_COMPANY_DOMAIN}

        -DPROJ_UAC_ON=${PROJ_UAC_ON}

        -DAPPLE=${APPLE}
        -P ${CMAKE_SOURCE_DIR}/CMake/Includes/git_revision_generate.cmake
        COMMENT "Analyzing Git revision/tag changes..." VERBATIM
)

# explicitly say that the executable depends on the git_revision
add_dependencies(${PROJECT_NAME} git_revision)
