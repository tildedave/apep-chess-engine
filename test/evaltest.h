#pragma once

#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include "common.h"
#include "xboard.h"
#include "eval.h"
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
                                "testDoubledPawnPenaltyWhiteTwoPawns",
				&EvalTest::testDoubledPawnPenaltyWhiteTwoPawns ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<EvalTest>(
                                "testDoubledPawnPenaltyBlackTwoPawns",
				&EvalTest::testDoubledPawnPenaltyBlackTwoPawns ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<EvalTest>(
                                "testRecognizesRepetition",
				&EvalTest::testRecognizesRepetition ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<EvalTest>(
                                "testRecognizesNoRepetition",
				&EvalTest::testRecognizesNoRepetition ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<EvalTest>(
                                "testProperlyHandlesWhitePawnCover",
				&EvalTest::testProperlyHandlesWhitePawnCover ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<EvalTest>(
                                "testProperlyHandlesWhitePawnCoverMissingOne",
				&EvalTest::testProperlyHandlesWhitePawnCoverMissingOne ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<EvalTest>(
                                "testProperlyHandlesWhitePawnCoverMissingTwo",
				&EvalTest::testProperlyHandlesWhitePawnCoverMissingTwo ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<EvalTest>(
                                "testProperlyHandlesWhitePawnCoverMissingThree",
				&EvalTest::testProperlyHandlesWhitePawnCoverMissingThree ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<EvalTest>(
                                "testProperlyHandlesBlackPawnCover",
				&EvalTest::testProperlyHandlesBlackPawnCover ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<EvalTest>(
                                "testProperlyHandlesBlackPawnCoverMissingOne",
				&EvalTest::testProperlyHandlesBlackPawnCoverMissingOne ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<EvalTest>(
                                "testProperlyHandlesBlackPawnCoverMissingTwo",
				&EvalTest::testProperlyHandlesBlackPawnCoverMissingTwo ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<EvalTest>(
                                "testProperlyHandlesBlackPawnCoverMissingThree",
				&EvalTest::testProperlyHandlesBlackPawnCoverMissingThree ) );


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

	void testDoubledPawnPenaltyWhiteTwoPawns() {
	    ChessBoard board;
	    std::string startingFEN("8/4P3/4P3/8/8/8/8/8 w KQkq - 0 1");
	    loadBoardFromFEN(&board, startingFEN);

	    int doubledPawnPenalty = doubledPawnTest(&board, true);
	    CPPUNIT_ASSERT_EQUAL(EvalParameters::doubledPawnPenalty, doubledPawnPenalty);
      	}

	void testDoubledPawnPenaltyBlackTwoPawns() {
	    ChessBoard board;
	    std::string startingFEN("8/4p3/4p3/8/8/8/8/8 w KQkq - 0 1");
	    loadBoardFromFEN(&board, startingFEN);

	    int doubledPawnPenalty = doubledPawnTest(&board, false);
	    CPPUNIT_ASSERT_EQUAL(EvalParameters::doubledPawnPenalty, doubledPawnPenalty);
      	}

	void testRecognizesRepetition() {
	  ChessBoard board;
	  std::string startingFEN("r11nRrk1/1p1R11pp/11p11111/p1111111/1PP111N1/11111111/P1111PPP/111111K1 w - - 0 27");
	  loadBoardFromFEN(&board, startingFEN);

	  processMove(&board, CoordStringToMove(&board, "d7e7"));
	  processMove(&board, CoordStringToMove(&board, "f8f7"));
	  processMove(&board, CoordStringToMove(&board, "e7d7"));
	  processMove(&board, CoordStringToMove(&board, "f7f8"));
	  processMove(&board, CoordStringToMove(&board, "d7e7"));
	  processMove(&board, CoordStringToMove(&board, "f8f7"));
	  processMove(&board, CoordStringToMove(&board, "e7d7"));
	  processMove(&board, CoordStringToMove(&board, "f7f8"));

	  CPPUNIT_ASSERT_EQUAL(true, checkForRepetition(&board));
	}

	void testRecognizesNoRepetition() {
	  ChessBoard board;
	  std::string startingFEN("r11nRrk1/1p1R11pp/11p11111/p1111111/1PP111N1/11111111/P1111PPP/111111K1 w - - 0 27");
	  loadBoardFromFEN(&board, startingFEN);

	  processMove(&board, CoordStringToMove(&board, "d7e7"));
	  processMove(&board, CoordStringToMove(&board, "f8f7"));
	  processMove(&board, CoordStringToMove(&board, "e7d7"));
	  processMove(&board, CoordStringToMove(&board, "f7f8"));
	  CPPUNIT_ASSERT_EQUAL(false, checkForRepetition(&board));
	}

	void testProperlyHandlesWhitePawnCover() {
	  ChessBoard board;
	  std::string startingFEN("8/8/8/8/8/8/PPPPPPPP/6K1 w - - 0 1");
	  
	  loadBoardFromFEN(&board, startingFEN);
	  int pawnScore = getKingPawnPlacementScore(&board, true);

	  CPPUNIT_ASSERT_EQUAL(0, pawnScore);
	}

	void testProperlyHandlesWhitePawnCoverMissingOne() {
	  ChessBoard board;
	  std::string startingFEN("8/8/8/8/8/8/PPPPP1PP/6K1 w - - 0 1");
	  
	  loadBoardFromFEN(&board, startingFEN);
	  int pawnScore = getKingPawnPlacementScore(&board, true);

	  CPPUNIT_ASSERT_EQUAL(EvalParameters::noPawnCoverBonus, pawnScore);
	}

	void testProperlyHandlesWhitePawnCoverMissingTwo() {
	  ChessBoard board;
	  std::string startingFEN("8/8/8/8/8/8/PPPPP2P/6K1 w - - 0 1");
	  
	  loadBoardFromFEN(&board, startingFEN);
	  int pawnScore = getKingPawnPlacementScore(&board, true);

	  CPPUNIT_ASSERT_EQUAL(EvalParameters::noPawnCoverBonus * 2, pawnScore);
	}

	void testProperlyHandlesWhitePawnCoverMissingThree() {
	  ChessBoard board;
	  std::string startingFEN("8/8/8/8/8/8/PPPPP3/6K1 w - - 0 1");
	  
	  loadBoardFromFEN(&board, startingFEN);
	  int pawnScore = getKingPawnPlacementScore(&board, true);

	  CPPUNIT_ASSERT_EQUAL(EvalParameters::noPawnCoverBonus * 3, pawnScore);
	}

	void testProperlyHandlesBlackPawnCover() {
	  ChessBoard board;
	  std::string startingFEN("6k1/pppppppp/8/8/8/8/8/8 w - - 0 1");
	  
	  loadBoardFromFEN(&board, startingFEN);
	  int pawnScore = getKingPawnPlacementScore(&board, false);

	  CPPUNIT_ASSERT_EQUAL(0, pawnScore);
	}


	void testProperlyHandlesBlackPawnCoverMissingOne() {
	  ChessBoard board;
	  std::string startingFEN("6k1/ppppp1pp/8/8/8/8/8/8 w - - 0 1");

	  loadBoardFromFEN(&board, startingFEN);
	  int pawnScore = getKingPawnPlacementScore(&board, false);

	  CPPUNIT_ASSERT_EQUAL(EvalParameters::noPawnCoverBonus, pawnScore);
	}

	void testProperlyHandlesBlackPawnCoverMissingTwo() {
	  ChessBoard board;
	  std::string startingFEN("6k1/ppppp2p/8/8/8/8/8/8 w - - 0 1");
	  
	  loadBoardFromFEN(&board, startingFEN);
	  int pawnScore = getKingPawnPlacementScore(&board, false);

	  CPPUNIT_ASSERT_EQUAL(EvalParameters::noPawnCoverBonus * 2, pawnScore);
	}

	void testProperlyHandlesBlackPawnCoverMissingThree() {
	  ChessBoard board;
	  std::string startingFEN("6k1/ppppp3/8/8/8/8/8/8 w - - 0 1");
	  
	  loadBoardFromFEN(&board, startingFEN);
	  int pawnScore = getKingPawnPlacementScore(&board, false);

	  CPPUNIT_ASSERT_EQUAL(EvalParameters::noPawnCoverBonus * 3, pawnScore);
	}
};
