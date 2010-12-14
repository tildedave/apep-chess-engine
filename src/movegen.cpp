#include "movegen.h"
#include "moveprocess.h"
#include "move.h"
#include "search.h"
#include "eval.h"
#include <limits.h>

int mvvPriority[8][8] = {
		{ 0, 6, 12, 0, 0, 18, 24, 30}, // PAWN CAPTURES PRIORITY: PAWN, KNIGHT, BISHOP, ROOK, QUEEN
		{ 0, 5, 11, 0, 0, 17, 23, 29}, // KNIGHT CAPTURES PRIORITY: PAWN, KNIGHT, BISHOP, ROOK, QUEEN 
		{ 0, 4, 10, 0, 0, 16, 22, 28}, // BISHOP CAPTURES PRIORITY: PAWN, KNIGHT, BISHOP, ROOK, QUEEN
		{ 0, 3, 9, 0, 0, 15, 21, 27},   // ROOK CAPTURES PRIORITY: PAWN, KNIGHT, BISHOP, ROOK, QUEEN
		{ 0, 2, 8, 0, 0, 14, 20, 26}, // QUEEN CAPTURES PRIORITY: PAWN, KNIGHT, BISHOP, ROOK, QUEEN
		{ 0, 1, 7, 0, 0, 13, 19, 25} // KING CAPTURES PRIORITY: PAWN, KNIGHT, BISHOP, ROOK, QUEEN
};

short getPriorityForMove_MVVLVA(ChessBoard * chessBoard, int move) {
	short movePiece = GetMovePiece(move);
	short capturedPiece = GetCapturePiece(move);
	if (capturedPiece != 0) {
		short priority = PRIORITY_CAPTURE + mvvPriority[movePiece][capturedPiece];
		return priority;
	}

	return PRIORITY_NORMAL_MOVE;
}

short getPriorityForMove_SEE(ChessBoard * chessBoard, int move, bool white) {
	
	short movePiece = GetMovePiece(move);
	short capturePiece = GetCapturePiece(move);
	if (capturePiece != 0) {
		//short captureOffset = getTo(move);
		short captureOffset = GetTo(move);

		if (IsPawn(movePiece) && captureOffset == chessBoard->enPassantOffset) {
			// en passant offset
			if (white) {
				captureOffset -= 8;
			}
			else {
				captureOffset += 8;
			}
			// TODO: put en passant captures into SEE
			return PRIORITY_CAPTURE;
		}
		else {
			short seeEvaluation = (short) getStaticExchangeEvaluation(chessBoard, captureOffset);
//			cerr << "capturing on " << offset_to_string(captureOffset) << " has value of " << seeEvaluation << endl;
//			cerr << board_to_string(chessBoard) << endl;
			
			if (seeEvaluation == 0)
				return PRIORITY_CAPTURE;
			else { 
				return PRIORITY_MATERIAL_WINNING_CAPTURE + (seeEvaluation / 100);
			}
		}
	}

	return PRIORITY_NORMAL_MOVE;
}

