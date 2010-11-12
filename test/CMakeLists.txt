include_directories ("${_ROOT_FOLDER}/include")

add_executable(testapep testrunner.cpp)
target_link_libraries(testapep apep cppunit)
