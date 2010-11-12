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
#include "perft.hpp"

namespace po = boost::program_options;

// perft has two main functions:
//  * check if move generator is working correctly: http://www.rocechess.ch/perft.html
//  * check speed of move generator

#define INDIV_PERFT_TIME 0
#define DIVIDE 0
#define NEW_PERFT 1

double moveGenTime;
double moveProcessTime;
double moveUnprocessTime;

int randomSeed = -1;

#include "board.h"
#include "movegen.h"

template <bool individualPerftTime>
long perft(ChessBoard* board, int depth, int startingDepth, bool divide = false) {
  if (depth == 0)
    return 1;

  long numNodes = 0;
  timeval start, end;

  if (individualPerftTime) {
    gettimeofday(&start, NULL);
  }

  int moves[128];
  int* startMoves = moves;
  int* endMoves = generateCaptures(board, board->whiteToMove, startMoves);
  endMoves = generateNonCaptures(board, board->whiteToMove, endMoves);	
	
  if (individualPerftTime) {
    gettimeofday(&end, NULL);
    moveGenTime += (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / (1000 * 1000.0);
  }

  for(int* currentMove = startMoves; currentMove < endMoves; ++currentMove) {
    int cm = *currentMove;
    short kingToCheck = board->whiteToMove ? KING_WHITE : KING_BLACK;

    if (individualPerftTime) {
      gettimeofday(&start, NULL);
    }
    processMove(board, cm);
    if (individualPerftTime) {
      gettimeofday(&end, NULL);
      moveProcessTime += (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / (1000 * 1000.0);
    }

    if (!isKingInCheck(board, kingToCheck)) {
      BITBOARD inc;
      if (depth == 1)
        inc = 1; 
      else {
        if (individualPerftTime) {
          inc = perft<true>(board, depth - 1, startingDepth, divide);
        }
        else {
          inc = perft<false>(board, depth - 1, startingDepth, divide);
        }
      }
                  
      if (divide) {
        if (depth == startingDepth) {
          cerr << offset_to_string(GetFrom(cm)) << offset_to_string(GetTo(cm)) << " " << inc << endl;
        }
      }
      numNodes += inc;
    }
    if (individualPerftTime) {
      gettimeofday(&start, NULL);
    }
    unprocessMove(board, cm);
    if (individualPerftTime) {
      gettimeofday(&end, NULL);
      moveUnprocessTime += (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / (1000 * 1000.0);
    }
  }

  return numNodes;
}

void doPerftTest(int depth, bool individualPerftTime, bool divide, 
                 std::string fenString = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
{
  ChessBoard board;
  loadBoardFromFEN(&board, fenString);
#ifdef WIN32
  SYSTEMTIME startSystemTime, endSystemTime;
  FILETIME start, end;
#else
  struct timeval start, end;
#endif
  for(int i = 0; i <= depth; ++i) {
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
    if (individualPerftTime) {
      perftNum = perft<true>(&board, i, i, divide);
    }
    else {
      perftNum = perft<false>(&board, i, i, divide);
    }
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
    std::cout << std::fixed << std::setprecision(4) << diff << "\t" << perftNum;
    if (verbose) {
      std::cout << " (movegen: " << moveGenTime << ", process: " << moveProcessTime << ", unprocess: " << moveUnprocessTime << ")";
    }
    std::endl;
  }
}

int main(int argc, char** argv) {
  po::options_description desc("Allowed options");

  bool verbose;
  bool divide;
  int depth = 6;

  desc.add_options()
    ("help", "produce help message")
    ("fen", po::value<std::string>(), "FEN to start perft from")
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

  doPerftTest(depth, verbose, divide);
}
