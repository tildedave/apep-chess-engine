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

add_library (apep STATIC ${SRCS})
target_link_libraries(apep cppunit)
set_target_properties(apep PROPERTIES VERSION "${_VERSION_MAJOR}.${_VERSION_MINOR}" OUTPUT_NAME ${PROJECT_NAME} CLEAN_DIRECT_OUTPUT 1)

install_targets(/lib apep)
add_executable(runapep apep.cpp)
target_link_libraries(runapep apep cppunit)
