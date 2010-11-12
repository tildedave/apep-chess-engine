include_directories ("${_ROOT_FOLDER}/include")

add_executable(testapep unittestrunner.cpp)
add_executable(perftapep perft.cpp)
target_link_libraries(testapep apep cppunit)
target_link_libraries(perftapep apep boost_program_options-mt)
