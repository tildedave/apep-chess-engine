#ifndef XBOARD_H_
#define XBOARD_H_

#include "common.h"
#include <list>

extern ChessBoard xboardBoard;
extern std::list<std::string> analysisMessages;

void xboardMainLoop();
void doXboardMove(ChessBoard * chessBoard, bool whiteToMove);
void sendBoardInformation(ChessBoard * chessBoard);
void searchForMove(ChessBoard * board, bool whiteToMove);
void analyzeBoard(ChessBoard * board, bool whiteToMove);
std::string MoveToXboardString(ChessBoard * board, int move);
int CoordStringToMove(ChessBoard * board, const std::string& str);
bool isMoveString(const std::string& str);
void setTimeoutValue(ChessBoard * board);

#endif /*XBOARD_H_*/