// basically adapted from crafty's Swap()
short getStaticExchangeEvaluation(ChessBoard * board, short offset) {
	// I really doubt we could have 32 attackers for a single square :)
//	cerr << "checking static exchange evaluation on " << offset_to_string(offset) << " for board: " << endl;
//	cerr << board_to_string(board) << endl;
	int captureList[32];
	captureList[0] = getElementScore(board->boardContents[offset], offset);
	int captureIndex = 1;
	bool whiteToMove = board->whiteToMove;
	
	BITBOARD attackers = get_attacksto_bitboard(board, offset);
	//cerr << "attackers:\n" << bitboard_to_string(attackers) << endl;
	while (attackers != 0 && captureIndex < 32) {
		short smallestAttacker = getSmallestAttacker(board, attackers, offset, whiteToMove);
		if (smallestAttacker == -1)
			break;
		attackers ^= offset_to_mask(smallestAttacker);
		short capturePiece = (whiteToMove ? 1 : -1) * board->boardContents[smallestAttacker];
//		cerr << "\tsmallest attacker on " << offset_to_string(smallestAttacker) << " and is a  " << capturePiece << endl;
		captureList[captureIndex] = -captureList[captureIndex - 1] + getElementScore(capturePiece, offset);
		
		int dir = direction[offset][smallestAttacker];
		BITBOARD rooksAndQueens;
		BITBOARD bishopsAndQueens;
		if (whiteToMove) {
			rooksAndQueens = board->whiteRooks | board->whiteQueens;
			bishopsAndQueens = board->whiteBishops | board->whiteQueens;
		}
		else {
			rooksAndQueens = board->blackRooks | board->blackQueens;
			bishopsAndQueens = board->blackBishops | board->blackQueens;
		}
		
		if (dir) {
			switch (dir) {
			case 1:
				attackers |= get_rank_attacks(board, smallestAttacker) & rooksAndQueens;
				break;
			case 7:
				attackers |= get_diag45R_attacks(board, smallestAttacker) & bishopsAndQueens;
				break;
			case 8:
				attackers |= get_file_attacks(board, smallestAttacker) & rooksAndQueens;
				break;
			case 9:
				attackers |= get_diag45L_attacks(board, smallestAttacker) & bishopsAndQueens;
				break;
			case -1:
				attackers |= get_rank_attacks(board, smallestAttacker) & rooksAndQueens;
				break;
			case -7:
				attackers |= get_diag45R_attacks(board, smallestAttacker) & bishopsAndQueens;
				break;
			case -8:
				attackers |= get_file_attacks(board, smallestAttacker) & rooksAndQueens;
				break;
			case -9:
				attackers |= get_diag45L_attacks(board, smallestAttacker) & bishopsAndQueens;
				break;
			}
		}
	
		whiteToMove = !whiteToMove;
		++captureIndex;
	}
	
//	cerr << "[";
//	for(int i = 0; i < captureIndex - 1; ++i) {
//		cerr << captureList[i];
//		if (i != captureIndex - 2) {
//			cerr << ", ";
//		}
//	}
//	
//	cerr << "]" << endl;
	
	while (captureIndex > 0) {
		captureIndex--;
		if (captureList[captureIndex] > -captureList[captureIndex - 1]) {
			captureList[captureIndex - 1] = -captureList[captureIndex];
		}
	}
	
//	cerr << "static exchange evaluation for capture on " << offset_to_string(offset) << " is " << captureList[0] << ":" << endl;
//	cerr << board_to_string(board) << endl;
//	
//	if (attackers != 0)
//		raise(SIGABRT);
	return captureList[0];
	
	/*
	int value = 0;
	int see_value = 0;

	short attackerOffset = getSmallestAttacker(board, offset);

	if (attackerOffset == -1) {
		// offset not attacked
		return 0;
	}

	short capturedPiece = board->boardContents[offset];
	int move;
	if (board->whiteToMove) {
		move = MakeCapture(board->boardContents[attackerOffset], attackerOffset, offset, -capturedPiece); 
	}
	else {
		move = MakeCapture(-board->boardContents[attackerOffset], attackerOffset, offset, capturedPiece);
	}

	processMove(board, move);
	see_value = -getStaticExchangeEvaluation(board, offset);
	if (see_value >= 0) {  // don't make losing captures
		int capturedScore = getElementScore(capturedPiece, offset);
		if (!board->whiteToMove)
			capturedScore = -capturedScore;
		value = capturedScore + see_value;
	}
	unprocessMove(board, move);
	return value;
	*/
}

short getSmallestAttacker(ChessBoard * board, BITBOARD attackers, short offset, bool whiteToMove) {
	BITBOARD capturePieces = whiteToMove ? board->whitePieces : board->blackPieces;
	attackers &= capturePieces;

	int smallestValue = INFINITE_VALUE;
	short smallestAttackerOffset = -1;
	while (attackers != 0) {
		short nextAttackerOffset = FirstOne(attackers);
		attackers ^= offset_to_mask(nextAttackerOffset);

		int currentValue = getElementScore(board->boardContents[nextAttackerOffset], nextAttackerOffset);
		if (currentValue < smallestValue) {
			smallestValue = currentValue;
			smallestAttackerOffset = nextAttackerOffset;
		}
	}

	return smallestAttackerOffset;
}

int* getQuiescentMoves(ChessBoard * chessBoard, bool white, int* startMoves) {
	if (sideToMoveIsInCheck(chessBoard)) {
		int *endMoves;

		endMoves = generateCaptures(chessBoard, white, startMoves);
		endMoves = generateNonCaptures(chessBoard, white, endMoves);
		
		return endMoves;
	}
	else {
		int *endMoves;
		endMoves = generateCaptures(chessBoard, white, startMoves);
		endMoves = generateChecks(chessBoard, white, endMoves);

		return endMoves;
	}
}


