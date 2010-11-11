//============================================================================
// Name        : apep.cpp
// Author      : Dave King
// Version     :
// Copyright   : GNU Lesser General Public License
// Description : Main function for apep.  XBoard/UCI interface
//============================================================================

#include <fstream>
#include <iostream>
#include <iomanip>
#include <time.h>
#ifndef WIN32
#include <sys/time.h>
#endif
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestResult.h>
#include <cppunit/ui/text/TestRunner.h>
#include "commonboards.h"
#include "board.h"
#include "movepriorityqueue.h"
#include "movegen.h"
#include "moveprocess.h"
#include "move.h"
#include "search.h"
#include "xboard.h"
#include "test/bittest.h"
#include "test/boardtest.h"
#include "test/searchtest.h"
#include "openbook.h"

using namespace std;

#define INDIV_PERFT_TIME 0
#define DIVIDE 0
#define NEW_PERFT 1

long perft(ChessBoard* board, int depth, int startingDepth);
int doUnitTests();
void tacticsTest(const std::string& tacticsFile);
bool doTacticsTest(const std::string& fenString, const std::string& answerString, bool noisy);

int randomSeed = -1;

double moveGenTime;
double moveProcessTime;
double moveUnprocessTime;

void doPerftTest(std::string perftString = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
{
	ChessBoard board;
	loadBoardFromFEN(&board, perftString);
#ifdef WIN32
	SYSTEMTIME startSystemTime, endSystemTime;
	FILETIME start, end;
#else
	struct timeval start, end;
#endif
    for(int i = 0; i <= 6; ++i) {
  	  moveGenTime = 0.0;
	  moveProcessTime = 0.0;
	  moveUnprocessTime = 0.0;
#ifdef WIN32
	GetSystemTime(&startSystemTime);
	SystemTimeToFileTime(&startSystemTime, &start);
#else
	gettimeofday(&start, NULL);
#endif
	  int perftNum = perft(&board, i, i);
#ifdef WIN32
	  GetSystemTime(&endSystemTime);
	  SystemTimeToFileTime(&endSystemTime, &end);
   	  __int64 nanoSecondsDiff = ((__int64) (end.dwHighDateTime - start.dwHighDateTime)) << 32;
	  nanoSecondsDiff += end.dwLowDateTime - start.dwLowDateTime;
	  double diff = nanoSecondsDiff / (10000000.0);
#else
	  gettimeofday(&end, NULL);
	  double diff = (end.tv_sec - start.tv_sec);
	  diff += (end.tv_usec - start.tv_usec) / (1000.0 * 1000.0);
#endif
	  cerr << std::fixed << std::setprecision(4) << diff << "\t" << perftNum << " (movegen: " << moveGenTime << ", process: " << moveProcessTime << ", unprocess: " << moveUnprocessTime << ")" << endl;
	}
}

int main(int argc, char** argv) {
	if (argc >= 2 && std::string(argv[1]) == "-random") {
		randomSeed = atoi(argv[2]);
	}
	initialize_common_boards();
	loadOpeningBook("bookl.dat");

	if (argc >= 2) {
		std::string argv1(argv[1]);
		if (argv1 == "-perft") {
			doPerftTest();
			return 0;
		}
		if (argv1 == "-dotests"){
			doUnitTests();
			return 0;
		}
	}
	
	xboardMainLoop();
	
	return 0;
}

long perft(ChessBoard* board, int depth, int startingDepth) {
	if (depth == 0)
		return 1;

	long numNodes = 0;
#if INDIV_PERFT_TIME
	timeval start, end;
	gettimeofday(&start, NULL);
#endif

#if NEW_PERFT
	int moves[128];
	int* startMoves = moves;
	int* endMoves = generateCaptures(board, board->whiteToMove, startMoves);
//	for(int i = startingDepth; i > depth; i--) {
//		cerr << "\t";
//	}
//	cerr << "captures: " << mll.toMoveStringNew(board) << endl;
	endMoves = generateNonCaptures(board, board->whiteToMove, endMoves);	
//	for(int i = startingDepth; i > depth; i--) {
//		cerr << "\t";
//	}
//	cerr << "moves: " << mll.toMoveStringNew(board) << endl;
	
#else
	MovePriorityQueue possibleMoves;
	if (board->whiteToMove)
		getWhiteMoves(board, possibleMoves);
	else
		getBlackMoves(board, possibleMoves);
#endif
	
//	if (startingDepth == depth) {
//		MovePriorityQueue otherMoves(possibleMoves);
//		while(!otherMoves.isEmpty()) {
//			int move = otherMoves.removeMaximum();
//			cerr << offset_to_string(getFrom(move)) << offset_to_string(getTo(move)) << endl;
//		}
//	}
#if INDIV_PERFT_TIME
	gettimeofday(&end, NULL);
	moveGenTime += (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / (1000 * 1000.0);
#endif

	//for(int i = depth; i < 2; ++i)
	//	cerr << "\t";
	//cerr << possibleMoves.toMoveString() << endl;

#if NEW_PERFT
	for(int* currentMove = startMoves; currentMove < endMoves; ++currentMove) {
		int cm = *currentMove;
#else
	while(!possibleMoves.isEmpty()) {
		int cm = possibleMoves.removeMaximum();
#endif
		short kingToCheck = board->whiteToMove ? KING_WHITE : KING_BLACK;
#if INDIV_PERFT_TIME
		gettimeofday(&start, NULL);
#endif
		processMove(board, cm);
#if INDIV_PERFT_TIME
		gettimeofday(&end, NULL);
		moveProcessTime += (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / (1000 * 1000.0);
#endif

		if (isKingInCheck(board, kingToCheck)) {
		  //for(int i = depth; i < 2; ++i)
		  //	cerr << "\t";
			//cerr << "move " << move_to_string(cm) << " puts king in check" << endl;
		}
		else {
		  //for(int i = depth; i < startingDepth; ++i)
		  //  cerr << "\t";
		  //cerr << "considering " << move_to_string(cm) << endl; 
			//cerr << "leads to board\n" << board_to_string(board) << endl;
			BITBOARD inc;
			if (depth == 1)
				inc = 1; 
			else
				inc = perft(board, depth - 1, startingDepth);
			//for(int i = depth; i < startingDepth; ++i)
			//  cerr << "\t";
#if DIVIDE
			if (depth == startingDepth) {
				cerr << offset_to_string(getFrom(cm)) << offset_to_string(getTo(cm)) << " " << inc << endl;
			}
#endif
			//cerr << move_to_string(cm) << " adds " << inc << " nodes" << endl;
			numNodes += inc;
		}
#if INDIV_PERFT_TIME
		gettimeofday(&start, NULL);
#endif
		unprocessMove(board, cm);
#if INDIV_PERFT_TIME
		gettimeofday(&end, NULL);
		moveUnprocessTime += (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / (1000 * 1000.0);
#endif
	}

	return numNodes;
}

int
doUnitTests() {
	CppUnit::TextUi::TestRunner runner;
	runner.addTest( BoardTest::suite() );

        // tests currently broken -- 
        // throw core dumps
        //runner.addTest( BitTest::suite() );
	//runner.addTest( SearchTest::suite() );
	runner.run();
	
	return 0;
}

void tacticsTest(const std::string& tacticsFile) {
	std::ifstream wacFileStream(tacticsFile.c_str());
	if (!wacFileStream) {
		cerr << "error reading file" << endl;
	}
	else {
		int totalTests = 0;
		int succeededTests = 0;
		std::string testString;
//		std::vector<std::string> answers;
		while(wacFileStream.eof() == false) {
			getline(wacFileStream, testString);
			string::size_type bmPos = testString.find("bm", 0);
			if (bmPos != string::npos) {
				// FEN comes before bm.
				std::string fenString = testString.substr(0, bmPos);
				int semiPos = testString.find(";", bmPos);
				std::string answerString = testString.substr(bmPos + 3, semiPos - (bmPos + 3));
//				cerr << "fen: " << fenString << endl;
//				cerr << "answerString: " << answerString << endl;
				std::string idString = testString.substr(semiPos + 6, string::npos);
				string::size_type lastQuote = idString.find("\";",0);
				std::string strippedString = idString.substr(0, lastQuote);

//				cerr << "ID: " << idString << endl;
				
				cout << strippedString << "\t\t\t" << fenString << endl;
				cout << "expected: " << answerString << " ... ";
				flush(cout);
				bool success = doTacticsTest(fenString, answerString, false);
				++totalTests;
				if (success) {
//					cout << "TEST " << idString << ": SUCCESS (" << answerString << ")" << endl;
					cout << " (SUCCESS)" << endl;
					++succeededTests;
				}
				else {
//					cout << "TEST " << idString << ": FAILURE (expected " << answerString << ")" << endl;
					cout << " (FAILURE)" << endl;
				}
			}
		}
		
		cerr << "succeeded " << succeededTests << " / " << totalTests << " (" << std::fixed << std::setprecision(4) << (succeededTests / (totalTests * 1.0)) << "%)" << endl;
	}
}

bool doTacticsTest(const std::string& fenString, const std::string& answerString, bool noisy) {
	ChessBoard board;
	loadBoardFromFEN(&board, fenString);
	int move = getMove(&board, noisy);
	std::string moveToString = MoveToString(move);
	cout << moveToString;
	std::stringstream strStream(answerString);
	
	while (!strStream.eof()) {
		std::string answerStringWithoutPlusAndSharp;
		strStream >> answerStringWithoutPlusAndSharp;
		string::size_type plusIndex = answerStringWithoutPlusAndSharp.find("+");
		if (plusIndex != string::npos)
			answerStringWithoutPlusAndSharp.replace(plusIndex, 1, "");
		string::size_type sharpIndex = answerStringWithoutPlusAndSharp.find("#");
		if (sharpIndex != string::npos)
			answerStringWithoutPlusAndSharp.replace(sharpIndex, 1, "");

		if (answerStringWithoutPlusAndSharp == moveToString)
			return true;

		std::string fromToString = offset_to_string(GetFrom(move)) + offset_to_string(GetTo(move));

		if (answerStringWithoutPlusAndSharp == fromToString)
			return true;
	}
	return false;
}
