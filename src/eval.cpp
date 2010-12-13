#include "eval.h"

EvalParameters evalParameters;
int
EvalParameters::endgameKingPositionalBonusScale[64] = {
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, 0,  1,  1,  1,  1,  0,  -1,
		-1, 0,  2,  2,  2,  2,  0,  -1,
		-1, 1,  2,  3,  3,  2,  1,  -1,
		-1, 1,  2,  3,  3,  2,  1,  -1,
		-1, 0,  2,  2,  2,  2,  0,  -1,
		-1, 0,  1,  1,  1,  1,  0,  -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
};
int 
EvalParameters::endgameCentralizedKingBonus = 15;

int
EvalParameters::middlegameKingPositionalBonusScale[64] = {
		1, 2, 1, -1, -1, -1, 2, 1,
		1, 1, -1, -2, -2, -2, 1, 1, 
		-2, -2, -2, -2, -2, -2, -2, -2, 
		-2, -2, -2, -2, -2, -2, -2, -2, 
		-2, -2, -2, -2, -2, -2, -2, -2, 
		-2, -2, -2, -2, -2, -2, -2, -2, 
		1, 1, -1, -2, -2, -2, 1, 1, 
		1, 2, 1, -1, -1, -1, 2, 1
};

int EvalParameters::middlegameKingCornerBonus = 15;
int EvalParameters::noPawnCoverBonus = -10;
int EvalParameters::cannotCastleBonus= -50;

int EvalParameters::whitePassedPawnRankScale[8] = { 0, 1, 1, 1, 2, 3, 8, 0 };
int EvalParameters::blackPassedPawnRankScale[8] = { 0, 8, 3, 2, 1, 1, 1, 0 };
int EvalParameters::passedPawnBonus = 50;
int EvalParameters::doubledPawnPenalty = 60;

int EvalParameters::knight_onOriginalSquareBonus = -10;
int EvalParameters::bishop_onOriginalSquareBonus = -8;
int EvalParameters::rook_onOriginalSquareBonus = -2;

