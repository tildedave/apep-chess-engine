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
#include <sstream>
#include <time.h>
#ifndef WIN32
#include <sys/time.h>
#endif
#include "commonboards.h"
#include "board.h"
#include "movepriorityqueue.h"
#include "movegen.h"
#include "moveprocess.h"
#include "move.h"
#include "search.h"
#include "xboard.h"
#include "openbook.h"


using namespace std;

void tacticsTest(const std::string& tacticsFile);
bool doTacticsTest(const std::string& fenString, const std::string& answerString, bool noisy);

int randomSeed = -1;

int main(int argc, char** argv) {
	if (argc >= 2 && std::string(argv[1]) == "-random") {
		randomSeed = atoi(argv[2]);
	}
	initialize_common_boards();
	loadOpeningBook("bookl.dat");

	xboardMainLoop();
	
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
