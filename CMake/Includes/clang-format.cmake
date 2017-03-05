find_program(CLANG_FORMAT NAMES clang-format)
if(CLANG_FORMAT)
    file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h)
    set(PROJECT_TRDPARTY_DIR ${PROJECT_SOURCE_DIR}/include)
    foreach(SOURCE_FILE ${ALL_SOURCE_FILES})
        string(FIND ${SOURCE_FILE} ${PROJECT_TRDPARTY_DIR} PROJECT_TRDPARTY_DIR_FOUND)
        if (NOT ${PROJECT_TRDPARTY_DIR_FOUND} EQUAL -1)
            list(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCE_FILE})
        endif()
    endforeach()

    add_custom_target(clang-format
        COMMAND ${CLANG_FORMAT} -i ${ALL_SOURCE_FILES}
    )
else()
    message(STATUS " -----> clang-format nout found...")
endif()
