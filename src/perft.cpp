#ifndef WIN32
#include <sys/time.h>
#endif
#include <string>
#include <iostream>
#include <iomanip>
#include <boost/program_options.hpp>

#include "move.h"
#include "moveprocess.h"
#include "board.h"
#include "perft.h"
#include "movegen.h"

namespace po = boost::program_options;

// perft has two main functions:
//  * check if move generator is working correctly: http://www.rocechess.ch/perft.html
//  * check speed of move generator

double moveGenTime;
double moveProcessTime;
double moveUnprocessTime;

PerftModule::PerftModule(const std::string& fenString, 
			 int depth, 
			 bool verbose, 
			 bool divide) : fenString_(fenString),
					depth_(depth),
					verbose_(verbose),
					divide_(divide)
{
}

void PerftModule::run()
{
  ChessBoard board;
  loadBoardFromFEN(&board, this->fenString_);

#ifdef WIN32
  SYSTEMTIME startSystemTime, endSystemTime;
  FILETIME start, end;
#else
  struct timeval start, end;
#endif

  for(int i = 0; i <= this->depth_; ++i) {
    moveGenTime = 0.0;
    moveProcessTime = 0.0;
    moveUnprocessTime = 0.0;

#ifdef WIN32
    GetSystemTime(&startSystemTime);
    SystemTimeToFileTime(&startSystemTime, &start);
#else
    gettimeofday(&start, NULL);
#endif
        
    int perftNum;
    if (verbose_) {
      perftNum = perft<true>(&board, i, i, this->divide_);
    }
    else {
      perftNum = perft<false>(&board, i, i, this->divide_);
    }

#ifdef WIN32
    GetSystemTime(&endSystemTime);
    SystemTimeToFileTime(&endSystemTime, &end);
    __int64 nanoSecondsDiff = ((__int64) (end.dwHighDateTime - 
					  start.dwHighDateTime)) << 32;
    nanoSecondsDiff += end.dwLowDateTime - start.dwLowDateTime;
    double diff = nanoSecondsDiff / (10000000.0);
#else
    gettimeofday(&end, NULL);
    double diff = (end.tv_sec - start.tv_sec);
    diff += (end.tv_usec - start.tv_usec) / (1000.0 * 1000.0);
#endif

    std::cout << std::fixed << std::setprecision(4) << i << "   " << diff << "   " << perftNum;
    if (verbose_) {
      std::cout << " (movegen: " 
		<< this->moveGenTime 
		<< ", process: " 
		<< this->moveProcessTime 
		<< ", unprocess: " 
		<< this->moveUnprocessTime << ")";
    }
    std::cout << std::endl;
  }
}
/*
int main(int argc, char** argv) {
  po::options_description desc("Allowed options");

  bool verbose;
  bool divide;
  int depth = 6;
  std::string startingFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

  desc.add_options()
    ("help", "produce help message")
    ("fen", po::value<std::string>()->default_value(startingFen), "FEN to start perft from")
    ("depth", po::value<int>(&depth)->default_value(6), "Perft depth")
    ("verbose", po::bool_switch(&verbose)->default_value(false), "Output time for move generation, processing, and unprocessing")
    ("divide", po::bool_switch(&divide)->default_value(false), "Output divide for each move?");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);    
  
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    exit(0);
  }

  std::cout << "caching common boards..." << std::endl;
  initialize_common_boards();
  std::cout << "done" << std::endl;

  std::string fenString = vm["fen"].as<std::string>();
  doPerftTest(depth, verbose, divide, fenString);
}
*/
