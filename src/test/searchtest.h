#ifndef SEARCHTEST_H_
#define SEARCHTEST_H_


#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include "../common.h"
#include "../board.h"
#include "../eval.h"
#include "../move.h"

class SearchTest : public CppUnit::TestCase {

public:

	static CppUnit::Test *suite()
	{
		CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite( "SearchTest" );
		/*
		 * */
		/*suiteOfTests->addTest( new CppUnit::TestCaller<SearchTest>( 
				"testCapture", 
				&SearchTest::testCapture ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<SearchTest>( 
				"testGuard", 
				&SearchTest::testGuard ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<SearchTest>( 
				"testBlackGuard", 
				&SearchTest::testBlackGuard ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<SearchTest>(
				"testBwtc39",
				&SearchTest::testBwtc39) );
		*/
		suiteOfTests->addTest( new CppUnit::TestCaller<SearchTest>(
				"testWcsac002",
				&SearchTest::testWcsac002) );
		/*suiteOfTests->addTest( new CppUnit::TestCaller<SearchTest>( 
				"testRookmate", 
				&SearchTest::testRookmate ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<SearchTest>( 
				"testKnightFork", 
				&SearchTest::testKnightFork ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<SearchTest>( 
				"testAided", 
				&SearchTest::testAided ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<SearchTest>( 
				"testNotAided", 
				&SearchTest::testNotAided ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<SearchTest>( 
				"testWac002", 
				&SearchTest::testWac002 ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<SearchTest>( 
				"testWac126", 
				&SearchTest::testWac126 ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<SearchTest>( 
				"testWac247", 
				&SearchTest::testWac247 ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<SearchTest>( 
				"testWcsac482", 
				&SearchTest::testWcsac482 ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<SearchTest>( 
				"testWcsac527", 
				&SearchTest::testWcsac527 ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<SearchTest>( 
				"testMorphyMate", 
				&SearchTest::testMorphyMate ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<SearchTest>( 
				"testMorphyMate2", 
				&SearchTest::testMorphyMate2 ) );*/
		/*
		*/
		return suiteOfTests;
	}


	void testCapture() {
		ChessBoard board;
		loadBoardFromFEN(&board, "8/3k4/8/8/8/8/2Kp4/8 w - - 0 1");
		cerr << board_to_string(&board) << endl;
		cerr << getMaterialScore(&board) << endl;
		int move = getMove(&board);
		CPPUNIT_ASSERT_EQUAL(std::string("Kxd2"),MoveToString(move));
	}

	void testGuard() {
		ChessBoard board;
		loadBoardFromFEN(&board, "8/8/3K1Pk2/8/8/8/8/8 w - - 0 1");
		cerr << board_to_string(&board) << endl;
		int move = getMove(&board);
		CPPUNIT_ASSERT(std::string("Ke5") == MoveToString(move) || 
				std::string("Ke6") == MoveToString(move) ||
				std::string("Ke7") == MoveToString(move));
	}

	void testBlackGuard() {
		ChessBoard board;
		loadBoardFromFEN(&board, "8/8/3Kp1k2/8/8/8/8/8 b - - 0 1");
		cerr << board_to_string(&board) << endl;
		int move = getMove(&board);
		CPPUNIT_ASSERT(std::string("Kf6") == MoveToString(move) ||
				std::string("Kf5") == MoveToString(move) || 
				std::string("Kf7") == MoveToString(move));
	}

	void testRookmate() {
		ChessBoard board;
		loadBoardFromFEN(&board, "3k4/8/3K4/8/8/8/8/7R w - - 0 1");
		cerr << board_to_string(&board) << endl;
		int move = getMove(&board);
		CPPUNIT_ASSERT_EQUAL(std::string("Rh8"),MoveToString(move));
	}

	void testKnightFork() {
		ChessBoard board;
		loadBoardFromFEN(&board, "8/1k3r2/8/8/2N6/8/8/K7 w - - 0 1");
		cerr << board_to_string(&board) << endl;
		int move = getMove(&board);
		CPPUNIT_ASSERT_EQUAL(std::string("Nd6"), MoveToString(move));
	}

	/**
	 * White should win pawn with rook + king
	 */
	void testAided() {
		std::string captureFenString = "8/3k4/3r4/8/8/8/2KpR3/8 w - - 0 1";
		ChessBoard board;
		loadBoardFromFEN(&board, captureFenString);
		cerr << board_to_string(&board) << endl;
		int cm = getMove(&board);
		CPPUNIT_ASSERT_EQUAL(std::string("Rxd2"), MoveToString(cm));
	}

