set(${PROJECT_NAME}_HDRS
    dialog/mainwindow.h
    dialog/MyMsgBox.h
    dialog/MyCertMsgBox.h
    dialog/MyInputDialog.h
    vpninfo.h
    storage.h
    dialog/editdialog.h
    common.h
    keypair.h
    key.h
    cert.h
    dialog/logdialog.h
    gtdb.h
    cryptdata.h
)
set(${PROJECT_NAME}_SRCS
    main.cpp
    dialog/mainwindow.cpp
    dialog/MyMsgBox.cpp
    dialog/MyCertMsgBox.cpp
    dialog/MyInputDialog.cpp
    vpninfo.cpp
    storage.cpp
    dialog/editdialog.cpp
    keypair.cpp
    key.cpp
    cert.cpp
    dialog/logdialog.cpp
    gtdb.cpp
    cryptdata.cpp
)
set(${PROJECT_NAME}_FORMS
    dialog/mainwindow.ui
    dialog/editdialog.ui
    dialog/logdialog.ui
)

set(${PROJECT_NAME}_RESOURCES
    ${PROJECT_NAME}.qrc
)

configure_file(
    config.cpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.cpp
    @ONLY
)
configure_file(
    config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
    @ONLY
)
list(APPEND ${PROJECT_NAME}_SRCS
    ${CMAKE_CURRENT_BINARY_DIR}/config.cpp
    config.cpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
    config.h.in
)

if(WIN32 AND MINGW)
    configure_file(
        ${PROJECT_NAME}.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.rc
        @ONLY
    )
    list(APPEND ${PROJECT_NAME}_RESOURCES
        ${PROJECT_NAME}.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.rc
    )
endif()
if(APPLE)
    list(APPEND ${PROJECT_NAME}_RESOURCES
        images/mono_lock.icns
		${OPENCONNECT_VPNC_SCRIPT}
    )
	set_source_files_properties(
		images/mono_lock.icns
		${OPENCONNECT_VPNC_SCRIPT}
		PROPERTIES MACOSX_PACKAGE_LOCATION Resources
	)
endif()

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE
    ${${PROJECT_NAME}_SRCS}
    ${${PROJECT_NAME}_HDRS}
    ${${PROJECT_NAME}_FORMS}
    ${${PROJECT_NAME}_RESOURCES}
)
if(APPLE)
	set_target_properties(${PROJECT_NAME} PROPERTIES
		MACOSX_BUNDLE_BUNDLE_NAME ${PRODUCT_NAME_SHORT}
		#MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
		MACOSX_BUNDLE_COPYRIGHT ${PRODUCT_NAME_COPYRIGHT}
		#MACOSX_BUNDLE_GUI_IDENTIFIER
		MACOSX_BUNDLE_ICON_FILE "mono_lock.icns"
		MACOSX_BUNDLE_INFO_STRING ${PRODUCT_NAME_LONG}
		MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION}
		MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
	)
endif()

target_link_libraries (${PROJECT_NAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
)
if(WIN32 AND MINGW)
	target_link_libraries (${PROJECT_NAME}
		# system libraries
		wsock32

		# 3rd party libraries
		libgmp.dll.a
		libgnutls.dll.a
		libhogweed.dll.a
		libnettle.dll.a
		libopenconnect.dll.a
		libp11-kit.dll.a
		libstoken.dll.a
		libxml2.dll.a
		libz.dll.a
	)
else()
	target_link_libraries (${PROJECT_NAME}
		${OPENCONNECT_LIBRARIES}
		${GNUTLS_LIBRARY}
	)
endif()

# install executable
install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin COMPONENT App
	BUNDLE DESTINATION bin COMPONENT App
)

if(WIN32 AND MINGW)
	# fixup the bundle
	set(APPS "\${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.exe")
	list(APPEND libSearchDirs ${CMAKE_SOURCE_DIR}/nsis)
	set(additionalLib ${QT_LIBRARY_DIR})
	install(CODE "
	    include(BundleUtilities)
	    fixup_bundle(\"${APPS}\" \"${additionalLib}\" \"${libSearchDirs}\")
	    " COMPONENT App
	)
endif()