int* generateCaptures(ChessBoard* chessBoard, bool white, int* moves) {
	if (white) {
		BITBOARD pieceBoard = chessBoard->whiteKnights;
		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD attacks = knightMoves[offset] & chessBoard->blackPieces;
			while (attacks != 0) {
				short to = FirstOne(attacks);
				attacks ^= offset_to_mask(to);
				int capture = MakeCapture(KNIGHT, offset, to, -chessBoard->boardContents[to]);
				*moves++ = capture;  
			}
		}

		pieceBoard = chessBoard->whiteBishops;
		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD attacks = get_bishop_attack_board(chessBoard, offset) & chessBoard->blackPieces;
			while (attacks != 0) {
				short to = FirstOne(attacks);
				attacks ^= offset_to_mask(to);
				int capture = MakeCapture(BISHOP, offset, to, -chessBoard->boardContents[to]);
				*moves++ = capture;  
			}
		}

		pieceBoard = chessBoard->whiteRooks;
		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD attacks = get_rook_attack_board(chessBoard, offset) & chessBoard->blackPieces;
			while (attacks != 0) {
				short to = FirstOne(attacks);
				attacks ^= offset_to_mask(to);
				int capture = MakeCapture(ROOK, offset, to, -chessBoard->boardContents[to]);
				*moves++ = capture;  
			}
		}

		pieceBoard = chessBoard->whiteQueens;
		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD attacks = get_queen_attack_board(chessBoard, offset) & chessBoard->blackPieces;
			while (attacks != 0) {
				short to = FirstOne(attacks);
				attacks ^= offset_to_mask(to);
				int capture = MakeCapture(QUEEN, offset, to, -chessBoard->boardContents[to]);
				*moves++ = capture;  
			}
		}

		pieceBoard = chessBoard->whitePawns;
		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD target = chessBoard->blackPieces;
			if (chessBoard->enPassantOffset != -1)
				target |= offset_to_mask(chessBoard->enPassantOffset);
			BITBOARD attacks = pawnAttacksWhite[offset] & target;
			
			while (attacks != 0) {
				short to = FirstOne(attacks);
				attacks ^= offset_to_mask(to);
				if (to <= H8) {
					int capture = MakePromotion(PAWN, offset, to, -chessBoard->boardContents[to], QUEEN);
					*moves++ = capture;  
				}
				else {
					if (to == chessBoard->enPassantOffset) {
						int capture = MakeCapture(PAWN, offset, to, -chessBoard->boardContents[to + 8]);
						*moves++ = capture;  
					}
					else {
						int capture = MakeCapture(PAWN, offset, to, -chessBoard->boardContents[to]);
						*moves++ = capture;  
					}
				}
			}
		}

		pieceBoard = chessBoard->whiteKings;
		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD attacks = kingMoves[offset] & chessBoard->blackPieces;
			while (attacks != 0) {
				short to = FirstOne(attacks);
				attacks ^= offset_to_mask(to);
				int capture = MakeCapture(KING, offset, to, -chessBoard->boardContents[to]);
				*moves++ = capture;  
			}
		}
	}
	else {
		BITBOARD pieceBoard = chessBoard->blackKnights;
		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD attacks = knightMoves[offset] & chessBoard->whitePieces;
			while (attacks != 0) {
				short to = FirstOne(attacks);
				attacks ^= offset_to_mask(to);
				int capture = MakeCapture(KNIGHT, offset, to, chessBoard->boardContents[to]);
				*moves++ = capture;  
			}
		}

		pieceBoard = chessBoard->blackBishops;
		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD attacks = get_bishop_attack_board(chessBoard, offset) & chessBoard->whitePieces;
			while (attacks != 0) {
				short to = FirstOne(attacks);
				attacks ^= offset_to_mask(to);
				int capture = MakeCapture(BISHOP, offset, to, chessBoard->boardContents[to]);
				*moves++ = capture;  
			}
		}

		pieceBoard = chessBoard->blackRooks;
		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD attacks = get_rook_attack_board(chessBoard, offset) & chessBoard->whitePieces;
			while (attacks != 0) {
				short to = FirstOne(attacks);
				attacks ^= offset_to_mask(to);
				int capture = MakeCapture(ROOK, offset, to, chessBoard->boardContents[to]);
				*moves++ = capture;  
			}
		}

		pieceBoard = chessBoard->blackQueens;
		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD attacks = get_queen_attack_board(chessBoard, offset) & chessBoard->whitePieces;
			while (attacks != 0) {
				short to = FirstOne(attacks);
				attacks ^= offset_to_mask(to);
				int capture = MakeCapture(QUEEN, offset, to, chessBoard->boardContents[to]);
				*moves++ = capture;  
			}
		}

		pieceBoard = chessBoard->blackPawns;
		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD target = chessBoard->whitePieces;
			if (chessBoard->enPassantOffset != -1)
				target |= offset_to_mask(chessBoard->enPassantOffset);
			BITBOARD attacks = pawnAttacksBlack[offset] & target;
			while (attacks != 0) {
				short to = FirstOne(attacks);
				attacks ^= offset_to_mask(to);
				if (to >= A1) {
					int capture = MakePromotion(PAWN, offset, to, chessBoard->boardContents[to], QUEEN);
					*moves++ = capture;  
				}
				else {
					if (to == chessBoard->enPassantOffset) {
						int capture = MakeCapture(PAWN, offset, to, chessBoard->boardContents[to - 8]);
						*moves++ = capture;  
					}
					else {
						int capture = MakeCapture(PAWN, offset, to, chessBoard->boardContents[to]);
						*moves++ = capture;  
					}
				}
			}
		}

		pieceBoard = chessBoard->blackKings;
		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD attacks = kingMoves[offset] & chessBoard->whitePieces;
			while (attacks != 0) {
				short to = FirstOne(attacks);
				attacks ^= offset_to_mask(to);
				int capture = MakeCapture(KING, offset, to, chessBoard->boardContents[to]);
				*moves++ = capture;  
			}
		}
	}
	
	return moves;
}

