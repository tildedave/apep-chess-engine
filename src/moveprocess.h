#ifndef MOVEPROCESS_H_
#define MOVEPROCESS_H_

#include "common.h"
#include "board.h"

void processMove(ChessBoard * chessBoard, int move);
void unprocessMove(ChessBoard * chessBoard, int move);
void sanityCheckMove(ChessBoard * chessBoard, int move);

void processNullMove(ChessBoard * chessBoard);
void unprocessNullMove(ChessBoard * chessBoard);

void do_processMove(ChessBoard * chessBoard, short item, short fromOffset, short toOffset, short capturedPiece, bool whiteToMove);
void do_processCapture(ChessBoard * chessBoard, short capturingItem, int fromOffset, short toOffset, short capturedPiece, bool whiteToMove);
void do_processCapture(ChessBoard * chessBoard, short capturingItem, int fromOffset, short toOffset, short capturedPiece, bool whiteToMove);
void do_unprocessCapture(ChessBoard * chessBoard, short capturingItem, int fromOffset, short toOffset, short capturedPiece, bool whiteToMove);
void do_unprocessPromotion(ChessBoard * chessBoard, short replacePiece, short movePiece, short from, short to, bool whiteToMove);
void do_processPromotion(ChessBoard * chessBoard, short replacePiece, short to, short movePiece, bool whiteToMove);
void do_unprocessQueensideCastle(ChessBoard * chessBoard, short movePiece, bool whiteToMove);
void do_unprocessKingsideCastle(ChessBoard * chessBoard, short movePiece, bool whiteToMove);
void do_processKingsideCastle(ChessBoard * chessBoard, short movePiece, bool whiteToMove);
void do_processQueensideCastle(ChessBoard * chessBoard, short movePiece, bool whiteToMove);

void toggleWhiteToMove(ChessBoard * chessBoard);
//void revertCastlingPrivileges(ChessBoard * chessBoard, int move);
void revertCastlingPrivileges(ChessBoard * chessBoard, MiscBoardInfo * miscBoardInfo);
void revertEnPassantSquare(ChessBoard * chessBoard, int move);
void updateCastlingPrivileges(ChessBoard * chessBoard, int move);
void do_updateCastlingPrivileges(ChessBoard * chessBoard, short movePiece, short from);
short getPieceFromPromotion(short type, short movePiece);

#endif /*MOVEPROCESS_H_*/
