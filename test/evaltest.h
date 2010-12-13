#pragma once

#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include "common.h"
#include "eval.hpp"

class EvalTest : public CppUnit::TestCase {

 public:

	static CppUnit::Test *suite()
	{
		CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite( "EvalTest" );
		suiteOfTests->addTest( new CppUnit::TestCaller<EvalTest>( 
				"testPassedPawnScoreWhiteSinglePawn", 
				&EvalTest::testPassedPawnScoreWhiteSinglePawn ) );

		suiteOfTests->addTest( new CppUnit::TestCaller<EvalTest>( 
				"testPassedPawnScoreNoPawns", 
				&EvalTest::testPassedPawnScoreWhiteNoPawns ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<EvalTest>( 
				"testPassedPawnScoreBlackSinglePawn", 
				&EvalTest::testPassedPawnScoreBlackSinglePawn ) );

		suiteOfTests->addTest( new CppUnit::TestCaller<EvalTest>( 
				"testPassedPawnScoreNoPawns", 
				&EvalTest::testPassedPawnScoreBlackNoPawns ) );

		return suiteOfTests;
	}

	int passedPawnTest(ChessBoard* board, bool white) {
	  board->eval = new EvalInfo();
	  preprocessEvalInformation(board);
	  int passedPawnScore = getPassedPawnScore(board, white);
	  delete board->eval;

	  return passedPawnScore;
	}

	void testPassedPawnScoreWhiteSinglePawn() {
	    ChessBoard board;
	    std::string startingFEN("8/8/8/8/4P3/8/8/8 w KQkq - 0 1");
	    loadBoardFromFEN(&board, startingFEN);

	    int passedPawnScore = passedPawnTest(&board, true);
	    CPPUNIT_ASSERT_EQUAL(passedPawnScore > 0, true);
	}

	void testPassedPawnScoreWhiteNoPawns() {
	    ChessBoard board;
	    std::string startingFEN("8/8/8/8/4p3/8/8/8 w KQkq - 0 1");
	    loadBoardFromFEN(&board, startingFEN);

	    int passedPawnScore = passedPawnTest(&board, true);
	    CPPUNIT_ASSERT_EQUAL(passedPawnScore == 0, true);
	}

	void testPassedPawnScoreBlackSinglePawn() {
	    ChessBoard board;
	    std::string startingFEN("8/8/8/8/4p3/8/8/8 w KQkq - 0 1");
	    loadBoardFromFEN(&board, startingFEN);

	    int passedPawnScore = passedPawnTest(&board, false);
	    CPPUNIT_ASSERT_EQUAL(passedPawnScore > 0, true);
	}

	void testPassedPawnScoreBlackNoPawns() {
	    ChessBoard board;
	    std::string startingFEN("8/8/8/8/4P3/8/8/8 w KQkq - 0 1");
	    loadBoardFromFEN(&board, startingFEN);

	    int passedPawnScore = passedPawnTest(&board, false);
	    CPPUNIT_ASSERT_EQUAL(passedPawnScore == 0, true);
	}
};
