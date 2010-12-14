cmake_minimum_required(VERSION 2.6)

project (apep)

set (_VERSION_MAJOR 0)
set (_VERSION_MINOR 1)
set (_ROOT_FOLDER ${PROJECT_SOURCE_DIR})

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE distribution)
endif(NOT CMAKE_BUILD_TYPE)

set (EXECUTABLE_OUTPUT_PATH "${_ROOT_FOLDER}/bin")
set (LIBRARY_OUTPUT_PATH "${_ROOT_FOLDER}/lib")

link_directories (/usr/local/lib)

add_subdirectory (src)
add_subdirectory (test)
