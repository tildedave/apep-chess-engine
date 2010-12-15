#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <list>
#include <boost/program_options.hpp>

#include "board.h"
#include "move.h"
#include "search.h"
#include "tactics.h"

namespace po = boost::program_options;

//int randomSeed = -1;
extern float TimeoutValue;

TacticsModule::TacticsModule(const std::string& fenString, 
			     const std::string& expected) : fenString_(fenString), 
							    expected_(expected),
							    wasSuccessful_(false)
{
}

void
TacticsModule::run() {
  ChessBoard board;
  loadBoardFromFEN(&board, this->fenString_);

  search_options options;
  options.noisyMode = true;
  
  int move = getMove(&board, &options);
  this->moveString_ = MoveToString(move);

  std::stringstream strStream(this->expected_);

  while (!strStream.eof()) {
    std::string answerStringWithoutPlusAndSharp;
    strStream >> answerStringWithoutPlusAndSharp;
    string::size_type plusIndex = answerStringWithoutPlusAndSharp.find("+");
    if (plusIndex != string::npos)
      answerStringWithoutPlusAndSharp.replace(plusIndex, 1, "");
    string::size_type sharpIndex = answerStringWithoutPlusAndSharp.find("#");
    if (sharpIndex != string::npos)
      answerStringWithoutPlusAndSharp.replace(sharpIndex, 1, "");
    
    if (answerStringWithoutPlusAndSharp == this->moveString_) {
      this->wasSuccessful_ = true;
      return;
    }
    
    std::string fromToString = offset_to_string(GetFrom(move)) + offset_to_string(GetTo(move));
    
    if (answerStringWithoutPlusAndSharp == fromToString) {
      this->wasSuccessful_ = true;
      return;
    }
  }
  this->wasSuccessful_ = false;
}

bool 
TacticsModule::wasSuccessful() {
  return wasSuccessful_;
}

std::string
TacticsModule::getMoveString() {
  return moveString_;
}


bool doTacticsTest(const std::string& fenString, const std::string& answerString, std::string& moveToString) {
	ChessBoard board;
	loadBoardFromFEN(&board, fenString);

	search_options options;
	options.noisyMode = true;

	int move = getMove(&board, &options);
	moveToString = MoveToString(move);
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

void outputStats(int succeededTests, int totalTests) {
  std::cout << "succeeded " 
            << succeededTests << " / " << totalTests 
            << " (" << std::fixed << std::setprecision(2) << (succeededTests / (totalTests * 1.0)) << "%)" << std::endl;
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
		std::list<std::string> incorrect;
		while(wacFileStream.eof() == false) {
			getline(wacFileStream, testString);
			string::size_type bmPos = testString.find("bm", 0);
			if (bmPos != string::npos) {
				// FEN comes before bm.
                                // TODO: this is pretty awful, just make this a regexp
				std::string fenString = testString.substr(0, bmPos);
				int semiPos = testString.find(";", bmPos);
				std::string answerString = testString.substr(bmPos + 3, semiPos - (bmPos + 3));
				std::string idString = testString.substr(semiPos + 6, std::string::npos);

				string::size_type lastQuote = idString.find("\";",0);
				std::string strippedString = idString.substr(0, lastQuote);

				cout << strippedString << "\t\t\t" << fenString << endl;
				cout << "expected: " << answerString << " ... " << endl;
				flush(cout);
                                std::string moveString;
				bool success = doTacticsTest(fenString, answerString, moveString);
				++totalTests;
                                std::cout << "received: " << moveString << std::endl;

				if (success) {
                                  cout << " (SUCCESS)" << endl;
                                  ++succeededTests;
				}
				else {
                                  cout << " (FAILURE; Expected: " << answerString << ")" << endl;
                                  // for some reason idString is coming with a vtab in it, making output
                                  // difficult
                                  incorrect.push_back(idString.substr(0, idString.size() - 1));
				}
			}

                        outputStats(succeededTests, totalTests);

                        if (incorrect.empty())
                          continue;

                        std::cout << "incorrect answers: " << std::endl;
                        std::cout << "\t[";
                        for(std::list<std::string>::iterator it = incorrect.begin(); 
                            it != incorrect.end(); 
                            ++it) {
                          std::cout << " " << *it;
                        }
                        std::cout << " ]" << std::endl;
		}
	}
}

int tacticsMain(int argc, char** argv) {
  initialize_common_boards();
  setupLogging();

  po::options_description desc("Allowed options");
  std::string fenFile;
  desc.add_options()
    ("help", "produce help message")
    ("file", po::value<std::string>(), "File to read FEN positions from (required)")
    ("fen", po::value<std::string>(), "FEN to solve")
    ("timeout", po::value<int>()->default_value(10), "Seconds per move (default value of 10 seconds/move)")
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

  TimeoutValue = vm["timeout"].as<int>();

  if (vm.count("file")) {
	  std::string file = vm["file"].as<std::string>();
	  tacticsTest(file);
  }
  if (vm.count("fen")) {
	  std::string fen = vm["fen"].as<std::string>();
	  std::string expected = vm["expected"].as<std::string>();
          std::string moveString;
	  bool result = doTacticsTest(fen, expected, moveString);
          if (result) {
            std::cout << "SUCCESS: ";
          }
          else {
            std::cout << "FAILURE: ";
          }
          std::cout << "expected: " << expected << ", received: " << moveString << endl;
  }

  return 0;
}
