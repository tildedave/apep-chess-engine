include_directories ("${_ROOT_FOLDER}/include")

add_executable(testapep unittestrunner.cpp)
target_link_libraries(testapep libapep cppunit)
