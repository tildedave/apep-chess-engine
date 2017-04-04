#ifndef MOVE_H_
#define MOVE_H_

#include "common.h"
#include "board.h"

// NEW APPROACH:
// (1) PAWN
// (2) KNIGHT
// (3) KING
// (5) BISHOP
// (6) ROOK
// (7) QUEEN
// can determine the white or blackness of a piece 
// from the to-move in the chessboard

// 3 BITS: MOVE PIECE
// 6 BITS: FROM OFFSET
// 6 BITS: TO OFFSET
// 3 BITS: CAPTURE PIECE
// 3 BITS: PROMOTION PIECE (eh)

// TODO: implement this!

#define MakeMove(piece,from,to) 	 			((piece) | ((from) << 3) | ((to) << 9))
#define MakeCapture(piece,from,to,cap)  		((piece) | ((from) << 3) | ((to) << 9) | ((cap) << 15))	 
#define MakePromotion(piece,from,to,cap,pro)  	((piece) | ((from) << 3) | ((to) << 9) | ((cap) << 15) | ((pro) << 19))
#define GetMovePiece(move)						(move & 7) 
#define GetCapturePiece(move)					((move >> 15) & 7) 
#define GetPromotionPiece(move)					((move >> 19) & 7) 
#define GetFrom(move)							((move >> 3) & 63)
#define GetTo(move)								((move >> 9) & 63)
#define IsKingsideCastleMove(move)				(GetMovePiece(move) == KING && (GetFrom(move) == GetTo(move) - 2))
#define IsQueensideCastleMove(move)				(GetMovePiece(move) == KING && (GetFrom(move) == GetTo(move) + 2))

const std::string MoveToString(int move);
bool MoveIsLegal(ChessBoard * chessBoard, int currentMove);

std::string piece_to_notation_string(short contents, short offset, bool isCapture);

#endif /*MOVE_H_*/
