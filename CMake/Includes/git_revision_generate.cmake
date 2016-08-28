#create a pretty commit id using git
#uses 'git describe --tags', so tags are required in the repo
#create a tag with 'git tag <name>' and 'git push --tags'

if(IS_DIRECTORY ${GIT_ROOT_DIR}/.git)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --dirty
        WORKING_DIRECTORY ${GIT_ROOT_DIR}
        RESULT_VARIABLE res_var
        OUTPUT_VARIABLE GIT_COM_ID
    )
    if(NOT ${res_var} EQUAL 0)
        set(GIT_COMMIT_ID "?.?.?-unknown")
        message(WARNING "Git failed (not a repo, or no tags). Build will not contain git revision info.")
    endif()
    string(REGEX REPLACE "\n$" "" GIT_COMMIT_ID ${GIT_COM_ID})
    string(REGEX REPLACE "^v" "" GIT_COMMIT_ID ${GIT_COMMIT_ID})

    # check number of digits in version string
    string(REPLACE "." ";" GIT_COMMIT_ID_VLIST ${GIT_COMMIT_ID})
    list(LENGTH GIT_COMMIT_ID_VLIST GIT_COMMIT_ID_VLIST_COUNT)

    # no.: major
    string(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" VERSION_MAJOR "${GIT_COMMIT_ID}")
    # no.: minor
    string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${GIT_COMMIT_ID}")

    if(${GIT_COMMIT_ID_VLIST_COUNT} STREQUAL "2")
        # no. patch
        set(VERSION_PATCH "0")
        # SHA1 string + git 'dirty' flag
        string(REGEX REPLACE "^v[0-9]+\\.[0-9]+(.*)" "\\1" VERSION_SHA1 "${GIT_COMMIT_ID}")
    else()
        # no. patch
        string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${GIT_COMMIT_ID}")
        # SHA1 string + git 'dirty' flag
        string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.[0-9]+(.*)" "\\1" VERSION_SHA1 "${GIT_COMMIT_ID}")
    endif()

    set(PROJECT_VERSION "${GIT_COMMIT_ID}")
    #string(APPEND PROJECT_VERSION " (git)")
    message(STATUS "Version: ${PROJECT_VERSION} [git]")
else()
    message(STATUS "Version: ${PROJECT_VERSION} [cmake]")
endif()

if(NOT APPLE)
    if(PROJ_UAC_ON)
        set(UAC_FLAG "")
    else()
        set(UAC_FLAG "//")
    endif()

    message(STATUS "Processing resource file...")
    file(READ ${INPUT_DIR}/${PROJECT_NAME}.rc.in rc_temporary)
    string(CONFIGURE ${rc_temporary} rc_updated)
    file(WRITE ${OUTPUT_DIR}/${PROJECT_NAME}.rc.tmp ${rc_updated})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${OUTPUT_DIR}/${PROJECT_NAME}.rc.tmp ${OUTPUT_DIR}/${PROJECT_NAME}.rc
    )
else()
    message(FATAL_ERROR " === ERROR ===")
endif()

message(STATUS "Processing app info file...")
file(READ ${INPUT_DIR}/${PROJECT_NAME}.cpp.in cpp_temporary)
string(CONFIGURE "${cpp_temporary}" cpp_updated @ONLY)
file(WRITE ${OUTPUT_DIR}/${PROJECT_NAME}.cpp.tmp "${cpp_updated}")
execute_process(
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	${OUTPUT_DIR}/${PROJECT_NAME}.cpp.tmp ${OUTPUT_DIR}/${PROJECT_NAME}.cpp
)
