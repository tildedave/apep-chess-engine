cmake_minimum_required(VERSION 2.6)

project (apep)

set (_VERSION_MAJOR 0)
set (_VERSION_MINOR 1)
set (_ROOT_FOLDER ${PROJECT_SOURCE_DIR})

set (EXECUTABLE_OUTPUT_PATH "${_ROOT_FOLDER}/bin")
set (LIBRARY_OUTPUT_PATH "${_ROOT_FOLDER}/lib")

add_subdirectory (src)
