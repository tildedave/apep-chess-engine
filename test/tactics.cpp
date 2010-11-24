#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <boost/program_options.hpp>

#include "board.h"
#include "move.h"
#include "search.h"

namespace po = boost::program_options;

int randomSeed = -1;

bool doTacticsTest(const std::string& fenString, const std::string& answerString, bool noisy) {
	ChessBoard board;
	std::cout << "loading FEN" << std::endl;

	loadBoardFromFEN(&board, fenString);
	std::cout << "getting move for FEN" << std::endl;
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

				std::string idString = testString.substr(semiPos + 6, string::npos);
				string::size_type lastQuote = idString.find("\";",0);
				std::string strippedString = idString.substr(0, lastQuote);

				cout << strippedString << "\t\t\t" << fenString << endl;
				cout << "expected: " << answerString << " ... ";
				flush(cout);
				bool success = doTacticsTest(fenString, answerString, false);
				++totalTests;
				if (success) {
					cout << " (SUCCESS)" << endl;
					++succeededTests;
				}
				else {
					cout << " (FAILURE)" << endl;
				}
			}
		}
		
		cerr << "succeeded " << succeededTests << " / " << totalTests << " (" << std::fixed << std::setprecision(4) << (succeededTests / (totalTests * 1.0)) << "%)" << endl;
	}
}

int main(int argc, char** argv) {
  initialize_common_boards();

  po::options_description desc("Allowed options");
  std::string fenFile;
  desc.add_options()
    ("help", "produce help message")
    ("file", po::value<std::string>(), "File to read FEN positions from (required)")
    ("fen", po::value<std::string>(), "FEN to solve")
    ("expected", po::value<std::string>(), "Expected solution for FEN");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);    

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    exit(0);
  }

  if (vm.count("file") && vm.count("fen")) {
	    std::cout << desc << std::endl;
	    exit(0);
  }

  if (!vm.count("file") && !vm.count("fen")) {
	    std::cout << desc << std::endl;
	    exit(0);
  }

  if (vm.count("fen") && !vm.count("expected")) {
	    std::cout << desc << std::endl;
	    exit(0);
  }

  if (vm.count("file")) {
	  std::string file = vm["file"].as<std::string>();
	  tacticsTest(file);
  }
  if (vm.count("fen")) {
	  std::string fen = vm["fen"].as<std::string>();
	  std::string expected = vm["expected"].as<std::string>();
	  doTacticsTest(fen, expected, true);
  }
}
