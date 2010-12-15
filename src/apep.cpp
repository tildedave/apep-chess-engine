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
extern float TimeoutValue;

int outputUsage(const po::options_description& desc) {
  std::cout << desc << std::endl;
  return 1;
}

int main(int argc, char** argv) {
  std::string fenFile;

  po::options_description generic("General Options");
  generic.add_options()
    ("help", "produce help message")
    ("random", po::value<int>(), "Random seed")
    ("book", po::value<std::string>()->default_value("bookl.dat"), "Read book from specified file")
    ("expected", po::value<std::string>()->default_value(""), "Expected solution for FEN");

  po::options_description shared("Shared Options");
  shared.add_options()
    ("fen", po::value<std::string>(), "Specify FEN on command line")
    ("file", po::value<std::string>(), "Read EPD file for perft/tactics file");

  po::options_description tactics("Tactics Options");
  tactics.add_options()
    ("tactics", "run tactics test")
    ("timeout", po::value<int>()->default_value(10), 
                "Seconds per move (default value of 10 seconds/move)");

  bool verbose;
  bool divide;

  po::options_description perft("Perft Options");
  perft.add_options()
    ("perft", "run perft test")
    ("verbose", po::bool_switch(&verbose)->default_value(false), 
                "Output time for move generation, processing, and unprocessing")
    ("divide", po::bool_switch(&divide)->default_value(false), "Output divide for each move?");


  po::options_description cmdline_options;
  cmdline_options.add(generic).add(shared).add(tactics).add(perft);

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, cmdline_options), vm);
  po::notify(vm);    

  if (vm.count("help")) {
    return outputUsage(cmdline_options);
  }

  if (vm.count("random")) {
    randomSeed = vm["random"].as<int>();
  }

  initialize_common_boards();
  setupLogging();
  int timeout = vm["timeout"].as<int>();

  if (vm.count("tactics")) {
    std::cerr << "tactics time" << std::endl;
    TimeoutValue = timeout;

    if (!vm.count("file")) {
      if (!vm.count("fen")) {
	return outputUsage(cmdline_options);
      }

      std::string fen = vm["fen"].as<std::string>();
      std::string expected = vm["expected"].as<std::string>();

      TacticsModule tm = TacticsModule(fen, expected, timeout);
      tm.run();
      return 0;
    }

    std::cerr << vm.count("file") << std::endl;
    if (vm.count("file")) {
      std::string file = vm["file"].as<std::string>();
      TacticsFileModule tm = TacticsFileModule(file, timeout);
      tm.run();

      return 0;
    }
  }


  loadOpeningBook(vm["book"].as<std::string>());
  
  xboardMainLoop();
  
  return 0;
}
