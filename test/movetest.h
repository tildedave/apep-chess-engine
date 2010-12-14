#pragma once

#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include "common.h"
#include "board.h"
#include "eval.h"
#include "move.h"
#include "moveprocess.h"
#include "movegen.h"
#include "xboard.h"

class MoveTest : public CppUnit::TestCase {

public:

	static CppUnit::Test *suite()
	{
		CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite( "MoveTest" );
		suiteOfTests->addTest( new CppUnit::TestCaller<MoveTest>( 
				"testProcessesHashKeyOnMove", 
				&MoveTest::testProcessesHashKeyOnMove ) );
		return suiteOfTests;
	}

	void testProcessesHashKeyOnMove() {
	  ChessBoard board;
	  loadBoardFromFEN(&board, "r11nRrk1/1p1R11pp/11p11111/p1111111/1PP111N1/11111111/P1111PPP/111111K1 w - - 0 27");

	  processMove(&board, CoordStringToMove(&board, "d7e7"));
	  processMove(&board, CoordStringToMove(&board, "f8f7"));
	  processMove(&board, CoordStringToMove(&board, "e7d7"));
	  processMove(&board, CoordStringToMove(&board, "f7f8"));

	  CPPUNIT_ASSERT_EQUAL(board.boardHashes[board.moveIndex], board.boardHashes[board.moveIndex - 4]);
	}
};








