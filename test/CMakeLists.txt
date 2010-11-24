include_directories ("${_ROOT_FOLDER}/include")

add_executable(testapep unittestrunner.cpp)
add_executable(perftapep perft.cpp)
add_executable(tacticsapep tactics.cpp)

target_link_libraries(testapep libapep cppunit)
target_link_libraries(perftapep libapep boost_program_options-mt)
target_link_libraries(tacticsapep libapep boost_program_options-mt)
