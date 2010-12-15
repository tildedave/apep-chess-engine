#pragma once

#include "board.h"
#include "movegen.h"
#include "module.h"

class PerftModule : public Module {

 public:
  PerftModule(const std::string& fenString, int depth, bool verbose, bool divide);
  void run();

  template <bool individualPerftTime> long perft(ChessBoard* board, int depth, int startingDepth, bool divide = false) {
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
      this->moveGenTime += (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / (1000 * 1000.0);
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
	this->moveProcessTime += (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / (1000 * 1000.0);
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
	this->moveUnprocessTime += (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / (1000 * 1000.0);
      }
    }

    return numNodes;
  }

 private:
  std::string fenString_;
  int depth_;
  bool verbose_;
  bool divide_;

  double moveGenTime;
  double moveProcessTime;
  double moveUnprocessTime;

};


