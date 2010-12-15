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
#include <boost/program_options.hpp>

#include "commonboards.h"
#include "board.h"
#include "movepriorityqueue.h"
#include "movegen.h"
#include "moveprocess.h"
#include "move.h"
#include "search.h"
#include "xboard.h"
#include "openbook.h"
#include "tactics.h"

using namespace std;

namespace po = boost::program_options;

int randomSeed = -1;

int outputUsage(const po::options_description& desc) {
  std::cout << desc << std::endl;
  return 1;
}

int main(int argc, char** argv) {
  po::options_description desc("Allowed options");
  std::string fenFile;
  desc.add_options()
    ("help", "produce help message")
    ("tactics", "run tactics test")
    ("random", po::value<int>(), "Random seed")
    ("file", po::value<std::string>(), "Read EPD file for perft/tactics file")
    ("book", po::value<std::string>()->default_value("bookl.dat"), "Read book from specified file")
    ("fen", po::value<std::string>(), "Specify FEN on command line")
    ("timeout", po::value<int>()->default_value(10), "Seconds per move (default value of 10 seconds/move)")
    ("expected", po::value<std::string>()->default_value(""), "Expected solution for FEN");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);    

  if (vm.count("help")) {
    return outputUsage(desc);
  }

  if (vm.count("random")) {
    randomSeed = vm["random"].as<int>();
  }

  initialize_common_boards();
  setupLogging();

  if (vm.count("tactics")) {
    if (!vm.count("file")) {
      if (!vm.count("fen")) {
	return outputUsage(desc);
      }

      std::cerr << "tactics time" << std::endl;

      std::string fen = vm["fen"].as<std::string>();
      std::string expected = vm["expected"].as<std::string>();

      TacticsModule tm = TacticsModule(fen, expected);
      tm.run();
      return 0;
    }
  }


  loadOpeningBook(vm["book"].as<std::string>());
  
  xboardMainLoop();
  
  return 0;
}