int EvalParameters::whiteCenterControlScale[64] =
{ 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 4, 4, 4, 4, 0, 0, 
  0, 0, 3, 3, 3, 3, 0, 0, 
  0, 0, 2, 2, 2, 2, 0, 0, 
  0, 0, 1, 1, 1, 1, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

int EvalParameters::blackCenterControlScale[64] =
{ 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 1, 1, 1, 1, 0, 0, 
  0, 0, 2, 2, 2, 2, 0, 0, 
  0, 0, 3, 3, 3, 3, 0, 0, 
  0, 0, 4, 4, 4, 4, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

int EvalParameters::PawnScore = 100;
int EvalParameters::BishopScore = 325;
int EvalParameters::KnightScore = 325;
int EvalParameters::RookScore = 500;
int EvalParameters::QueenScore = 900;

int EvalParameters::PawnSquareValue[64] = 
{
	0,   0,  0,  0,  0,  0,  0,  0,
	15, 30, 30, 60, 60, 30, 30, 15,
	10, 20, 20, 50, 50, 20, 20, 10,
	0, 10, 10, 40, 40, 10, 10, 0,
	0, 5, 5, 30, 30, 5, 5, 0,
	0, 0, 0, 15, 15, 0, 0, 0,
	-5, -5, -5, -5, -5, -5, -5, 0,
	0,   0,  0,  0,  0,  0,  0,  0
};

// TODO: give knight a bonus for being on an output/hole
int EvalParameters::KnightSquareValue[64] = 
{
	5,  10,  10,  10,  10,  10,  10,   5,
	15, 30,  30,  30,  30,  30,  30,  15,
	20, 35,  50,  70,  70,  50,  35,  20,
	10, 20,  20,  50,  50,  20,  20,  10,
	 5, 20,  30,  40,  40,  30,  20,   5,
	 0, 10,  20,  30,  30,  20,  10,   0,
    -5, -5,  -5,   0,   0,  -5,  -5,  -5,
   -10, -10, -10, -10, -10, -10, -10, -10
};

// TODO: give bishop a bonus for activity
int EvalParameters::BishopSquareValue[64] = 
{
	 10, 10,  10,  10,  10,  10,  10,   10,
	 10, 10,  10,  10,  10,  10,  10,   10,
	 10, 10,  10,  10,  10,  10,  10,   10,
	 10, 10,  10,  10,  10,  10,  10,   10,
	 10, 10,  10,  10,  10,  10,  10,   10,
	 10, 10,  10,  10,  10,  10,  10,   10,
     5,  10,   5,   5,   5,   5,  10,    5,
     5,   0, -10, -10, -10, -10,   0,    5
};

// TODO: give rook bonus for being on an open file
int EvalParameters::RookSquareValue[64] = 
{
	10,  10,  10,  10,  10,  10,  10,  10,	
	50,  50,  50,  50,  50,  50,  50,  50,
	10,  10,  10,  10,  10,  10,  10,  10,	
	10,  10,  10,  10,  10,  10,  10,  10,	
	10,  10,  10,  10,  10,  10,  10,  10,	
	10,  10,  10,  10,  10,  10,  10,  10,	
	10,  10,  10,  10,  10,  10,  10,  10,	
	-5,   5,   5,   5,   5,   5,   5,  -5
};

// TODO: figure out what to do here
int EvalParameters::QueenSquareValue[64] = 
{
	10,  10,  10,  10,  10,  10,  10,  10,	
	20,  20,  20,  20,  20,  20,  20,  20,
	10,  10,  10,  10,  10,  10,  10,  10,	
	10,  10,  10,  10,  10,  10,  10,  10,	
	10,  10,  10,  10,  10,  10,  10,  10,	
	10,  10,  10,  15,  10,  10,  10,  10,	
	10,  10,  10,  15,  15,  10,  10,  10,	
	-5,   -5,   -5,  -5,  -5,   -5,   -5,  -5
};

int EvalParameters::KingSquareValue_Middlegame[64] = 
{
	-50, -50, -50, -50, -50, -50, -50, -50, 
	-50, -50, -50, -50, -50, -50, -50, -50, 
	-50, -50, -50, -50, -50, -50, -50, -50, 
	-50, -50, -50, -50, -50, -50, -50, -50, 
	-50, -50, -50, -50, -50, -50, -50, -50, 
	-30, -30, -30, -30, -30, -30, -30, -30, 
	5, 10, -10, -20, -20, -10, 10, 5, 
	10, 20, 10, -20, -20, 10, 20, 10
};

int EvalParameters::KingSquareValue_Endgame[64] = 
{
	-50, -30, -20, -20, -20, -20, -30, -50,
	-30, -20, -10, -10, -10, -10, -20, -30,
	-20, -10,   0,   5,  5,   0, -10, -20,
	-15,  -5,  10,  15,  15,  10,   5, -15,
	-10,   0,  20,  30,  30,  20,   0, -10,
	-15,  -5,  10,  15,  15,  10,   5, -15,
	-30, -20, -10, -10, -10, -10, -20, -30,
	-50, -30, -20, -20, -20, -20, -30, -50,
};

void preprocessEvalInformation(ChessBoard * board) {
	// store the positions of the chess pieces into the eval info
	BITBOARD occupied = board->occupied;
	while (occupied != 0) {
		short nextOffset = FirstOne(occupied);
		occupied ^= offset_to_mask(nextOffset);

		switch (board->boardContents[nextOffset]) {
			case PAWN_WHITE:
				board->eval->whitePawns[board->eval->whitePawnsNum] = nextOffset;
				++board->eval->whitePawnsNum;
				break;
			case KNIGHT_WHITE:
				board->eval->whiteKnights[board->eval->whiteKnightsNum] = nextOffset;
				++board->eval->whiteKnightsNum;
				break;
			case BISHOP_WHITE:
				board->eval->whiteBishops[board->eval->whiteBishopsNum] = nextOffset;
				++board->eval->whiteBishopsNum;
				break;
			case ROOK_WHITE:
				board->eval->whiteRooks[board->eval->whiteRooksNum] = nextOffset;
				++board->eval->whiteRooksNum;
				break;
			case QUEEN_WHITE:
				board->eval->whiteQueens[board->eval->whiteQueensNum] = nextOffset;
				++board->eval->whiteQueensNum;
				break;
			case PAWN_BLACK:
				board->eval->blackPawns[board->eval->blackPawnsNum] = nextOffset;
				++board->eval->blackPawnsNum;
				break;
			case KNIGHT_BLACK:
				board->eval->blackKnights[board->eval->blackKnightsNum] = nextOffset;
				++board->eval->blackKnightsNum;
				break;
			case BISHOP_BLACK:
				board->eval->blackBishops[board->eval->blackBishopsNum] = nextOffset;
				++board->eval->blackBishopsNum;
				break;
			case ROOK_BLACK:
				board->eval->blackRooks[board->eval->blackRooksNum] = nextOffset;
				++board->eval->blackRooksNum;
				break;
			case QUEEN_BLACK:
				board->eval->blackQueens[board->eval->blackQueensNum] = nextOffset;
				++board->eval->blackQueensNum;
				break;
		}
	}

	board->eval->pieceWhiteMaterialScore = 
		board->eval->whiteKnightsNum * EvalParameters::KnightScore + 
		board->eval->whiteBishopsNum * EvalParameters::BishopScore + 
		board->eval->whiteRooksNum * EvalParameters::RookScore + 
	        board->eval->whiteQueensNum * EvalParameters::QueenScore
	  ;

	board->eval->rawWhiteMaterialScore = 
	  board->eval->pieceWhiteMaterialScore + 
	  board->eval->whitePawnsNum * EvalParameters::PawnScore
		;

	board->eval->pieceBlackMaterialScore = 
		board->eval->blackKnightsNum * EvalParameters::KnightScore + 
		board->eval->blackBishopsNum * EvalParameters::BishopScore + 
		board->eval->blackRooksNum * EvalParameters::RookScore + 
		board->eval->blackQueensNum * EvalParameters::QueenScore 
		;

	board->eval->rawBlackMaterialScore =
	  board->eval->pieceBlackMaterialScore + 
	  board->eval->blackPawnsNum * EvalParameters::PawnScore
	  ;

	setGamePhase(board);
}

int getMaterialScore(ChessBoard * board) {
	int whiteScore = getWhiteMaterialScore(board);
	int blackScore = getBlackMaterialScore(board);

	if (board->whiteToMove)
		return whiteScore - blackScore;
	else
		return blackScore - whiteScore;
}

int getWhiteMaterialScore(ChessBoard* board) {
	int whiteScore = 0;
	int i = 0;

	EvalInfo* eval = board->eval;
	for(i = 0; i < eval->whitePawnsNum; ++i) {
		whiteScore += EvalParameters::PawnScore + EvalParameters::PawnSquareValue[eval->whitePawns[i]];
	}
	for(i = 0; i < eval->whiteKnightsNum; ++i) {
		whiteScore += EvalParameters::KnightScore + EvalParameters::KnightSquareValue[eval->whiteKnights[i]];
	}
	for(i = 0; i < eval->whiteBishopsNum; ++i) {
		whiteScore += EvalParameters::BishopScore + EvalParameters::BishopSquareValue[eval->whiteBishops[i]];
	}
	for(i = 0; i < eval->whiteRooksNum; ++i) {
		whiteScore += EvalParameters::RookScore + EvalParameters::RookSquareValue[eval->whiteRooks[i]];
	}
	for(i = 0; i < eval->whiteQueensNum; ++i) {
		whiteScore += EvalParameters::QueenScore + EvalParameters::QueenSquareValue[eval->whiteQueens[i]];
	}

	if (board->gamePhase != PHASE_ENDGAME)
		whiteScore += EvalParameters::KingSquareValue_Middlegame[board->whiteKingOffset];
	else
		whiteScore += EvalParameters::KingSquareValue_Endgame[board->whiteKingOffset];

	return whiteScore;
}

int getBlackMaterialScore(ChessBoard* board) {
	int blackScore = 0;
	int i = 0;

	EvalInfo* eval = board->eval;
	for(i = 0; i < eval->blackPawnsNum; ++i) {
		blackScore += EvalParameters::PawnScore + EvalParameters::PawnSquareValue[offset_vflip(eval->blackPawns[i])];
	}
	for(i = 0; i < eval->blackKnightsNum; ++i) {
		blackScore += EvalParameters::KnightScore + EvalParameters::KnightSquareValue[offset_vflip(eval->blackKnights[i])];
	}
	for(i = 0; i < eval->blackBishopsNum; ++i) {
		blackScore += EvalParameters::BishopScore + EvalParameters::BishopSquareValue[offset_vflip(eval->blackBishops[i])];
	}
	for(i = 0; i < eval->blackRooksNum; ++i) {
		blackScore += EvalParameters::RookScore + EvalParameters::RookSquareValue[offset_vflip(eval->blackRooks[i])];
	}
	for(i = 0; i < eval->blackQueensNum; ++i) {
		blackScore += EvalParameters::QueenScore + EvalParameters::QueenSquareValue[offset_vflip(eval->blackQueens[i])];
	}
	if (board->gamePhase != PHASE_ENDGAME)
		blackScore += EvalParameters::KingSquareValue_Middlegame[offset_vflip(board->blackKingOffset)];
	else
		blackScore += EvalParameters::KingSquareValue_Endgame[offset_vflip(board->blackKingOffset)];

	return blackScore;
}

// only used by SEE.  no reason to include piece value maps IMO
int getElementScore(short contents, short offset) {
	switch(contents) {
		case PAWN_WHITE:
			return EvalParameters::PawnScore;
		case KNIGHT_WHITE:
			return EvalParameters::KnightScore;
		case BISHOP_WHITE:
			return EvalParameters::BishopScore;
		case ROOK_WHITE:
			return EvalParameters::RookScore;
		case QUEEN_WHITE:
			return EvalParameters::QueenScore;
		case PAWN_BLACK:
			return -EvalParameters::PawnScore;
		case KNIGHT_BLACK:
			return -EvalParameters::KnightScore;
		case BISHOP_BLACK:
			return -EvalParameters::BishopScore;
		case ROOK_BLACK:
			return -EvalParameters::RookScore;
		case QUEEN_BLACK:
			return -EvalParameters::QueenScore;
	}

	return 0;
}

int getKingScore(short offset) {
	// TODO: this will depend on 1) the stage of the game 2) the placement of the king 
	return 0;
}

int getDoubledPawnPenalty(ChessBoard* board, bool white) {
  BITBOARD pawns = (white) ? board->whitePawns :  board->blackPawns;
  BITBOARD originalPawns = pawns;
  BITBOARD* doubledPawnMask = (white) ? whiteDoubledPawnMask : blackDoubledPawnMask;

  int doubledPawnPenalty = 0;

  while(pawns != 0) {
    short pawnOffset = FirstOne(pawns);
    pawns ^= offset_to_mask(pawnOffset);
    
    BITBOARD maskResult = doubledPawnMask[pawnOffset] & originalPawns;

    if (maskResult != 0) {
      doubledPawnPenalty += EvalParameters::doubledPawnPenalty * NumOnes(maskResult);
    }
  }

  return doubledPawnPenalty;
}

int getPassedPawnScore(ChessBoard * board, bool white) {
	BITBOARD pawns = (white) ? board->whitePawns : board->blackPawns;
	BITBOARD originalPawns = pawns;
	BITBOARD otherPawns = (white) ? board->blackPawns : board->whitePawns;
	BITBOARD* doubledPawnMask = (white) ? whiteDoubledPawnMask : blackDoubledPawnMask;
	BITBOARD* passedPawnMask = (white) ? whitePassedPawnMask : blackPassedPawnMask;
	int* passedPawnRankScale = (white) ? EvalParameters::whitePassedPawnRankScale : EvalParameters::blackPassedPawnRankScale;

	int passedPawnScore = 0;

	while (pawns != 0) {
		short pawnOffset = FirstOne(pawns);
		pawns ^= offset_to_mask(pawnOffset);

		if ((passedPawnMask[pawnOffset] & otherPawns) == 0) {
			// pawn is passed.  give bonus

		        // check if pawn is doubled
                        BITBOARD isDoubled = doubledPawnMask[pawnOffset] & originalPawns;

			if (isDoubled == 0) {
			  if (board->gamePhase == PHASE_ENDGAME) {
			    passedPawnScore += EvalParameters::passedPawnBonus * passedPawnRankScale[8 - Rank(pawnOffset)];
			  }
			  else {
			    passedPawnScore += (1/4.0) * EvalParameters::passedPawnBonus * passedPawnRankScale[8 - Rank(pawnOffset)];
			  }
			}

			// no passed pawn bonus if pawn is doubled
		}
	}

	return passedPawnScore;
}


int getKingPlacementScore(ChessBoard * board, bool white) {
	short kingOffset = white ? board->whiteKingOffset : board->blackKingOffset;
	BITBOARD pawns = white ? board->whitePawns : board->blackPawns;
	int kingScore = 0;
	if (board->gamePhase != PHASE_ENDGAME) {
		// reward the king being covered by pawns and in the corner.
		kingScore += EvalParameters::middlegameKingCornerBonus * EvalParameters::middlegameKingPositionalBonusScale[kingOffset];
		int pawnCover = NumOnes(kingMoves[kingOffset] & pawns);
		if (pawnCover < 3) {
			kingScore += pawnCover * EvalParameters::noPawnCoverBonus;
		}

		//if (kingScore >= 0)
		//	cerr << "king score: " << kingScore << endl << board_to_string(board) << endl;

		// TODO: penalize not being able to castle ... if you haven't castled.  seems annoying 
		
		if (white) {
			if (board->whiteHasCastled == false && !board->whiteCanCastleKingside && !board->whiteCanCastleQueenside)
				kingScore += EvalParameters::cannotCastleBonus;
		}
		else {
			if (board->blackHasCastled == false && !board->whiteCanCastleKingside && !board->blackCanCastleQueenside)
				kingScore += EvalParameters::cannotCastleBonus;
		}
	}
	else {
		// reward centralized king
		kingScore += EvalParameters::endgameCentralizedKingBonus * EvalParameters::endgameKingPositionalBonusScale[kingOffset];
	}
	return kingScore;
}

int getDevelopmentScore(ChessBoard * board, bool white) {
	BITBOARD bishops = (white) ? board->whiteBishops : board->blackBishops;
	BITBOARD rooks = (white) ? board->whiteRooks : board->blackRooks;
	BITBOARD knights = (white) ? board->whiteKnights : board->blackKnights;

	BITBOARD startingBishops = (white) ? defaultStartingBoard.whiteBishops : defaultStartingBoard.blackBishops;
	BITBOARD startingRooks = (white) ? defaultStartingBoard.whiteRooks : defaultStartingBoard.blackRooks;
	BITBOARD startingKnights = (white) ? defaultStartingBoard.whiteKnights : defaultStartingBoard.blackKnights;

	return (NumOnes(bishops & startingBishops) * EvalParameters::bishop_onOriginalSquareBonus +
			NumOnes(knights & startingKnights) * EvalParameters::knight_onOriginalSquareBonus +
			NumOnes(rooks & startingRooks) * EvalParameters::rook_onOriginalSquareBonus);

	return 0;
}

int getCenterControlScore(ChessBoard * board, bool white) {
	int whiteCenterScore = 0;
	int blackCenterScore = 0;

	BITBOARD iterateCenterSquares = centerSquares;
	while(iterateCenterSquares != 0) {
		short centerSquare = FirstOne(iterateCenterSquares);
		iterateCenterSquares ^= offset_to_mask(centerSquare);
		BITBOARD attackBitboard = get_attacksto_bitboard(board, centerSquare);
		short whiteAttacks = NumOnes(attackBitboard & board->whitePieces);
		short blackAttacks = NumOnes(attackBitboard & board->blackPieces);
		short contents = board->boardContents[centerSquare];
		if (contents != EMPTY) {
			if (IsPieceWhite(contents))
				whiteAttacks += 2;
			else
				blackAttacks += 2;
		}
		whiteCenterScore += whiteAttacks * EvalParameters::whiteCenterControlScale[centerSquare];
		blackCenterScore += blackAttacks * EvalParameters::blackCenterControlScale[centerSquare];
	}

	if (white)
		return whiteCenterScore - blackCenterScore;
	else
		return blackCenterScore - whiteCenterScore;
}

int getKingAttackScore(ChessBoard * board, bool white) {
	short kingOffset = white ? board->whiteKingOffset : board->blackKingOffset;

	return 0;
}

int getPawnPositionScore(ChessBoard * board, bool white) {
	BITBOARD pawns = white ? board->whitePawns : board->blackPawns;
	int pawnPenalty = 0;
	BITBOARD rank1Pawns = pawns & RANK1;
	pawnPenalty += (NumOnes(rank1Pawns) - 1);
	BITBOARD rank2Pawns = pawns & RANK2;
	pawnPenalty += (NumOnes(rank2Pawns) - 1);
	BITBOARD rank3Pawns = pawns & RANK3;
	pawnPenalty += (NumOnes(rank3Pawns) - 1);
	BITBOARD rank4Pawns = pawns & RANK4;
	pawnPenalty += (NumOnes(rank4Pawns) - 1);
	BITBOARD rank5Pawns = pawns & RANK5;
	pawnPenalty += (NumOnes(rank5Pawns) - 1);
	BITBOARD rank6Pawns = pawns & RANK6;
	pawnPenalty += (NumOnes(rank6Pawns) - 1);
	BITBOARD rank7Pawns = pawns & RANK7;
	pawnPenalty += (NumOnes(rank7Pawns) - 1);
	BITBOARD rank8Pawns = pawns & RANK8;
	pawnPenalty += (NumOnes(rank8Pawns) - 1);

	return pawnPenalty * -50;
}
