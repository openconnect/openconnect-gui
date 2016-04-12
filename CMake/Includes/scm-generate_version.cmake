#create a pretty commit id using git
#uses 'git describe --tags --dirty', so tags are required in the repo
#create a tag with 'git tag <name>' and 'git push --tags'

execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags --dirty
    WORKING_DIRECTORY ${GIT_ROOT_DIR}
    RESULT_VARIABLE res_var
    OUTPUT_VARIABLE GIT_COM_ID
)
string(REPLACE "\n" "" GIT_COMMIT_ID ${GIT_COM_ID})

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

set(VERSION "${GIT_COMMIT_ID}")

configure_file(
    ${SOURCE_DIR}/scm_version.cmake.in
    ${BINARY_DIR}/scm_version.cmake
    @ONLY
)
