#pragma once
#include <memory.h>

template<bool verbose>
int evaluateBoard(ChessBoard * board, std::ostream* os) {
	board->eval = new EvalInfo();
	memset(board->eval, 0, sizeof(EvalInfo));

	preprocessEvalInformation(board);

	int score = do_evaluateBoard<verbose>(board, os);

	delete board->eval;
	return score;
}

template<bool verbose>
int getPositionScore(ChessBoard * board, std::ostream* os) {
	// passed pawns

	int phase = board->gamePhase;
	if (verbose) {
		(*os) << "\tgame phase: ";
		switch(phase) {
			case PHASE_OPENING:
				(*os) << "opening";
				break;
			case PHASE_MIDDLEGAME:
				(*os) << "middlegame";
				break;
			case PHASE_ENDGAME:
				(*os) << "endgame";
				break;
		}
		(*os) << endl;
	}

	int baseScore = getCenterControlScore(board, board->whiteToMove);
	if (verbose)
		(*os) << "\tcenter control: " << baseScore << endl;

	int whiteScore = 0;

	int whitePassedPawnScore = getPassedPawnScore(board, true);
	whiteScore += whitePassedPawnScore;
	if (verbose)
		(*os) << "\tpassed pawns (white): " << whitePassedPawnScore << endl;

	int whiteKingPlacementScore = getKingPlacementScore(board, true);
	whiteScore += whiteKingPlacementScore;
	if (verbose)
		(*os) << "\tking placement (white): " << whiteKingPlacementScore << endl;

	int whiteDevelopmentScore = getDevelopmentScore(board, true);
	whiteScore += whiteDevelopmentScore;
	if (verbose)
		(*os) << "\tdevelopment (white): " << whiteDevelopmentScore << endl;

	int whiteDoubledPawnPenalty = getDoubledPawnPenalty(board, true);
	whiteScore -= whiteDoubledPawnPenalty;
	if (verbose)
		(*os) << "\tdoubled pawns (white): -" << whiteDoubledPawnPenalty << endl;

	int blackScore = 0;

	int blackPassedPawnScore = getPassedPawnScore(board, false);
	blackScore += blackPassedPawnScore;
	if (verbose)
		(*os) << "\tpassed pawns (black): " << blackPassedPawnScore << endl;
	int blackKingPlacementScore = getKingPlacementScore(board, false);
	blackScore += blackKingPlacementScore;
	if (verbose)
		(*os) << "\tking placement (black): " << blackKingPlacementScore << endl;
	int blackDevelopmentScore = getDevelopmentScore(board, false);
	if (verbose)
		(*os) << "\tdevelopment (black): " << blackDevelopmentScore << endl;
	blackScore += blackDevelopmentScore;

	int blackDoubledPawnPenalty = getDoubledPawnPenalty(board, false);
	blackScore -= blackDoubledPawnPenalty;
	if (verbose)
		(*os) << "\tdoubled pawns (black): -" << blackDoubledPawnPenalty << endl;

	if (board->whiteToMove) { 
		return baseScore + whiteScore - blackScore;
	}
	else {
		return baseScore + blackScore - whiteScore;
	}
}

template<bool verbose>
int do_evaluateBoard(ChessBoard * board, std::ostream* os) {
	int material = getMaterialScore(board);
	
	if (verbose)
		(*os) << "material score: " << material << endl;
	int position = getPositionScore<verbose>(board, os);
	if (verbose)
		(*os) << "position score: " << position << endl;

	int gameResult = getGameResult(board);

	switch(gameResult) {
	case 0:
	  break;
	case 1:
	  return (board->whiteToMove ? CHECKMATE : -CHECKMATE);
	case -1:
	  return (board->whiteToMove ? -CHECKMATE : CHECKMATE);
	case 2:
	case 3:
	  return 0;
	}

	int totalScore = material + position;

	if (verbose)
		(*os) << "total board score: " << totalScore << endl;

	return totalScore;
}
