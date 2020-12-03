add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/controller")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/dfwimpl")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/tile_editor")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/tools")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/tests")

set(SOURCE ${SOURCE} PARENT_SCOPE)
