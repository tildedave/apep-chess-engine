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

int main(int argc, char** argv) {
  po::options_description desc("Allowed options");
  std::string fenFile;
  desc.add_options()
    ("help", "produce help message")
    ("tactics", "run tactics test")
    ("tacticsfile", "run tactics test from a file")
    ("random", po::value<int>(), "Random seed")
    ("file", po::value<std::string>(), "Read EPD file for perft/tactics file")
    ("book", po::value<std::string>(), "Read book from specified file")
    ("fen", po::value<std::string>(), "Specify FEN on command line")
    ("timeout", po::value<int>()->default_value(10), "Seconds per move (default value of 10 seconds/move)")
    ("expected", po::value<std::string>(), "Expected solution for FEN");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);    

  initialize_common_boards();
  setupLogging();

  if (argc >= 2 && std::string(argv[1]) == "-random") {
    randomSeed = atoi(argv[2]);
  }
  initialize_common_boards();
  loadOpeningBook("bookl.dat");
  
  xboardMainLoop();
  
  return 0;
}
