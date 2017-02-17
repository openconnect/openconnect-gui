install(FILES vpnc-script.js
   DESTINATION .
   COMPONENT vpnc_script
)

install(FILES lib/openconnect.exe
    DESTINATION .
    COMPONENT App_Console
)

add_definitions(-DUNICODE)
add_library(qtsingleapplication STATIC
    ${CMAKE_BINARY_DIR}/external/src/qt-solutions/qtsingleapplication/src/qtsingleapplication.cpp
    ${CMAKE_BINARY_DIR}/external/src/qt-solutions/qtsingleapplication/src/qtsingleapplication.h
    ${CMAKE_BINARY_DIR}/external/src/qt-solutions/qtsingleapplication/src/qtlocalpeer.cpp
    ${CMAKE_BINARY_DIR}/external/src/qt-solutions/qtsingleapplication/src/qtlocalpeer.h
)
target_link_libraries(qtsingleapplication
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
)