int* generateNonCaptures(ChessBoard* chessBoard, bool white, int *destMoves) {
	if (white) {
		BITBOARD target = ~chessBoard->occupied;
		BITBOARD pieceBoard = chessBoard->whiteKnights;
		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD moves = knightMoves[offset] & target;
			while (moves != 0) {
				short to = FirstOne(moves);
				moves ^= offset_to_mask(to);
				int move = MakeMove(KNIGHT, offset, to);
				*destMoves++ = move;
			}
		}

		pieceBoard = chessBoard->whiteBishops;
		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD moves = get_bishop_attack_board(chessBoard, offset) & target;
			while (moves != 0) {
				short to = FirstOne(moves);
				moves ^= offset_to_mask(to);
				int move = MakeMove(BISHOP, offset, to);
				*destMoves++ = move;
			}
		}

		pieceBoard = chessBoard->whiteRooks;

		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD moves = get_rook_attack_board(chessBoard, offset) & target;
			while (moves != 0) {
				short to = FirstOne(moves);
				moves ^= offset_to_mask(to);
				int move = MakeMove(ROOK, offset, to);
				*destMoves++ = move;
			}
		}
		
		pieceBoard = chessBoard->whiteQueens;
		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD moves = get_queen_attack_board(chessBoard, offset) & target;
			while (moves != 0) {
				short to = FirstOne(moves);
				moves ^= offset_to_mask(to);
				int move = MakeMove(QUEEN, offset, to);
				*destMoves++ = move;
			}
		}

		pieceBoard = chessBoard->whitePawns;
		BITBOARD advance1 = (chessBoard->whitePawns >> 8) & target;
		BITBOARD advance2 = ((advance1 & RANK3) >> 8) & target;

		while (advance2 != 0) {
			short destOffset = FirstOne(advance2);
			advance2 ^= offset_to_mask(destOffset);
			int move = MakeMove(PAWN, destOffset + 16, destOffset);
			*destMoves++ = move;
		}

		while (advance1 != 0) {
			short destOffset = FirstOne(advance1);
			advance1 ^= offset_to_mask(destOffset);
			if (destOffset <= H8) {
				short fromOffset = destOffset + 8;
				int queenPromotion = MakePromotion(PAWN, fromOffset, destOffset, 0, QUEEN);
				int rookPromotion = MakePromotion(PAWN, fromOffset, destOffset, 0, ROOK);
				int knightPromotion = MakePromotion(PAWN, fromOffset, destOffset, 0, KNIGHT);
				int bishopPromotion = MakePromotion(PAWN, fromOffset, destOffset, 0, BISHOP);

				*destMoves++ = queenPromotion;
				*destMoves++ = rookPromotion;
				*destMoves++ = knightPromotion;
				*destMoves++ = bishopPromotion;
			}
			else {
				int move = MakeMove(PAWN, destOffset + 8, destOffset);
				*destMoves++ = move;
			}
		}

		BITBOARD rank7Pawns = chessBoard->whitePawns & RANK7;
		if (rank7Pawns != 0) {
			BITBOARD pawnTarget = chessBoard->blackPieces & RANK8;
			do {
				short offset = FirstOne(rank7Pawns);
				rank7Pawns ^= offset_to_mask(offset);

				BITBOARD pawnCaptures = pawnAttacksWhite[offset] & pawnTarget;

				while (pawnCaptures != 0) {
					short destOffset = FirstOne(pawnCaptures);
					pawnCaptures ^= offset_to_mask(destOffset);
					int rookPromotion = MakePromotion(PAWN, offset, destOffset, -chessBoard->boardContents[destOffset], ROOK);
					int knightPromotion = MakePromotion(PAWN, offset, destOffset, -chessBoard->boardContents[destOffset], KNIGHT);
					int bishopPromotion = MakePromotion(PAWN, offset, destOffset, -chessBoard->boardContents[destOffset], BISHOP);

					*destMoves++ = rookPromotion;
					*destMoves++ = bishopPromotion;
					*destMoves++ = knightPromotion;
					/*
					list.add(rookPromotion);
					list.add(knightPromotion);
					list.add(bishopPromotion);
					*/
				}

			} while (rank7Pawns != 0);
		}

		short offset = chessBoard->whiteKingOffset;
		if (offset != -1) {
			BITBOARD moves = kingMoves[offset] & target;
			while (moves != 0) {
				short to = FirstOne(moves);
				moves ^= offset_to_mask(to);
				int move = MakeMove(KING, offset, to);
				*destMoves++ = move;
			}
		}
		
		bool canCastleKingSide = chessBoard->whiteCanCastleKingside &&  
		chessBoard->boardContents[F1] == EMPTY && 
		chessBoard->boardContents[G1] == EMPTY &&
		chessBoard->boardContents[H1] == ROOK_WHITE;
		bool canCastleQueenSide = chessBoard->whiteCanCastleQueenside && 
		chessBoard->boardContents[D1] == EMPTY && 
		chessBoard->boardContents[C1] == EMPTY &&
		chessBoard->boardContents[B1] == EMPTY &&
		chessBoard->boardContents[A1] == ROOK_WHITE;

		if (canCastleKingSide) {
			// check if there's anything between you and the rook.  plus if anyone's attacking the 
			// squares between you and the rook... but not including the rook.  for the kingside, check 
			// if anyone from the other side is attacking e1, f1, or g1.  for the queenside, check 
			// if anyone is attacking e1, d1, c1.

			//		BITBOARD kingsideRook = offset_to_mask((isWhite) ? H1 : H8);
			//		kingsideRook &= isWhite ? chessBoard->whiteRooks : chessBoard->blackRooks;
			short destSquare = G1;

			BITBOARD eAttacks = get_attacksto_bitboard(chessBoard, E1);
			BITBOARD fAttacks = get_attacksto_bitboard(chessBoard, F1);
			BITBOARD gAttacks = get_attacksto_bitboard(chessBoard, G1);
			BITBOARD otherPieces = chessBoard->blackPieces;

			bool canCastle = ((eAttacks & otherPieces) == 0) &&
			((fAttacks & otherPieces) == 0) &&
			((gAttacks & otherPieces) == 0);

			if (canCastle) {
				*destMoves++ = MakeMove(KING, chessBoard->whiteKingOffset, destSquare);
			}
		}
		if (canCastleQueenSide) {
			//		BITBOARD queensideRook = offset_to_mask((isWhite) ? A1 : A8);
			//		queensideRook &= isWhite ? chessBoard->whiteRooks : chessBoard->blackRooks;
			short destSquare = C1;

			BITBOARD eAttacks = get_attacksto_bitboard(chessBoard, E1);
			BITBOARD dAttacks = get_attacksto_bitboard(chessBoard, D1);
			BITBOARD cAttacks = get_attacksto_bitboard(chessBoard, C1);
			BITBOARD otherPieces = chessBoard->blackPieces;

			bool canCastle = ((eAttacks & otherPieces) == 0) &&
			((dAttacks & otherPieces) == 0) &&
			((cAttacks & otherPieces) == 0);

			if (canCastle) {
				*destMoves++ = MakeMove(KING, chessBoard->whiteKingOffset, destSquare);
			}
		}
	}
	else {
		BITBOARD target = ~chessBoard->occupied;
		BITBOARD pieceBoard = chessBoard->blackKnights;
		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD moves = knightMoves[offset] & target;
			while (moves != 0) {
				short to = FirstOne(moves);
				moves ^= offset_to_mask(to);
				int move = MakeMove(KNIGHT, offset, to);
				*destMoves++ = move;
			}
		}

		pieceBoard = chessBoard->blackBishops;
		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD moves = get_bishop_attack_board(chessBoard, offset) & target;
			while (moves != 0) {
				short to = FirstOne(moves);
				moves ^= offset_to_mask(to);
				int move = MakeMove(BISHOP, offset, to);
				*destMoves++ = move;
			}
		}

		pieceBoard = chessBoard->blackRooks;

		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD moves = get_rook_attack_board(chessBoard, offset) & target;
			while (moves != 0) {
				short to = FirstOne(moves);
				moves ^= offset_to_mask(to);
				int move = MakeMove(ROOK, offset, to);
				*destMoves++ = move;
			}
		}

		pieceBoard = chessBoard->blackQueens;
		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD moves = get_queen_attack_board(chessBoard, offset) & target;
			while (moves != 0) {
				short to = FirstOne(moves);
				moves ^= offset_to_mask(to);
				int move = MakeMove(QUEEN, offset, to);
				*destMoves++ = move;
			}
		}

		pieceBoard = chessBoard->blackPawns;
		BITBOARD advance1 = (chessBoard->blackPawns << 8) & target;
		BITBOARD advance2 = ((advance1 & RANK6) << 8) & target;

		while (advance2 != 0) {
			short destOffset = FirstOne(advance2);
			advance2 ^= offset_to_mask(destOffset);
			int move = MakeMove(PAWN, destOffset - 16, destOffset);
			*destMoves++ = move;
	}

		while (advance1 != 0) {
			short destOffset = FirstOne(advance1);
			advance1 ^= offset_to_mask(destOffset);
			if (destOffset >= A1) {
				short fromOffset = destOffset - 8;
				int queenPromotion = MakePromotion(PAWN, fromOffset, destOffset, 0, QUEEN);
				int rookPromotion = MakePromotion(PAWN, fromOffset, destOffset, 0, ROOK);
				int knightPromotion = MakePromotion(PAWN, fromOffset, destOffset, 0, KNIGHT);
				int bishopPromotion = MakePromotion(PAWN, fromOffset, destOffset, 0, BISHOP);

				*destMoves++ = queenPromotion;
				*destMoves++ = rookPromotion;
				*destMoves++ = knightPromotion;
				*destMoves++ = bishopPromotion;
			}
			else {
				int move = MakeMove(PAWN, destOffset - 8, destOffset);
				*destMoves++ = move;
			}
//			int move = MakeMove(PAWN, destOffset - 8, destOffset);
//			*destMoves++ = move;
		}

		BITBOARD rank2Pawns = chessBoard->blackPawns & RANK2;
		if (rank2Pawns != 0) {
			BITBOARD pawnTarget = chessBoard->whitePieces & RANK1;
			do {
				short offset = FirstOne(rank2Pawns);
				rank2Pawns ^= offset_to_mask(offset);

				BITBOARD pawnCaptures = pawnAttacksBlack[offset] & pawnTarget;

				while (pawnCaptures != 0) {
					short destOffset = FirstOne(pawnCaptures);
					pawnCaptures ^= offset_to_mask(destOffset);
					int rookPromotion = MakePromotion(PAWN, offset, destOffset, 
									  chessBoard->boardContents[destOffset], ROOK);
					int knightPromotion = MakePromotion(PAWN, offset, destOffset, 
									    chessBoard->boardContents[destOffset], KNIGHT);
					int bishopPromotion = MakePromotion(PAWN, offset, destOffset, 
									    chessBoard->boardContents[destOffset], BISHOP);

					
					*destMoves++ = rookPromotion;
					*destMoves++ = knightPromotion;
					*destMoves++ = bishopPromotion;
				}

			} while (rank2Pawns != 0);
		}

		short offset = chessBoard->blackKingOffset;
		if (offset != -1) {
			BITBOARD moves = kingMoves[offset] & target;
			while (moves != 0) {
				short to = FirstOne(moves);
				moves ^= offset_to_mask(to);
				int move = MakeMove(KING, offset, to);
				*destMoves++ = move;
			}
		}
		
		bool canCastleKingSide = chessBoard->blackCanCastleKingside &&  
		chessBoard->boardContents[F8] == EMPTY && 
		chessBoard->boardContents[G8] == EMPTY &&
		chessBoard->boardContents[H8] == ROOK_BLACK;
		bool canCastleQueenSide = chessBoard->blackCanCastleQueenside && 
		chessBoard->boardContents[D8] == EMPTY && 
		chessBoard->boardContents[C8] == EMPTY &&
		chessBoard->boardContents[B8] == EMPTY &&
		chessBoard->boardContents[A8] == ROOK_BLACK;

		if (canCastleKingSide) {
			// check if there's anything between you and the rook.  plus if anyone's attacking the 
			// squares between you and the rook... but not including the rook.  for the kingside, check 
			// if anyone from the other side is attacking e8, f8, or g8.  for the queenside, check 
			// if anyone is attacking e8, d8, c8.

			//		BITBOARD kingsideRook = offset_to_mask((isBlack) ? H8 : H8);
			//		kingsideRook &= isBlack ? chessBoard->blackRooks : chessBoard->blackRooks;
			short destSquare = G8;

			BITBOARD eAttacks = get_attacksto_bitboard(chessBoard, E8);
			BITBOARD fAttacks = get_attacksto_bitboard(chessBoard, F8);
			BITBOARD gAttacks = get_attacksto_bitboard(chessBoard, G8);
			BITBOARD otherPieces = chessBoard->whitePieces;

			bool canCastle = ((eAttacks & otherPieces) == 0) &&
			((fAttacks & otherPieces) == 0) &&
			((gAttacks & otherPieces) == 0);

			if (canCastle) {
				*destMoves++ = MakeMove(KING, chessBoard->blackKingOffset, destSquare);
			}
		}
		if (canCastleQueenSide) {
			//		BITBOARD queensideRook = offset_to_mask((isBlack) ? A8 : A8);
			//		queensideRook &= isBlack ? chessBoard->blackRooks : chessBoard->blackRooks;
			short destSquare = C8;

			BITBOARD eAttacks = get_attacksto_bitboard(chessBoard, E8);
			BITBOARD dAttacks = get_attacksto_bitboard(chessBoard, D8);
			BITBOARD cAttacks = get_attacksto_bitboard(chessBoard, C8);
			BITBOARD otherPieces = chessBoard->whitePieces;

			bool canCastle = ((eAttacks & otherPieces) == 0) &&
			((dAttacks & otherPieces) == 0) &&
			((cAttacks & otherPieces) == 0);

			if (canCastle) {
				*destMoves++ = MakeMove(KING, chessBoard->blackKingOffset, destSquare);
			}
		}
	}
	return destMoves;
}


