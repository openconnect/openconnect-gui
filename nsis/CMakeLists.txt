install(FILES vpnc-script.js
	DESTINATION bin
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
    DESTINATION bin
    COMPONENT App_Console
)
