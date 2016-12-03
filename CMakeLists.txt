CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)
cmake_policy(SET CMP0022 NEW)
PROJECT(UDT C CXX)

add_subdirectory(log4cplus-1.2.x)

SET(LOG4CPLUS_LIBRARIES  log4cplus CACHE STRING "CUSTOMER_LIBRARIES" FORCE)#fixme must be filepath
SET(LOG4CPLUS_INCLUDES ${log4cplus_SOURCE_DIR}/include; 
                     ${log4cplus_BINARY_DIR}/include CACHE INTERNAL  "Include path for the header files of log4cplus" )
SET(LOG4CPLUS_FOUND true CACHE INTERNAL "" FORCE)

mark_as_advanced (
    LOG4CPLUS_LIBRARIES
    LOG4CPLUS_INCLUDES
    )

add_subdirectory(SHARE)
add_subdirectory(UDT)
