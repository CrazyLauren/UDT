
set(_OUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/example_number_one)
set(_CMAKE_CMAKE_PREFIX_PATH "${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_PACKAGE_INSTALL_DIR}")
configure_file(CMakeLists.txt.in
               ${_OUT_DIRECTORY}/CMakeLists.txt @ONLY)

install(FILES ${_OUT_DIRECTORY}/CMakeLists.txt
        api_example_customer.h
        doing_something.cpp
        main.cpp
        DESTINATION "${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/example_number_one/" COMPONENT examples
        )