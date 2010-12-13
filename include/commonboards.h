#ifndef COMMONBOARDS_H_
#define COMMONBOARDS_H_

#include "common.h"

// the chess bitboards common to all chess boards
// knight moves
// king moves
// white pawn moves
// white pawn attacks
// black pawn moves
// black pawn attacks

extern BITBOARD knightMoves[64];
extern BITBOARD kingMoves[64];

// white and black pawns have different moves and attacks
extern BITBOARD pawnMovesWhite[64];
extern BITBOARD pawnMovesBlack[64];
extern BITBOARD pawnAttacksWhite[64];
extern BITBOARD pawnAttacksBlack[64];

extern BITBOARD plus1[64];
extern BITBOARD plus7[64];
extern BITBOARD plus8[64];
extern BITBOARD plus9[64];
extern BITBOARD minus1[64];
extern BITBOARD minus7[64];
extern BITBOARD minus8[64];
extern BITBOARD minus9[64];

// direction from offset to offset
extern short direction[64][64]; 

// and rotated bitboards for queens/rooks/bishops
extern BITBOARD rankAttacks[64][64];
extern BITBOARD fileAttacks[64][64];
extern BITBOARD diag45RAttacks[64][64];
extern BITBOARD diag45LAttacks[64][64];

// has a different index for each possible piece that can be at a possible location
extern BITBOARD zobristArray[12][64];
extern BITBOARD zobristWhiteToMove;
extern BITBOARD zobristWhiteCanCastleKingside;
extern BITBOARD zobristWhiteCanCastleQueenside;
extern BITBOARD zobristBlackCanCastleKingside;
extern BITBOARD zobristBlackCanCastleQueenside;
extern BITBOARD zobristEnPassantSquare;

extern BITBOARD mask[64];
extern BITBOARD whitePassedPawnMask[64];
extern BITBOARD blackPassedPawnMask[64];
extern BITBOARD centerSquares;

// ROTATED BITBOARDS

extern short rotate90R[64];
extern short rotate45R[64];
extern short rotate45L[64];
extern short shiftRank[64];
extern short shiftFile[64];
extern short shift45R[64];
extern short shift45L[64];

void initialize_common_boards();
void initializeZobristArray();
void initializeMask();
void initializeFirstOne();
void initializeKingMoves();
void initializeKnightMoves();
void initializePawnMoves();
void initializeDirectionMasks();
void initializeDirection();
void initializeFileAttacks();
void initializeRankAttacks();
void initializeDiag45RAttacks();
void initializeDiag45LAttacks();

void initializePassedPawnMask();
void initializeDoubledPawnMask();
#endif /*COMMONBOARDS_H_*/
