//============================================================================
// Name        : board.h
// Author      : Dave King
// Version     :
// Copyright   : GNU Lesser General Public License
// Description : Basic chessboard structure and functions that operate on it.
//============================================================================

#ifndef BOARD_H_
#define BOARD_H_

#include "common.h"
#include "commonboards.h"
#include <string>

struct EvalInfo;

typedef struct MiscBoardInfo { 
	bool whiteCanCastleKingside;
	bool blackCanCastleKingside;
	bool whiteCanCastleQueenside;
	bool blackCanCastleQueenside;
	short enPassantOffset;
	short gamePhase;
} MiscBoardInfo;

typedef struct ChessBoard {
	
	// BITBOARDS
	BITBOARD whitePawns;
	BITBOARD whiteKnights;
	BITBOARD whiteRooks;
	BITBOARD whiteBishops;
	BITBOARD whiteKings;
	BITBOARD whiteQueens;
	
	BITBOARD blackPawns;
	BITBOARD blackKnights;
	BITBOARD blackRooks;
	BITBOARD blackBishops;
	BITBOARD blackKings;
	BITBOARD blackQueens;
	
	BITBOARD whitePieces;
	BITBOARD blackPieces;
	
	// ROTATED BITBOARDS
	BITBOARD occupied;
	BITBOARD occupied90R;
	BITBOARD occupied45R;
	BITBOARD occupied45L;
	
	// OTHER INFORMATION

	bool whiteToMove;

	bool whiteCanCastleKingside;
	bool whiteCanCastleQueenside;
	bool blackCanCastleKingside;
	bool blackCanCastleQueenside;

	bool whiteHasCastled;
	bool blackHasCastled;
	
	short gamePhase;

	int halfmoveClock;
	int fullmoveClock;

	short enPassantOffset;
	
	// BOARD ARRAY
	short boardContents[64];
	
	// MEMOIZATION FIELDS
	short whiteKingOffset;
	short blackKingOffset;
	bool whiteInCheck;
	bool blackInCheck;
	
	// HASH KEY
	HASHKEY zobristHashKey;

        // MOVE HISTORY
	int moveHistory[MAX_MOVES];
        HASHKEY boardHashes[MAX_MOVES];
	int moveIndex;

	
	bool whiteCheckHistory[MAX_MOVES];
	bool blackCheckHistory[MAX_MOVES];
	MiscBoardInfo historyBoardInfo[MAX_MOVES];
	
	int moveList[MAX_DEPTH][MAX_BRANCH];
	int moveSortList[MAX_DEPTH][MAX_BRANCH];
	int checkmateTestMoves[MAX_BRANCH];

	// store the killer moves for each depth
	int killerMoves[MAX_DEPTH];
	int secondaryKillerMoves[MAX_DEPTH];
	int searchPhase[MAX_DEPTH];
	int moveNum[MAX_DEPTH];
	int* lastMove[MAX_DEPTH];
	
	int currentSearchDepth;

	EvalInfo* eval;

} ChessBoard;

extern ChessBoard defaultStartingBoard;

int initialize_board(ChessBoard * board);
std::string board_to_string(ChessBoard * board);
void board_to_string_internal(ChessBoard * board, std::string& boardString, std::string& fenString);
bool loadBoardFromFEN(ChessBoard * board, std::string fenString);
std::string boardToFEN(ChessBoard * board);

void set_board_contents(ChessBoard * board, short offset, short contents);
void setWhiteCanCastleKingside(ChessBoard * board, bool b);
void setWhiteCanCastleQueenside(ChessBoard * board, bool b);
void setBlackCanCastleKingside(ChessBoard * board, bool b);
void setBlackCanCastleQueenside(ChessBoard * board, bool b);

void initialize_pieces(ChessBoard * board);
void update_bitboards(ChessBoard * board);
void initialize_bitboards(ChessBoard * board);
void compute_checks(ChessBoard * board);
void setGamePhase(ChessBoard * board);

bool isKingInCheck(ChessBoard* board, short king);
bool isKingInCheckImpl(ChessBoard * board, short king);

BITBOARD get_file_attacks(ChessBoard* board, short offset);
BITBOARD get_rank_attacks(ChessBoard* board, short offset);
BITBOARD get_diag45L_attacks(ChessBoard* board, short offset);
BITBOARD get_diag45R_attacks(ChessBoard* board, short offset);

// gets the bishop attack board from the specified offset
BITBOARD get_queen_attack_board(ChessBoard* board, short offset);
// gets the bishop attack board from the specified offset
BITBOARD get_bishop_attack_board(ChessBoard* board, short offset);
// gets the rook attack board from the specified offset
BITBOARD get_rook_attack_board(ChessBoard* board, short offset);
// contains a 1 at each space where a piece is attacking/defending the specified offset
BITBOARD get_attacksto_bitboard(ChessBoard* board, short offset);
BITBOARD get_white_attacksto_bitboard(ChessBoard* board, short offset);
BITBOARD get_black_attacksto_bitboard(ChessBoard* board, short offset);

// TEST FOR CHECKMATE

int getGameResult(ChessBoard* board);
bool sideToMoveIsInCheck(ChessBoard* board);

// DEBUGGING FUNCTIONS

bool internalConsistencyCheck(ChessBoard* board, ChessBoard* backBoard = NULL);
bool boardsEqual(ChessBoard* board1, ChessBoard* board2);
int copy_board(ChessBoard * dest, ChessBoard * src);
int getZobristOffsetForContents(short contents);

#endif /*BOARD_H_*/
