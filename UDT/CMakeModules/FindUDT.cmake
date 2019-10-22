# - Find UDT
# Find the UDT headers
#
#  UDT_FOUND      = Set to true, if all components of UDT have been found.
#  UDT_INCLUDES   = Include path for the header files of UDT
#  UDT_LIBRARIES  = Link these to use UDT

set(UDT_FOUND
	false
	CACHE BOOL
	""
	FORCE
	)
if (UDT_ROOT)
    set(customer_ROOT ${UDT_ROOT})
    set(udt_share_ROOT ${UDT_ROOT})
endif ()

find_package(customer REQUIRED)

if (NOT customer_FOUND)
    message(FATAL_ERROR "Failed to find customer library. set UDT_ROOT direcoty")
    return()
endif ()

find_package(udt_share REQUIRED)

if (NOT udt_share_FOUND)
    message(FATAL_ERROR "Failed to find udt_share. set UDT_ROOT direcoty")
    return()
endif ()

find_package(SHARE REQUIRED)

if (NOT SHARE_FOUND)
    message(FATAL_ERROR "Failed to find share. set SHARE_ROOT direcoty")
    return()
endif ()

set(UDT_FOUND
    true
    CACHE BOOL
    ""
	FORCE
    )