	/**
	 * White should not take pawn if the king is not supporting the rook.
	 * Need the knight to guard the promotion square
	 */
	void testNotAided() {
		std::string captureFenString = "8/2nk4/3r4/8/8/8/1K1pR3/3N4 w - - 0 1";
		ChessBoard board;
		loadBoardFromFEN(&board, captureFenString);
		cerr << board_to_string(&board) << endl;
		int cm = getMove(&board);
		CPPUNIT_ASSERT(std::string("Rxd2") != MoveToString(cm));
	}

	void testMorphyMate() {
		ChessBoard board; 
		loadBoardFromFEN(&board, "5rk1/1q3ppp/pn3b2/1p3Q2/8/1P1N1P2/PBP4P/2KR4 w - - 0 1");
		cerr << board_to_string(&board) << endl;
		int move = getMove(&board);
		CPPUNIT_ASSERT_EQUAL(std::string("Qxf6"), MoveToString(move));
	}
	
	void testMorphyMate2() {
		ChessBoard board; 
		loadBoardFromFEN(&board, "11111rk1/1q111ppp/pn111Q11/1p111111/11111111/1P1N1P11/PBP1111P/11KR1111 b - - 0 0");
		cerr << board_to_string(&board) << endl;
		int move = getMove(&board);
		// try and stay alive longer!
		CPPUNIT_ASSERT_EQUAL(std::string("gxf6"), MoveToString(move));
	}

	void testWac002() {
		std::string wacString = "8/7p/5k2/5p2/p1p2P2/Pr1pPK2/1P1R3P/8 b - - 0 1";
		ChessBoard board;
		loadBoardFromFEN(&board, wacString);
		cerr << board_to_string(&board) << endl;
		int cm = getMove(&board);
		CPPUNIT_ASSERT_EQUAL(std::string("Rxb2"), MoveToString(cm));	
	}

	void testWac126() {
		std::string wacString = "r5r1/pQ5p/1qp2R2/2k1p3/4P3/2PP4/P1P3PP/6K1 w - - 0 1";
		ChessBoard board;
		loadBoardFromFEN(&board, wacString);
		cerr << board_to_string(&board) << endl;
		int cm = getMove(&board);
		CPPUNIT_ASSERT_EQUAL(std::string("Rxc6"), MoveToString(cm));	
	}
	
	void testWcsac002() {
		std::string fenString = "r1bq4/1p4kp/3p1n2/5pB1/p1pQ4/8/1P4PP/4RRK1 w - -"; 
		ChessBoard board;
		loadBoardFromFEN(&board, fenString);
		cerr << board_to_string(&board) << endl;
		int cm = getMove(&board);
		CPPUNIT_ASSERT_EQUAL(std::string("Re8"), MoveToString(cm));	
	}

	void testWac247() {
		std::string wacString = "2k1r3/1p2Bq2/p2Qp3/Pb1p1p1P/2pP1P2/2P5/2P2KP1/1R6 w - - 0 1";
		ChessBoard board;
		loadBoardFromFEN(&board, wacString);
		cerr << board_to_string(&board) << endl;
		int cm = getMove(&board);
		CPPUNIT_ASSERT_EQUAL(std::string("Rxb5"), MoveToString(cm));	
	}

	void testWcsac482() {
		std::string wacString = "r2Nqb1r/pQ1bp1pp/1pn1p3/2kp4/P1p2B2/2P5/1PP2PPP/R3KB1R w KQ - 0 1";
		ChessBoard board;
		loadBoardFromFEN(&board, wacString);
		cerr << board_to_string(&board) << endl;
		int cm = getMove(&board);
		CPPUNIT_ASSERT_EQUAL(std::string("Qxc6"), MoveToString(cm));
	}

	void testWcsac527() {
		std::string wacString = "5bk1/5pp1/3N2b1/2B4p/6P1/1R4QP/1P2rP1K/5q2 b - - 0 1";
		ChessBoard board;
		loadBoardFromFEN(&board, wacString);
		cerr << board_to_string(&board) << endl;
		int cm = getMove(&board);
		CPPUNIT_ASSERT_EQUAL(std::string("Bxd6"), MoveToString(cm));
	}
	
	void testBwtc39() {
		std::string cmString = "rn1k1r2/pp2R3/2pP1pqp/6p1/2BQ4/2P1B3/PP4P1/6K1 w - - 0 1";
		ChessBoard board;
		loadBoardFromFEN(&board, cmString);
		cerr << board_to_string(&board) << endl;
		int cm = getMove(&board);
		CPPUNIT_ASSERT_EQUAL(std::string("Qb6"), MoveToString(cm));
	}
};

#endif /*SEARCHTEST_H_*/
