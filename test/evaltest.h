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
		suiteOfTests->addTest( new CppUnit::TestCaller<EvalTest>( 
				"testPassedPawnScoreWhiteDoubledPassedPawns", 
				&EvalTest::testPassedPawnScoreWhiteDoubledPassedPawns ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<EvalTest>( 
				"testPassedPawnScoreBlackDoubledPassedPawns", 
				&EvalTest::testPassedPawnScoreBlackDoubledPassedPawns ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<EvalTest>(
                                "testDoubledPawnPenaltyWhiteSinglePawn",
				&EvalTest::testDoubledPawnPenaltyWhiteSinglePawn ) );

		return suiteOfTests;
	}

	void positionTest(ChessBoard* board) {
	  board->eval = new EvalInfo();
	  board->gamePhase = PHASE_MIDDLEGAME; // take us out of book

	  preprocessEvalInformation(board);
	}

	int passedPawnTest(ChessBoard* board, bool white) {
	  positionTest(board);
	  int passedPawnScore = getPassedPawnScore(board, white);
	  delete board->eval;

	  return passedPawnScore;
	}

	int doubledPawnTest(ChessBoard* board, bool white) {
	  positionTest(board);
	  int doubledPawnScore = getDoubledPawnPenalty(board, white);
	  delete board->eval;

	  return doubledPawnScore;
	}

	void testPassedPawnScoreWhiteSinglePawn() {
	    ChessBoard board;
	    std::string startingFEN("8/8/8/8/8/4P3/8/8 w KQkq - 0 1");
	    loadBoardFromFEN(&board, startingFEN);

	    int passedPawnScore = passedPawnTest(&board, true);
	    CPPUNIT_ASSERT_EQUAL(EvalParameters::passedPawnBonus, passedPawnScore);
	}

	void testPassedPawnScoreWhiteNoPawns() {
	    ChessBoard board;
	    std::string startingFEN("8/8/8/8/4p3/8/8/8 w KQkq - 0 1");
	    loadBoardFromFEN(&board, startingFEN);

	    int passedPawnScore = passedPawnTest(&board, true);
	    CPPUNIT_ASSERT_EQUAL(0, passedPawnScore);
	}

	void testPassedPawnScoreBlackSinglePawn() {
	    ChessBoard board;
	    std::string startingFEN("8/8/8/4p3/8/8/8/8 w KQkq - 0 1");
	    loadBoardFromFEN(&board, startingFEN);

	    int passedPawnScore = passedPawnTest(&board, false);
	    CPPUNIT_ASSERT_EQUAL(EvalParameters::passedPawnBonus, passedPawnScore);
	}

	void testPassedPawnScoreBlackNoPawns() {
	    ChessBoard board;
	    std::string startingFEN("8/8/8/8/4P3/8/8/8 w KQkq - 0 1");
	    loadBoardFromFEN(&board, startingFEN);

	    int passedPawnScore = passedPawnTest(&board, false);
	    CPPUNIT_ASSERT_EQUAL(0, passedPawnScore);
	}

	void testPassedPawnScoreWhiteDoubledPassedPawns() {
	    ChessBoard board;
	    std::string startingFEN("8/8/8/8/8/4P3/4P3/8 w KQkq - 0 1");
	    loadBoardFromFEN(&board, startingFEN);

	    int passedPawnScore = passedPawnTest(&board, true);
	    CPPUNIT_ASSERT_EQUAL(EvalParameters::passedPawnBonus, passedPawnScore);
	}

	void testPassedPawnScoreBlackDoubledPassedPawns() {
	    ChessBoard board;
	    std::string startingFEN("8/4p3/4p3/8/8/8/8/8 w KQkq - 0 1");
	    loadBoardFromFEN(&board, startingFEN);

	    int passedPawnScore = passedPawnTest(&board, false);
	    CPPUNIT_ASSERT_EQUAL(EvalParameters::passedPawnBonus, passedPawnScore);
	}

	void testDoubledPawnPenaltyWhiteSinglePawn() {
	    ChessBoard board;
	    std::string startingFEN("8/4P3/4P3/8/8/8/8/8 w KQkq - 0 1");
	    loadBoardFromFEN(&board, startingFEN);

	    int doubledPawnPenalty = doubledPawnTest(&board, true);
	    CPPUNIT_ASSERT_EQUAL(EvalParameters::doubledPawnPenalty, doubledPawnPenalty);
	  
	}
};
