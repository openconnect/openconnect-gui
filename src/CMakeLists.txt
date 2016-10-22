
add_subdirectory(spdlog)

set(${PROJECT_NAME}_SRCS
	${PROJECT_NAME}.cpp
	${PROJECT_NAME}.h
	${PROJECT_NAME}.qrc
	cert.cpp
	cert.h
	common.h
	config.h
	config.h.in
	cryptdata.cpp
	cryptdata.h
	dialog/MyCertMsgBox.cpp
	dialog/MyCertMsgBox.h
	dialog/MyInputDialog.cpp
	dialog/MyInputDialog.h
	dialog/MyMsgBox.cpp
	dialog/MyMsgBox.h
	dialog/editdialog.cpp
	dialog/editdialog.h
	dialog/editdialog.ui
	dialog/logdialog.cpp
	dialog/logdialog.h
	dialog/logdialog.ui
	dialog/mainwindow.cpp
	dialog/mainwindow.h
	dialog/mainwindow.ui
	gtdb.cpp
	gtdb.h
	key.cpp
	key.h
	keypair.cpp
	keypair.h
	main.cpp
	server_storage.cpp
	server_storage.h
	vpninfo.cpp
	vpninfo.h

	logger.cpp
	logger.h

    FileLogger.cpp
    FileLogger.h
)
set_source_files_properties(
	${PROJECT_NAME}.cpp
	PROPERTIES
		GENERATED true
)
set_source_files_properties(
	build_timestamp.h
	PROPERTIES
		GENERATED true
		HEADER_FILE_ONLY true
)
set_source_files_properties(
   ${PROJECT_NAME}.rc
	PROPERTIES
		GENERATED true
)

configure_file(
	config.h.in
	config.h
	@ONLY
)

if(WIN32 AND MINGW)
	list(APPEND ${PROJECT_NAME}_RESOURCES
		${PROJECT_NAME}.rc.in
		${PROJECT_NAME}.rc
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

add_executable(${PROJECT_NAME}
	${${PROJECT_NAME}_SRCS}
	${${PROJECT_NAME}_RESOURCES}
)
if(WIN32 AND MINGW)
	set_target_properties(${PROJECT_NAME} PROPERTIES
		WIN32_EXECUTABLE true
	)
endif()
if(APPLE)
	set_target_properties(${PROJECT_NAME} PROPERTIES
		MACOSX_BUNDLE true
		MACOSX_BUNDLE_BUNDLE_NAME ${PRODUCT_NAME_SHORT}
		MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0"
		MACOSX_BUNDLE_COPYRIGHT ${PRODUCT_NAME_COPYRIGHT}
		MACOSX_BUNDLE_GUI_IDENTIFIER "io.github.openconnect.openconnect-gui"
		MACOSX_BUNDLE_ICON_FILE "mono_lock.icns"
		MACOSX_BUNDLE_INFO_STRING ${PRODUCT_NAME_LONG}
		MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION}
		MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
	)
endif()

target_link_libraries (${PROJECT_NAME}
    spdlog

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
endif()
if(UNIX)
	target_link_libraries(${PROJECT_NAME}
		${OPENCONNECT_LIBRARIES}
		${GNUTLS_LIBRARY}
	)
endif()

# install executable
install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION . COMPONENT App
	BUNDLE DESTINATION . COMPONENT App
	LIBRARY DESTINATION . COMPONENT App
)

# fixup the bundle
#get_target_property(QT_LIBRARY_DIR Qt5::Core LOCATION)
#get_filename_component(QT_LIBRARY_DIR ${QT_LIBRARY_DIR} PATH)
#get_filename_component(QT_LIBRARY_DIR "${QT_LIBRARY_DIR}/.." ABSOLUTE)
if(WIN32 AND MINGW)
	set(APPS "\${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.exe")
	list(APPEND libSearchDirs ${CMAKE_SOURCE_DIR}/nsis)

	# Qt Platform Plugin
	install(FILES
		${_qwindows_dll}
		DESTINATION plugins/platforms
		COMPONENT App
	)
	install(FILES
		${CMAKE_SOURCE_DIR}/nsis/qt.conf
		DESTINATION .
		COMPONENT App
	)
else()
	set(APPS "\${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app")
	list(APPEND libSearchDirs "")
endif()
set(additionalLib ${_qwindows_dll})
install(CODE "
	include(BundleUtilities)
	fixup_bundle(\"${APPS}\" \"${additionalLib}\" \"${libSearchDirs}\")
	" DESTINATION . COMPONENT App
)
