install(FILES vpnc-script.js
   DESTINATION .
   COMPONENT vpnc_script
)

install(FILES tap-windows.exe
    DESTINATION Drivers
    COMPONENT TAP_drivers
)

install(FILES lib/openconnect.exe
    DESTINATION .
    COMPONENT App_Console
)
