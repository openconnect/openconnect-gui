install(FILES vpnc-script.js
	DESTINATION .
	COMPONENT vpnc_script
)

install(FILES tap-windows.exe
	DESTINATION Drivers
	COMPONENT TAP_drivers
)

install(FILES vcredist_x86.exe
	DESTINATION Drivers
	COMPONENT VcRedist_libs
)

install(FILES openconnect.exe
    DESTINATION .
    COMPONENT App_Console
)
