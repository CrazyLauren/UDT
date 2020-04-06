if(CPACK_GENERATOR STREQUAL "NSIS")
    list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS
         "
         !include \\\"winmessages.nsh\\\"
         ; HKLM (all users) vs HKCU (current user) defines
         !define env_hklm 'HKLM \\\"SYSTEM\\\\CurrentControlSet\\\\Control\\\\Session Manager\\\\Environment\\\"'
         ; set variable for local machine
         WriteRegExpandStr \\\${env_hklm} UDT_CUSTOMER_CONFIG_PATH \\\$INSTDIR\\\\${CMAKE_INSTALL_BINDIR}\\\\default_customer_config.json
         WriteRegExpandStr \\\${env_hklm} UDT_CUSTOMER_MODULE_PATH \\\$INSTDIR\\\\${CMAKE_INSTALL_BINDIR}
         SendMessage \\\${HWND_BROADCAST} \\\${WM_WININICHANGE} 0 \\\"STR:Environment\\\" /TIMEOUT=5000
         CreateShortCut '$INSTDIR\\\\${CMAKE_INSTALL_BINDIR}\\\\${CMAKE_PACKAGE_INSTALL_DIR}.lnk' '$INSTDIR\\\\${CMAKE_PACKAGE_INSTALL_DIR}\\\\'
        "
         )

        list(APPEND CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS
             "
                 DeleteRegValue \\\${env_hklm} UDT_CUSTOMER_MODULE_PATH
                 DeleteRegValue \\\${env_hklm} UDT_CUSTOMER_CONFIG_PATH
                 SendMessage \\\${HWND_BROADCAST} \\\${WM_WININICHANGE} 0 \\\"STR:Environment\\\" /TIMEOUT=5000
            ")

    list(APPEND CPACK_NSIS_CREATE_ICONS_EXTRA
         "\n
          SetOutPath \\\"$INSTDIR\\\\${CMAKE_INSTALL_BINDIR}\\\"
         CreateShortCut '$DESKTOP\\\\Kernel.lnk' '$INSTDIR\\\\${CMAKE_INSTALL_BINDIR}\\\\Kernel.exe'
        ")
    list(APPEND CPACK_NSIS_DELETE_ICONS_EXTRA
         "\n Delete '$DESKTOP\\\\Kernel.lnk'")

    list(APPEND CPACK_NSIS_CREATE_ICONS_EXTRA
         "\n CreateShortCut '$DESKTOP\\\\example_number_one.lnk' '$INSTDIR\\\\${CMAKE_INSTALL_DATAROOTDIR}\\\\${PROJECT_NAME}\\\\example_number_one'")
    list(APPEND CPACK_NSIS_DELETE_ICONS_EXTRA
         "\n Delete '$DESKTOP\\\\example_number_one.lnk'")
endif()
