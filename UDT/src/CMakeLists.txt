
SET(PATH_TO_REVISION_IN ${CMAKE_CURRENT_SOURCE_DIR}/revision.c.in CACHE INTERNAL  "" FORCE)

add_subdirectory(udt_share)
add_subdirectory(kernel)
add_subdirectory(customer)