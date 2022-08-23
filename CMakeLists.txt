cmake_minimum_required(VERSION 3.5.1)

project(tile_editor)

option(BUILD_TESTS "Build test code" ON)
option(BUILD_DEBUG "Build a debug release" ON)
option(BUILD_APPIMAGE "Build for appimage" OFF)
option(BUILD_REGULAR "Regular build" OFF)

set(MAJOR_VERSION 1)
set(MINOR_VERSION 1)
set(PATCH_VERSION 1)

if(${CMAKE_VERSION} VERSION_LESS "3.22.0")

	add_definitions(-DMAJOR_VERSION=${MAJOR_VERSION})
	add_definitions(-DMINOR_VERSION=${MINOR_VERSION})
	add_definitions(-DPATCH_VERSION=${PATCH_VERSION})
else()

	add_compile_definitions(MAJOR_VERSION=${MAJOR_VERSION})
	add_compile_definitions(MINOR_VERSION=${MINOR_VERSION})
	add_compile_definitions(PATCH_VERSION=${PATCH_VERSION})
endif()

if(${BUILD_APPIMAGE})

	message("will build as AppImage")

	if(${CMAKE_VERSION} VERSION_LESS "3.22.0")

		add_definitions(-DAS_APPIMAGE=1)
	else()

		add_compile_definitions(AS_APPIMAGE=1)
	endif()
endif()

if(${BUILD_REGULAR})

	message("will build as regular app")

	if(${CMAKE_VERSION} VERSION_LESS "3.22.0")

		add_definitions(-DAS_REGULAR=1)
	else()

		add_compile_definitions(AS_REGULAR=1)
	endif()
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(${BUILD_DEBUG})

	message("DEBUG BUILD")
	set(CMAKE_BUILD_TYPE Debug)
	if(${CMAKE_VERSION} VERSION_LESS "3.12.0")
		add_definitions(-DMAJOR_VERSION=1)
	else()
		add_compile_definitions(LIBDANSDL2_DEBUG)
	endif()
else()
	message("RELEASE BUILD")
	set(CMAKE_BUILD_TYPE Release)
endif()

#-Werror
add_compile_options(-Wall -Wextra -Wundef -Wcast-align -Wwrite-strings -Wlogical-op -Wmissing-declarations -Wredundant-decls -Wshadow -Woverloaded-virtual -Wno-deprecated -ansi -pedantic -fno-rtti)

include_directories("${CMAKE_CURRENT_LIST_DIR}/include/tile_editor")
include_directories("${CMAKE_CURRENT_LIST_DIR}/include/dfw_impl")
include_directories("${CMAKE_CURRENT_LIST_DIR}/include/input")
include_directories("${CMAKE_CURRENT_LIST_DIR}/include/controller")
include_directories("${CMAKE_CURRENT_LIST_DIR}/include")

add_subdirectory("${PROJECT_SOURCE_DIR}/src")

#add the framework dependencies
add_library(lm SHARED IMPORTED)
add_library(tools SHARED IMPORTED)
add_library(ldtools SHARED IMPORTED)
add_library(dansdl2 SHARED IMPORTED)
add_library(dfw SHARED IMPORTED)

#If the libraries are not installed, tell them where they are.
set_target_properties(lm PROPERTIES IMPORTED_LOCATION /usr/local/lib/liblm.so)
set_target_properties(tools PROPERTIES IMPORTED_LOCATION /usr/local/lib/libtools.so)
set_target_properties(dansdl2 PROPERTIES IMPORTED_LOCATION /usr/local/lib/libdansdl2.so)
set_target_properties(ldtools PROPERTIES IMPORTED_LOCATION /usr/local/lib/libldtools.so)
set_target_properties(dfw PROPERTIES IMPORTED_LOCATION /usr/local/lib/libdfw.so)

#Virtual library so we don't have to build again and again for all tests...
add_library(applib STATIC ${SOURCE})
set_target_properties(applib PROPERTIES OUTPUT_NAME applib)


if(${BUILD_TESTS})

	add_executable(config src/tests/config/main.cpp)
	target_link_libraries(config applib tools ldtools dansdl2 dfw lm SDL2 SDL2_ttf SDL2_mixer SDL2_image GL stdc++fs)
	add_custom_command(TARGET config POST_BUILD COMMAND cp -r ../src/tests/config/data ./)

	add_executable(map src/tests/map/main.cpp)
	target_link_libraries(map applib tools ldtools dansdl2 dfw lm SDL2 SDL2_ttf SDL2_mixer SDL2_image GL stdc++fs)
	add_custom_command(TARGET map POST_BUILD COMMAND cp -r ../src/tests/map/data ./)

	add_executable(message_manager src/tests/tools/message_manager.cpp)
	target_link_libraries(message_manager applib tools ldtools dansdl2 dfw lm SDL2 SDL2_ttf SDL2_mixer SDL2_image GL stdc++fs)
endif()

#Configure the executable, we want it into the "build directory" and we want all resources copied
#to the build directory.
add_executable(tile_editor src/main.cpp)
set_target_properties(tile_editor PROPERTIES RUNTIME_OUTPUT_DIRECTORY build)
add_custom_command(TARGET tile_editor POST_BUILD COMMAND cp -r ../resources/* ./build)
target_link_libraries(tile_editor applib dfw tools dansdl2 ldtools lm SDL2 SDL2_ttf SDL2_mixer SDL2_image GL stdc++fs)

