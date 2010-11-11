add_library (apeplib apep.cpp commonboards.cpp movegen.cpp moveprocess.cpp transpositiontable.cpp board.cpp eval.cpp movelinkedlist.cpp openbook.cpp xboard.cpp common.cpp move.cpp movepriorityqueue.cpp search.cpp)

add_executable(apep apep.cpp commonboards.cpp movegen.cpp moveprocess.cpp transpositiontable.cpp board.cpp eval.cpp movelinkedlist.cpp openbook.cpp xboard.cpp common.cpp move.cpp movepriorityqueue.cpp search.cpp)
target_link_libraries(apep apeplib cppunit)
