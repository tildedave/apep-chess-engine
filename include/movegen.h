#ifndef MOVEGEN_H_
#define MOVEGEN_H_

#include "board.h"
#include "movepriorityqueue.h"
#include "movelinkedlist.h"

int* generateCaptures(ChessBoard* chessBoard, bool white, int* moves);
int* generateNonCaptures(ChessBoard* chessBoard, bool white, int *moves);
int* generateChecks(ChessBoard* chessBoard, bool white, int *destMoves);
int* getQuiescentMoves(ChessBoard * chessBoard, bool white, int* startMoves);

short getPriorityForMove_MVVLVA(ChessBoard * chessBoard, int move);
short getPriorityForMove_SEE(ChessBoard * chessBoard, int move, bool white);
short getStaticExchangeEvaluation(ChessBoard * chessBoard, short offset);
short getSmallestAttacker(ChessBoard * board, BITBOARD attackers, short offset, bool whiteToMove);

#endif /*MOVEGEN_H_*/
