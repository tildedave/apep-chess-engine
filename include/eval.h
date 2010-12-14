#ifndef EVAL_H_
#define EVAL_H_

#include "common.h"
#include "board.h"
#include <ostream>

#define PAWN_MATERIAL_SCORE		100
#define BISHOP_MATERIAL_SCORE 	300
#define KNIGHT_MATERIAL_SCORE 	300
#define ROOK_MATERIAL_SCORE 	500
#define QUEEN_MATERIAL_SCORE 	900

#define CHECKMATE 				15000
#define IsCheckmate(x)			(x > 14000 || x < -14000)

#define PRIORITY_PAWN_TO_QUEEN_PROMOTION 	100
#define PRIORITY_HASH_MOVE					90
#define PRIORITY_KILLER_MOVE				89
#define PRIORITY_MATERIAL_WINNING_CAPTURE   50
#define PRIORITY_CAPTURE					49
#define PRIORITY_CHECK						48
#define PRIORITY_NORMAL_MOVE				40
#define PRIORITY_UNDERPROMOTION 			10


class EvalParameters {

public:
	static int endgameKingPositionalBonusScale[64];
	static int endgameCentralizedKingBonus;
	
	static int middlegameKingPositionalBonusScale[64];
	static int middlegameKingCornerBonus;
	static int noPawnCoverBonus;
	static int cannotCastleBonus;

	static int knightPositionalBonusScale[64];
	static int knightPositionalBonus;

	static int knight_onOriginalSquareBonus;
	static int bishop_onOriginalSquareBonus;
	static int rook_onOriginalSquareBonus;

	static int whitePassedPawnRankScale[8];
	static int blackPassedPawnRankScale[8];
	static int passedPawnBonus;
	static int doubledPawnPenalty;
	
	static int whiteCenterControlScale[64];
	static int blackCenterControlScale[64];
	static int centerControlBonus;

	static int PawnScore;
	static int PawnSquareValue[64];

	static int KnightScore;
	static int KnightSquareValue[64];

	static int BishopScore;
	static int BishopSquareValue[64];

	static int RookScore;
	static int RookSquareValue[64];

	static int QueenScore;
	static int QueenSquareValue[64];

	static int KingScore;
	static int KingSquareValue_Endgame[64];
	static int KingSquareValue_Middlegame[64];

};

struct EvalInfo {
	int rawWhiteMaterialScore;
	int rawBlackMaterialScore;
	int pieceWhiteMaterialScore;
	int pieceBlackMaterialScore;

	int whitePawns[8];
	int whitePawnsNum;
	int whiteKnights[8];
	int whiteKnightsNum;
	int whiteBishops[8];
	int whiteBishopsNum;
	int whiteRooks[8];
	int whiteRooksNum;
	int whiteQueens[8];
	int whiteQueensNum;

	int blackPawns[8];
	int blackPawnsNum;
	int blackKnights[8];
	int blackKnightsNum;
	int blackBishops[8];
	int blackBishopsNum;
	int blackRooks[8];
	int blackRooksNum;
	int blackQueens[8];
	int blackQueensNum;
};

//int do_evaluateBoard(ChessBoard * board);
template<bool verbose> int evaluateBoard(ChessBoard * board, std::ostream* os = 0);
template<bool verbose> int do_evaluateBoard(ChessBoard * board, std::ostream* os = 0);
template<bool verbose> int getPositionScore(ChessBoard * board, std::ostream* os = 0);

void preprocessEvalInformation(ChessBoard * board);
int getMaterialScore(ChessBoard * board);
int getElementScore(short contents, short offset);
int getWhiteMaterialScore(ChessBoard* board);
int getBlackMaterialScore(ChessBoard* board);

int getPassedPawnScore(ChessBoard * board, bool white);
int getKingPlacementScore(ChessBoard * board, bool white);
int getKingAttackScore(ChessBoard * board, bool white);
int getDevelopmentScore(ChessBoard * board, bool white);
int getCenterControlScore(ChessBoard * board, bool white);
int getDoubledPawnPenalty(ChessBoard* board, bool white);

bool checkForRepetition(ChessBoard* board);

#include "eval.hpp"

#endif /*EVAL_H_*/
