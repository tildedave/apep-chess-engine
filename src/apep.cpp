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

// MODULES
#include "eval.h"
#include "tactics.h"
#include "perft.h"

using namespace std;

namespace po = boost::program_options;

int randomSeed = -1;

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

  po::options_description eval("Evaluation Options");
  eval.add_options()
    ("eval", "run board evaluation");

  bool verbose;
  bool divide;
  int depth;

  po::options_description perft("Perft Options");
  perft.add_options()
    ("perft", "run perft test")
    ("verbose", po::bool_switch(&verbose)->default_value(false), 
                "Output time for move generation, processing, and unprocessing")
    ("depth", po::value<int>(&depth)->default_value(5), "Perft depth")
    ("divide", po::bool_switch(&divide)->default_value(false), "Output divide for each move?");


  po::options_description cmdline_options;
  cmdline_options.add(generic).add(shared).add(tactics).add(perft).add(eval);

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

  int timeout = vm["timeout"].as<int>();

  if (vm.count("tactics")) {
    setupLogging("apep.tactics.log");
    if (!vm.count("file")) {
      if (!vm.count("fen")) {
	return outputUsage(cmdline_options);
      }

      std::string fen = vm["fen"].as<std::string>();
      std::string expected = vm["expected"].as<std::string>();

      TacticsModule tm(fen, expected, timeout);
      tm.run();
      return 0;
    }

    if (vm.count("file")) {
      std::string file = vm["file"].as<std::string>();
      TacticsFileModule tm(file, timeout);
      tm.run();

      return 0;
    }
  }

  if (vm.count("perft")) {
    std::string startingFen;
    if (!vm.count("fen")) {
      startingFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    }
    else {
      startingFen = vm["fen"].as<std::string>();
    }

    setupLogging("apep.perft.log");
    PerftModule pm(startingFen, depth, verbose, divide);
    pm.run();

    return 0;
  }

  if (vm.count("eval")) {
    if (!vm.count("fen")) {
      return outputUsage(cmdline_options);
    }

    std::string fenString = vm["fen"].as<std::string>();
    setupLogging("apep.eval.log");
    EvalModule em(fenString);
    em.run();

    return 0;
  }

  setupLogging();
  loadOpeningBook(vm["book"].as<std::string>());
  XboardModule xm;
  xm.run();
  
  return 0;
}
