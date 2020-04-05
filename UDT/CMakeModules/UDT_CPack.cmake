if(CPACK_GENERATOR STREQUAL "NSIS")
    list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS
         "   !include \\\"winmessages.nsh\\\"
         ; HKLM (all users) vs HKCU (current user) defines
         !define env_hklm 'HKLM \\\"SYSTEM\\\\CurrentControlSet\\\\Control\\\\Session Manager\\\\Environment\\\"'
         ; set variable for local machine
         WriteRegExpandStr \\\${env_hklm} UDT_CUSTOMER_CONFIG_PATH \\\$INSTDIR\\\\${CMAKE_INSTALL_BINDIR}\\\\default_customer_config.json
         WriteRegExpandStr \\\${env_hklm} UDT_CUSTOMER_MODULE_PATH \\\$INSTDIR\\\\${CMAKE_INSTALL_BINDIR}"
         )

        list(APPEND CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS
             "DeleteRegValue \\\${env_hklm} UDT_CUSTOMER_CONFIG_PATH
              DeleteRegValue \\\${env_hklm} UDT_CUSTOMER_MODULE_PATH
            ")
endif()
