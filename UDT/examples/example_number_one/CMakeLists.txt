
set(_OUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/example_number_one)
configure_file(CMakeLists.txt.in
               ${_OUT_DIRECTORY}/CMakeLists.txt @ONLY)

install(FILES ${_OUT_DIRECTORY}/CMakeLists.txt
        api_example_customer.h
        doing_something.cpp
        main.cpp
        DESTINATION ${CMAKE_INSTALL_PREFIX}/example_number_one/
        )