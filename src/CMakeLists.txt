file(GLOB_RECURSE SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
               commonboards.cpp 
               movegen.cpp 
               moveprocess.cpp
               transpositiontable.cpp
               board.cpp
               eval.cpp
               movelinkedlist.cpp
               openbook.cpp
               xboard.cpp
               common.cpp
               move.cpp
               movepriorityqueue.cpp
               search.cpp   
)

include_directories ("${_ROOT_FOLDER}/include")

add_library (libapep STATIC ${SRCS})
target_link_libraries(libapep cppunit log4cxx apr-1 aprutil-1)
set_target_properties(libapep PROPERTIES VERSION "${_VERSION_MAJOR}.${_VERSION_MINOR}" OUTPUT_NAME ${PROJECT_NAME} CLEAN_DIRECT_OUTPUT 1)

install_targets(/lib libapep)
add_executable(apep apep.cpp)
SET(CMAKE_BUILD_TYPE distribution)
SET(CMAKE_CXX_FLAGS_DISTRIBUTION "-O3 -Wall -pedantic -Wno-long-long")

set_target_properties(apep PROPERTIES COMPILE_FLAGS "-O3 -Wall -pedantic -Wno-long-long")

target_link_libraries(apep libapep cppunit)
