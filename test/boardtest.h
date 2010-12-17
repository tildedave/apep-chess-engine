#ifndef BOARDTEST_H_
#define BOARDTEST_H_

#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include "common.h"
#include "board.h"
#include "eval.h"
#include "move.h"
#include "moveprocess.h"
#include "movegen.h"

class BoardTest : public CppUnit::TestCase {

public:

	static CppUnit::Test *suite()
	{
		CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite( "BoardTest" );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
				"testMoveAndUnmoves", 
				&BoardTest::testMoveAndUnmoves ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
				"testMoveAndUnmoves2", 
				&BoardTest::testMoveAndUnmoves2 ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
				"testMoveAndUnmoves3", 
				&BoardTest::testMoveAndUnmoves3 ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
				"testMoveAndUnmoves4", 
				&BoardTest::testMoveAndUnmoves4 ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
				"testMoveAndUnmoves5", 
				&BoardTest::testMoveAndUnmoves5 ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
				"testMoveAndUnmoves6", 
				&BoardTest::testMoveAndUnmoves6 ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
				"testMoveAndUnmoves7", 
				&BoardTest::testMoveAndUnmoves7 ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
				"testMoveRecursive", 
				&BoardTest::testMoveRecursive ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
				"testMoveRecursive2", 
				&BoardTest::testMoveRecursive2 ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
				"testPawnCheck", 
				&BoardTest::testPawnCheck ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
				"testCheckmate1", 
				&BoardTest::testCheckmate1 ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
				"testCheckmate2", 
				&BoardTest::testCheckmate2 ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
				"testCheckmate3", 
				&BoardTest::testCheckmate3 ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
				"testCheckmate4", 
				&BoardTest::testCheckmate4 ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
				"testCheckmate5", 
				&BoardTest::testCheckmate5 ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
				"testCheckmate6", 
				&BoardTest::testCheckmate6 ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
				"testNotCheckmate1", 
				&BoardTest::testNotCheckmate1 ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
				"testNotCheckmate2", 
				&BoardTest::testNotCheckmate2 ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
				"testNotCheckmate3", 
				&BoardTest::testNotCheckmate3 ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
				"testNotCheckmate4", 
				&BoardTest::testNotCheckmate4 ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
				"testCanCastle", 
				&BoardTest::testCanCastle ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
				"testCantCastleOutOfCheck", 
				&BoardTest::testCantCastleOutOfCheck ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
				"testCantCastleThroughCheck", 
				&BoardTest::testCantCastleThroughPieces ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
				"testCantCastleThroughCheck2", 
				&BoardTest::testCantCastleThroughCheck ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
				"testCantCastleIntoCheck", 
				&BoardTest::testCantCastleIntoCheck ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
				"testCastleBug", 
				&BoardTest::testCastleBug ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
						"testEnPassant", 
						&BoardTest::testEnPassant ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>( 
						"testReverseEnPassant", 
						&BoardTest::testReverseEnPassant ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>(
						"testConvertStartingBoardToFEN",
						&BoardTest::testConvertStartingBoardToFEN ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>(
						"testGameResultDetectsCheckmate",
						&BoardTest::testGameResultDetectsCheckmate ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>(
						"testGameResultDetectsLackOfMaterial_KK",
						&BoardTest::testGameResultLackOfMaterial_KK ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>(
						"testGameResultDetectsLackOfMaterial_KKN",
						&BoardTest::testGameResultLackOfMaterial_KKN ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>(
						"testGameResultDetectsLackOfMaterial_KNK",
						&BoardTest::testGameResultLackOfMaterial_KNK ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>(
						"testGameResultDetectsLackOfMaterial_KKB",
						&BoardTest::testGameResultLackOfMaterial_KKB ) );
		suiteOfTests->addTest( new CppUnit::TestCaller<BoardTest>(
						"testGameResultDetectsLackOfMaterial_KBK",
						&BoardTest::testGameResultLackOfMaterial_KBK ) );

		return suiteOfTests;
	}
	
	void testMoveAndUnmoves() {
		ChessBoard board;
		loadBoardFromFEN(&board, "8/3k4/8/8/8/8/2Kp4/8 w - - 0 1");
		doTestCompactMovesAndUnmovesForFenString(&board, 1, true);
	}

	void testMoveAndUnmoves2() {
		ChessBoard board;
		loadBoardFromFEN(&board, "8/3k4/8/8/8/8/2KpR3/8 w - - 0 1");
		doTestCompactMovesAndUnmovesForFenString(&board, 1, true);
	}

	void testMoveAndUnmoves3() {
		ChessBoard board;
		loadBoardFromFEN(&board, "8/3k4/3r4/8/8/8/2KpR3/8 w - - 0 1");
		doTestCompactMovesAndUnmovesForFenString(&board, 1, true);
	}

	void testMoveAndUnmoves4() {
		ChessBoard board;
		loadBoardFromFEN(&board, "8/3kR3/3r4/8/8/8/2KpR3/8 b - - 0 1");
		doTestCompactMovesAndUnmovesForFenString(&board, 1, false);
	}

	void testMoveAndUnmoves5() {
		std::string captureFenString = "8/3k4/3r4/8/8/8/1K1pR3/8 w - - 0 1";
		ChessBoard board; loadBoardFromFEN(&board, captureFenString);

		doTestCompactMovesAndUnmovesForFenString(&board, 1, true);
	}

	void testMoveAndUnmoves6() {
		std::string captureFenString = "2k5/8/8/8/8/2K5/3p4/8 b - - 0 1";
		ChessBoard board; loadBoardFromFEN(&board, captureFenString);
		doTestCompactMovesAndUnmovesForFenString(&board, 1, false);
	}

	void testMoveAndUnmoves7() {
		std::string captureFenString = "11k11111/11111111/11111111/11111111/11111111/11111111/111p1111/111K1111 w - - 0 0";
		ChessBoard board; loadBoardFromFEN(&board, captureFenString);
		doTestCompactMovesAndUnmovesForFenString(&board, 2, true);
	}

	void testMoveRecursive() {
		std::string captureFenString = "2k5/8/8/8/8/2K5/3p4/8 b - - 0 1";
		ChessBoard board; loadBoardFromFEN(&board, captureFenString);
		doTestCompactMovesAndUnmovesForFenString(&board, 4, false);
	}

	void testMoveRecursive2() {
		std::string captureFenString = "8/3k4/8/8/8/8/2Kp4/8 w - - 0 1";
		ChessBoard board; loadBoardFromFEN(&board, captureFenString);
		doTestCompactMovesAndUnmovesForFenString(&board, 4, true);
	}

	void testPawnCheck() {
		std::string whiteInCheckFromBlackPawnString = "6k1/2p2ppp/4b3/1P1p4/P2P1P2/r1b1P3/P2p2PP/1RK4R w - - 0 1";
		ChessBoard board; 
		loadBoardFromFEN(&board, whiteInCheckFromBlackPawnString);

		CPPUNIT_ASSERT(isKingInCheck(&board, KING_WHITE));
	}

	void testCheckmate1() {
		std::string checkMateString = "3k3R/8/3K4/8/8/8/8/8 b - - 0 1";
		ChessBoard board; loadBoardFromFEN(&board, checkMateString);
		// checkmate for white
		CPPUNIT_ASSERT_EQUAL(1, getGameResult(&board));
	}

	void testCheckmate2() {
		std::string morphyStr = "11111rk1/1q111pQp/pn111111/1p111111/11111111/1P111P11/PBP1111P/11KR1111 b - - 0 1";
		ChessBoard board; loadBoardFromFEN(&board, morphyStr);
		// checkmate for white
		CPPUNIT_ASSERT_EQUAL(1, getGameResult(&board));
	}

	void testCheckmate3() {
		std::string morphyStr2 = "11111r1k/1q111p1p/pn111B11/1p111111/11111111/1P111P11/P1P1111P/11K111R1 b - - 0 1";
		ChessBoard board; loadBoardFromFEN(&board, morphyStr2);
		// checkmate for white
		CPPUNIT_ASSERT_EQUAL(1, getGameResult(&board));
	}

	void testCheckmate4() {
		// queen death hug
		std::string checkMateString = "3k4/3Q4/3K4/8/8/8/8/8 b - - 0 1";
		ChessBoard board; loadBoardFromFEN(&board, checkMateString);
		// checkmate for white
		CPPUNIT_ASSERT_EQUAL(1, getGameResult(&board));
	}

	void testCheckmate5() {
		// queen death hug
		std::string checkMateString = "r1k1R3/1p1n4/1BpPBp1p/6p1/8/2P5/PP4P1/6K1 b - - 0 2";
		ChessBoard board; loadBoardFromFEN(&board, checkMateString);
		// checkmate for white
		CPPUNIT_ASSERT_EQUAL(1, getGameResult(&board));
	}

	void testCheckmate6() {
		// from crafty vs apep 0.0.1
		std::string checkmateString = "r3k2r/pp3ppp/2p1b3/2b5/P2q1P2/N1K1n3/1PP4P/R1B5 w - - 0 1";
		ChessBoard board; loadBoardFromFEN(&board, checkmateString);
		// checkmate for black
		CPPUNIT_ASSERT_EQUAL(-1, getGameResult(&board));
	}

	void testNotCheckmate1() {
		// can capture checking piece
		std::string checkMateString = "3k3R/8/3K4/8/8/8/8/7r b - - 0 1";
		ChessBoard board; loadBoardFromFEN(&board, checkMateString);
		// not checkmate for white
		CPPUNIT_ASSERT_EQUAL(0, getGameResult(&board));
	}

	void testNotCheckmate2() {
		// can block checking piece
		std::string checkMateString = "3k3R/8/3K4/8/8/8/8/6r1 b - - 0 1";
		ChessBoard board; loadBoardFromFEN(&board, checkMateString);
		// not checkmate for white
		CPPUNIT_ASSERT_EQUAL(0, getGameResult(&board));
	}

	void testNotCheckmate3() {
		// can block checking piece
		std::string checkMateString = "3k4/8/3K4/8/8/8/8/6R1 b - - 0 1";
		ChessBoard board;
		loadBoardFromFEN(&board, checkMateString);
		// not checkmate for white
		CPPUNIT_ASSERT_EQUAL(0, getGameResult(&board));
	}

	void testNotCheckmate4() {
		// can block checking piece
		std::string checkMateString = "rnk1qR2/1p6/1BpPBp1p/6p1/8/2P5/PP4P1/6K1 b - - 0 6";
		ChessBoard board;
		loadBoardFromFEN(&board, checkMateString);
		// not checkmate for white
		CPPUNIT_ASSERT_EQUAL(0, getGameResult(&board));
	}

	void doTestCompactMovesAndUnmovesForFenString(ChessBoard * board, int depth, bool isWhite) {
		if (depth == 0)
			return;
		
		CPPUNIT_ASSERT(board->whiteToMove == isWhite);
		
		ChessBoard originalBoard;
		copy_board(&originalBoard, board);
		std::string boardString = board_to_string(board);

		// this tests both chessboard processing behavior and 
		// move generation, aBITBOARD with the six move generating
		// functions (getMoves, getCaptures, getChecks) w/b

		if (!sideToMoveIsInCheck(board)) {
			processNullMove(board);
			CPPUNIT_ASSERT_MESSAGE("processing null move messed up board", internalConsistencyCheck(board));
			doTestCompactMovesAndUnmovesForFenString(board, depth - 1, !isWhite);
			unprocessNullMove(board);
		}
		
//		cerr << "going to get moves" << endl;
		int moves[256];
		int* startMoves = moves;
		int* endMoves;

		if (isWhite) {
//			getWhiteMoves(board, possibleMoves);
//			getWhiteMoves(board, originalPossibleMoves);
			endMoves = generateCaptures(board, true, startMoves);
			endMoves = generateNonCaptures(board, true, endMoves);
		}
		else {
//			getBlackMoves(board, possibleMoves);
//			getBlackMoves(board, originalPossibleMoves);
			endMoves = generateCaptures(board, false, startMoves);
			endMoves = generateNonCaptures(board, false, endMoves);
		}
//		cerr << "got moves" << endl;
		
		CPPUNIT_ASSERT_MESSAGE("generating captures messed up board", internalConsistencyCheck(board));

//		System.err.println(originalBoard.toString());
//		System.err.println(possibleMoves.toDetailString());
//		System.err.println(origPossibleMoves.toDetailString());
		
		CPPUNIT_ASSERT_EQUAL_MESSAGE("getting moves messed up board", boardString, board_to_string(board));
		CPPUNIT_ASSERT_MESSAGE("board not equal to itself -- copy method problem", boardsEqual(&originalBoard, board));
		
		
		//MoveLinkedList::Iterator itMoves = possibleMoves.iterator();
		//while(!itMoves.atEnd()) {
		for(int* currentMove = startMoves; currentMove < endMoves; ++currentMove) {
			int cm = *currentMove;
			
			CPPUNIT_ASSERT(board->whiteToMove == isWhite);

//			cerr << "process move: " << MoveToString(cm) << endl;
//			cerr << board_to_string(board) << endl;
			processMove(board, cm);
//			cerr << "now: " << endl;
//			cerr << board_to_string(board) << endl;
			std::string message("processing move " + MoveToString(cm) + " messed up board");
			CPPUNIT_ASSERT_MESSAGE(message, internalConsistencyCheck(board));
			doTestCompactMovesAndUnmovesForFenString(board, depth - 1, !isWhite);
//			cerr << "unprocess move: " << MoveToString(cm) << endl;
//			cerr << board_to_string(board) << endl;
			unprocessMove(board, cm);
//			cerr << "now: " << endl;
//			cerr << board_to_string(board) << endl;

			message = std::string("unprocessing move " + MoveToString(cm) + " messed up board");
			CPPUNIT_ASSERT_MESSAGE(message, internalConsistencyCheck(board));

			std::string failureMessage = "move ";
			failureMessage += MoveToString(cm);
			failureMessage += " messed up board";
			
			CPPUNIT_ASSERT_EQUAL_MESSAGE(failureMessage, boardString, board_to_string(board));
 			CPPUNIT_ASSERT_MESSAGE(failureMessage, boardsEqual(&originalBoard, board));
		}
	}
	
	void testCanCastle() {
		std::string whiteCastleString = "R3k2R/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1";
		ChessBoard board;
		loadBoardFromFEN(&board, whiteCastleString);
		
		CPPUNIT_ASSERT(board.whiteCanCastleKingside);
		CPPUNIT_ASSERT(board.blackCanCastleKingside);
		CPPUNIT_ASSERT(board.whiteCanCastleQueenside);
		CPPUNIT_ASSERT(board.blackCanCastleQueenside);
		
		doTestCompactMovesAndUnmovesForFenString(&board, 2, true);
	}
	
	void testCantCastleOutOfCheck() {
		std::string blackCantCastleOutOfCheckString = "R3k2R/pppp1ppp/8/8/4R3/8/PPPPPPPP/4K3 b kq - 0 1";
		ChessBoard board;  
		loadBoardFromFEN(&board, blackCantCastleOutOfCheckString);
		
		int moves[64];
		int* endMoves = generateNonCaptures(&board, false, moves);
		for(int *ptr = moves; ptr < endMoves; ++ptr) {
			int move = *ptr;
			if (MoveToString(move) == "O-O-O")
				CPPUNIT_ASSERT_MESSAGE("cannot castle out of check", false);
			if (MoveToString(move) == "O-O")
				CPPUNIT_ASSERT_MESSAGE("cannot castle out of check", false);
		}	
	}
	
	void testCantCastleThroughPieces() {
		std::string blackCantCastleThroughPiecesString = "RN2k1NR/pppp1ppp/8/8/4R3/8/PPPPPPPP/4K3 b kq - 0 1";
		ChessBoard board;  loadBoardFromFEN(&board, blackCantCastleThroughPiecesString);
		
		int moves[64];
		int* endMoves = generateNonCaptures(&board, false, moves);
		for(int *ptr = moves; ptr < endMoves; ++ptr) {
			int move = *ptr;
			if (MoveToString(move) == "O-O-O")
				CPPUNIT_ASSERT_MESSAGE("cannot castle out of check", false);
			if (MoveToString(move) == "O-O")
				CPPUNIT_ASSERT_MESSAGE("cannot castle out of check", false);
		}	
	}
	
	void testCantCastleThroughCheck() {
		std::string blackCantCastleThroughCheckString = "R3k2R/ppp1p1pp/8/8/3R1R1/8/PPPPPPPP/4K3 b kq - 0 1";
		ChessBoard board;  loadBoardFromFEN(&board, blackCantCastleThroughCheckString);
		
		int moves[64];
		int* endMoves = generateNonCaptures(&board, false, moves);
		for(int *ptr = moves; ptr < endMoves; ++ptr) {
			int move = *ptr;
			if (MoveToString(move) == ("O-O-O"))
				CPPUNIT_ASSERT_MESSAGE("cannot castle through check", false);
			if (MoveToString(move) == ("O-O"))
				CPPUNIT_ASSERT_MESSAGE("cannot castle through check", false);
		}
	}
	
	void testCantCastleIntoCheck() {
		std::string blackCantCastleIntoCheckString = "R3k2R/pp1ppp1p/8/8/2R3R1/8/PPPPPPPP/4K3 b kq - 0 1";
		ChessBoard board;  loadBoardFromFEN(&board, blackCantCastleIntoCheckString);
		
		int moves[64];
		int* endMoves = generateNonCaptures(&board, false, moves);
		for(int *ptr = moves; ptr < endMoves; ++ptr) {
			int move = *ptr;
			if (MoveToString(move) == "O-O-O")
				CPPUNIT_ASSERT_MESSAGE("cannot castle into check", false);
			if (MoveToString(move) == "O-O")
				CPPUNIT_ASSERT_MESSAGE("cannot castle out of check", false);
		}
	}
	
	void testCastleBug() {
		std::string fenString ="rnbqkbnr/pppppp11/8/6p1/8/4PNP1/PPPP2BP/RNBQK2R b KQkq - 0 1";
		ChessBoard board;  loadBoardFromFEN(&board, fenString);
		
		doTestCompactMovesAndUnmovesForFenString(&board, 3, false);
	}
	

	void testEnPassant() {
		std::string fenString = "8/8/4k3/8/1p6/8/P1P5/7K w - - 0 1";
		ChessBoard board;
		loadBoardFromFEN(&board, fenString);

		doTestCompactMovesAndUnmovesForFenString(&board, 2, true);

		// okay.  find a2 - a4.  verify that black can take a3.

		int moves[256];
		int replies[256];
		int* startMoves = moves;
		int* endMoves;
		endMoves = generateCaptures(&board, true, startMoves);
		endMoves = generateNonCaptures(&board, true, endMoves);
		
		for(int* currentMove = startMoves; currentMove < endMoves; ++currentMove) {
			int move = *currentMove;

			if (MoveToString(move) == "c4") {
				processMove(&board, move);
				CPPUNIT_ASSERT(board.enPassantOffset == C3);

				int* startReplies = replies;
				int* endReplies;

				endReplies = generateCaptures(&board, false, startReplies);
				endReplies = generateNonCaptures(&board, false, endReplies);
				
				bool foundEnPassantCapture = false;

				for(int* currentReply = startReplies; currentReply < endReplies; ++currentReply) {
					int reply = *currentReply;

					if (MoveToString(reply) == "bxc3") {
						foundEnPassantCapture = true;
						unprocessMove(&board, move);
					}
				}

				if (!foundEnPassantCapture)
					CPPUNIT_ASSERT_MESSAGE("could not find en passant capture on c3 after c4", false);
			}
			if (MoveToString(move) == "a4") {
				processMove(&board, move);
				CPPUNIT_ASSERT(board.enPassantOffset == A3);

				//int* startReplies = repliesArray;
				int* endReplies;
				
				//cerr << startReplies  << " vs " << repliesArray << endl;
				endReplies = generateCaptures(&board, false, replies);
				endReplies = generateNonCaptures(&board, false, endReplies);

				bool foundEnPassantCapture = false;

				for(int* currentReply = replies; currentReply < endReplies; ++currentReply) {
					int reply = *currentReply;
					if (MoveToString(reply) == "bxa3") {
						foundEnPassantCapture = true;
						unprocessMove(&board, move);
						//System.err.println(board);
					}
				}

				if (!foundEnPassantCapture)
					CPPUNIT_ASSERT_MESSAGE("could not find en passant capture on a3 after a4", false);
			}
		}
		// find c2 - c4.  verify that black can take c3.

	}
	
	void testReverseEnPassant() {
		std::string fenString = "7k/p1p5/8/1P6/8/8/8/7K b - - 0 1";
		ChessBoard board;
		loadBoardFromFEN(&board, fenString);

		doTestCompactMovesAndUnmovesForFenString(&board, 2, false);

		// okay.  find a2 - a4.  verify that black can take a3.

		int moves[256];
		int* startMoves = moves;
		int* endMoves;
		endMoves = generateCaptures(&board, false, startMoves);
		endMoves = generateNonCaptures(&board, false, endMoves);
		
		for(int* currentMove = startMoves; currentMove < endMoves; ++currentMove) {
			int move = *currentMove;

			if (MoveToString(move) == "c5") {
				processMove(&board, move);
				CPPUNIT_ASSERT(board.enPassantOffset == C6);
				int replies[256];
				int* startReplies = replies;
				int* endReplies;

				endReplies = generateCaptures(&board, true, startReplies);
				endReplies = generateNonCaptures(&board, true, endReplies);
				
				bool foundEnPassantCapture = false;

				for(int* currentReply = startReplies; currentReply < endReplies; ++currentReply) {
					int reply = *currentReply;

					if (MoveToString(reply) == "bxc6") {
						foundEnPassantCapture = true;
						unprocessMove(&board, move);
					}
				}

				if (!foundEnPassantCapture)
					CPPUNIT_ASSERT_MESSAGE("could not find en passant capture on c6 after c5", false);
			}
			if (MoveToString(move) == "a5") {
				processMove(&board, move);
				CPPUNIT_ASSERT(board.enPassantOffset == A6);

				int replies[256];
				int* startReplies = replies;
				int* endReplies;

				endReplies = generateCaptures(&board, true, startReplies);
				endReplies = generateNonCaptures(&board, true, endReplies);
				
				bool foundEnPassantCapture = false;

				for(int* currentReply = startReplies; currentReply < endReplies; ++currentReply) {
					int reply = *currentReply;
					if (MoveToString(reply) == "bxa6") {
						foundEnPassantCapture = true;
						unprocessMove(&board, move);
						//System.err.println(board);
					}
				}

				if (!foundEnPassantCapture)
					CPPUNIT_ASSERT_MESSAGE("could not find en passant capture on a6 after a5", false);
			}
		}
		// find c2 - c4.  verify that black can take c3.

	}

        void testConvertStartingBoardToFEN() {
 	    ChessBoard board;
	    std::string startingFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	    loadBoardFromFEN(&board, startingFEN);
	    std::string convertedBoard = boardToFEN(&board);
	    std::string expectedFEN = "rnbqkbnr/pppppppp/11111111/11111111/11111111/11111111/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	    CPPUNIT_ASSERT_EQUAL(convertedBoard, expectedFEN);
	}
	
	void testGameResultDetectsCheckmate() {
	    ChessBoard board;
	    std::string checkmateFEN("7r/p1p5/1p2P3/6r1/8/P4k2/P6K/8 w - - 2 38");
	    loadBoardFromFEN(&board, checkmateFEN);
	    int gameResult = getGameResult(&board);
	    CPPUNIT_ASSERT_EQUAL(gameResult, -1);
	}

	void testGameResultLackOfMaterial_KK() {
	  ChessBoard board;
	  std::string kkFEN("7k/8/8/4K3/8/8/8/8 w - - 0 1");
	  loadBoardFromFEN(&board, kkFEN);
	  int gameResult = getGameResult(&board);

	  CPPUNIT_ASSERT_EQUAL(gameResult, 3);
	}

	void testGameResultLackOfMaterial_KNK() {
	  ChessBoard board;
	  std::string knkFEN("7k/8/8/4KN2/8/8/8/8 w - - 0 1");
	  loadBoardFromFEN(&board, knkFEN);
	  int gameResult = getGameResult(&board);

	  CPPUNIT_ASSERT_EQUAL(gameResult, 3);
	}

	void testGameResultLackOfMaterial_KKN() {
	  ChessBoard board;
	  std::string kknFEN("6nk/8/8/4K3/8/8/8/8 w - - 0 1");
	  loadBoardFromFEN(&board, kknFEN);
	  int gameResult = getGameResult(&board);

	  CPPUNIT_ASSERT_EQUAL(gameResult, 3);
	}

	void testGameResultLackOfMaterial_KBK() {
	  ChessBoard board;
	  std::string kbkFEN("7k/8/8/4KB2/8/8/8/8 w - - 0 1");
	  loadBoardFromFEN(&board, kbkFEN);
	  int gameResult = getGameResult(&board);

	  CPPUNIT_ASSERT_EQUAL(gameResult, 3);
	}

	void testGameResultLackOfMaterial_KKB() {
	  ChessBoard board;
	  std::string kkbFEN("6bk/8/8/4K3/8/8/8/8 w - - 0 1");
	  loadBoardFromFEN(&board, kkbFEN);
	  int gameResult = getGameResult(&board);

	  CPPUNIT_ASSERT_EQUAL(gameResult, 3);
	}
};

#endif /*BOARDTEST_H_*/