int* generateChecks(ChessBoard* chessBoard, bool white, int *destMoves) {
	if (white) {
		if (chessBoard->blackKingOffset == -1)
			return destMoves;

		BITBOARD target = knightMoves[chessBoard->blackKingOffset] & ~chessBoard->occupied;
		BITBOARD pieceBoard = chessBoard->whiteKnights;
		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD moves = knightMoves[offset] & target;
			while (moves != 0) {
				short to = FirstOne(moves);
				moves ^= offset_to_mask(to);
				int move = MakeMove(KNIGHT, offset, to);
				*destMoves++ = move;
			}
		}

		target = get_bishop_attack_board(chessBoard, chessBoard->blackKingOffset) & ~chessBoard->occupied;
		pieceBoard = chessBoard->whiteBishops;
		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD moves = get_bishop_attack_board(chessBoard, offset) & target;
			while (moves != 0) {
				short to = FirstOne(moves);
				moves ^= offset_to_mask(to);
				int move = MakeMove(BISHOP, offset, to);
				*destMoves++ = move;
			}
		}

		pieceBoard = chessBoard->whiteRooks;
		target = get_rook_attack_board(chessBoard, chessBoard->blackKingOffset) & ~chessBoard->occupied;

		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD moves = get_rook_attack_board(chessBoard, offset) & target;
			while (moves != 0) {
				short to = FirstOne(moves);
				moves ^= offset_to_mask(to);
				int move = MakeMove(ROOK, offset, to);
				*destMoves++ = move;
			}
		}
		
		pieceBoard = chessBoard->whiteQueens;
		target = get_queen_attack_board(chessBoard, chessBoard->blackKingOffset) & ~chessBoard->occupied;
		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD moves = get_queen_attack_board(chessBoard, offset) & target;
			while (moves != 0) {
				short to = FirstOne(moves);
				moves ^= offset_to_mask(to);
				int move = MakeMove(QUEEN, offset, to);
				*destMoves++ = move;
			}
		}

		pieceBoard = chessBoard->whitePawns;
		target = pawnAttacksBlack[chessBoard->blackKingOffset] & ~chessBoard->occupied;
		BITBOARD advance1 = (chessBoard->whitePawns >> 8) & target;
		BITBOARD advance2 = ((advance1 & RANK3) >> 8) & target;

		while (advance2 != 0) {
			short destOffset = FirstOne(advance2);
			advance2 ^= offset_to_mask(destOffset);
			int move = MakeMove(PAWN, destOffset + 16, destOffset);
			*destMoves++ = move;
		}

		while (advance1 != 0) {
			short destOffset = FirstOne(advance1);
			advance1 ^= offset_to_mask(destOffset);
			if (destOffset <= H8) {
				short fromOffset = destOffset + 8;
				int queenPromotion = MakePromotion(PAWN, fromOffset, destOffset, 0, QUEEN);
				int rookPromotion = MakePromotion(PAWN, fromOffset, destOffset, 0, ROOK);
				int knightPromotion = MakePromotion(PAWN, fromOffset, destOffset, 0, KNIGHT);
				int bishopPromotion = MakePromotion(PAWN, fromOffset, destOffset, 0, BISHOP);

				*destMoves++ = queenPromotion;
				*destMoves++ = rookPromotion;
				*destMoves++ = knightPromotion;
				*destMoves++ = bishopPromotion;
			}
			else {
				int move = MakeMove(PAWN, destOffset + 8, destOffset);
				*destMoves++ = move;
			}
		}

		BITBOARD rank7Pawns = chessBoard->whitePawns & RANK7;
		if (rank7Pawns != 0) {
			BITBOARD pawnTarget = chessBoard->blackPieces & RANK8;
			do {
				short offset = FirstOne(rank7Pawns);
				rank7Pawns ^= offset_to_mask(offset);

				BITBOARD pawnCaptures = pawnAttacksWhite[offset] & pawnTarget;

				while (pawnCaptures != 0) {
					short destOffset = FirstOne(pawnCaptures);
					pawnCaptures ^= offset_to_mask(destOffset);
					int rookPromotion = MakePromotion(PAWN, offset, destOffset, -chessBoard->boardContents[destOffset], ROOK);
					int knightPromotion = MakePromotion(PAWN, offset, destOffset, -chessBoard->boardContents[destOffset], KNIGHT);
					int bishopPromotion = MakePromotion(PAWN, offset, destOffset, -chessBoard->boardContents[destOffset], BISHOP);

					*destMoves++ = rookPromotion;
					*destMoves++ = bishopPromotion;
					*destMoves++ = knightPromotion;
					/*
					list.add(rookPromotion);
					list.add(knightPromotion);
					list.add(bishopPromotion);
					*/
				}

			} while (rank7Pawns != 0);
		}
	}
	else {
		if (chessBoard->whiteKingOffset == -1)
			return destMoves;

		BITBOARD target = knightMoves[chessBoard->whiteKingOffset] & ~chessBoard->occupied;
		BITBOARD pieceBoard = chessBoard->blackKnights;
		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD moves = knightMoves[offset] & target;
			while (moves != 0) {
				short to = FirstOne(moves);
				moves ^= offset_to_mask(to);
				int move = MakeMove(KNIGHT, offset, to);
				*destMoves++ = move;
			}
		}

		pieceBoard = chessBoard->blackBishops & ~chessBoard->occupied;
		target = get_bishop_attack_board(chessBoard, chessBoard->whiteKingOffset) & ~chessBoard->occupied;

		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD moves = get_bishop_attack_board(chessBoard, offset) & target;
			while (moves != 0) {
				short to = FirstOne(moves);
				moves ^= offset_to_mask(to);
				int move = MakeMove(BISHOP, offset, to);
				*destMoves++ = move;
			}
		}

		pieceBoard = chessBoard->blackRooks & ~chessBoard->occupied;
		target = get_rook_attack_board(chessBoard, chessBoard->whiteKingOffset) & ~chessBoard->occupied;

		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD moves = get_rook_attack_board(chessBoard, offset) & target;
			while (moves != 0) {
				short to = FirstOne(moves);
				moves ^= offset_to_mask(to);
				int move = MakeMove(ROOK, offset, to);
				*destMoves++ = move;
			}
		}

		pieceBoard = chessBoard->blackQueens & ~chessBoard->occupied;
		target = get_queen_attack_board(chessBoard, chessBoard->whiteKingOffset) & ~chessBoard->occupied;

		while (pieceBoard != 0) {
			short offset = FirstOne(pieceBoard);
			pieceBoard ^= offset_to_mask(offset);
			BITBOARD moves = get_queen_attack_board(chessBoard, offset) & target;
			while (moves != 0) {
				short to = FirstOne(moves);
				moves ^= offset_to_mask(to);
				int move = MakeMove(QUEEN, offset, to);
				*destMoves++ = move;
			}
		}

		pieceBoard = chessBoard->blackPawns & ~chessBoard->occupied;
		target = pawnAttacksWhite[chessBoard->whiteKingOffset] & ~chessBoard->occupied;
		BITBOARD advance1 = (chessBoard->blackPawns << 8) & target;
		BITBOARD advance2 = ((advance1 & RANK6) << 8) & target;

		while (advance2 != 0) {
			short destOffset = FirstOne(advance2);
			advance2 ^= offset_to_mask(destOffset);
			int move = MakeMove(PAWN, destOffset - 16, destOffset);
			*destMoves++ = move;
		}

		while (advance1 != 0) {
			short destOffset = FirstOne(advance1);
			advance1 ^= offset_to_mask(destOffset);
			if (destOffset >= A1) {
				short fromOffset = destOffset - 8;
				int queenPromotion = MakePromotion(PAWN, fromOffset, destOffset, 0, QUEEN);
				int rookPromotion = MakePromotion(PAWN, fromOffset, destOffset, 0, ROOK);
				int knightPromotion = MakePromotion(PAWN, fromOffset, destOffset, 0, KNIGHT);
				int bishopPromotion = MakePromotion(PAWN, fromOffset, destOffset, 0, BISHOP);

				*destMoves++ = queenPromotion;
				*destMoves++ = rookPromotion;
				*destMoves++ = knightPromotion;
				*destMoves++ = bishopPromotion;
			}
			else {
				int move = MakeMove(PAWN, destOffset - 8, destOffset);
				*destMoves++ = move;
			}
//			int move = MakeMove(PAWN, destOffset - 8, destOffset);
//			*destMoves++ = move;
		}

		BITBOARD rank2Pawns = chessBoard->blackPawns & RANK2;
		if (rank2Pawns != 0) {
			BITBOARD pawnTarget = chessBoard->whitePieces & RANK1;
			do {
				short offset = FirstOne(rank2Pawns);
				rank2Pawns ^= offset_to_mask(offset);

				BITBOARD pawnCaptures = pawnAttacksBlack[offset] & pawnTarget;

				while (pawnCaptures != 0) {
					short destOffset = FirstOne(pawnCaptures);
					pawnCaptures ^= offset_to_mask(destOffset);
					int rookPromotion = MakePromotion(PAWN, offset, destOffset, chessBoard->boardContents[destOffset], ROOK);
					int knightPromotion = MakePromotion(PAWN, offset, destOffset, chessBoard->boardContents[destOffset], KNIGHT);
					int bishopPromotion = MakePromotion(PAWN, offset, destOffset, chessBoard->boardContents[destOffset], BISHOP);

					
					*destMoves++ = rookPromotion;
					*destMoves++ = knightPromotion;
					*destMoves++ = bishopPromotion;
				}

			} while (rank2Pawns != 0);
		}
	}
	
	return destMoves;
}
