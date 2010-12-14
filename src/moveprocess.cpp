#include "moveprocess.h"
#include "move.h"
#include "eval.h"
#include <csignal>

#define SANITY_CHECK 0

void processMove(ChessBoard * chessBoard, int move) {

#if SANITY_CHECK
	sanityCheckMove(chessBoard, move);
	if (!internalConsistencyCheck(chessBoard))
		cerr << "got error before processing " << MoveToString(move) << endl;
#endif

	chessBoard->historyBoardInfo[chessBoard->moveIndex].enPassantOffset = chessBoard->enPassantOffset;
	chessBoard->historyBoardInfo[chessBoard->moveIndex].whiteCanCastleKingside = chessBoard->whiteCanCastleKingside;
	chessBoard->historyBoardInfo[chessBoard->moveIndex].whiteCanCastleQueenside = chessBoard->whiteCanCastleQueenside;
	chessBoard->historyBoardInfo[chessBoard->moveIndex].blackCanCastleKingside = chessBoard->blackCanCastleKingside;
	chessBoard->historyBoardInfo[chessBoard->moveIndex].blackCanCastleQueenside = chessBoard->blackCanCastleQueenside;	
	chessBoard->historyBoardInfo[chessBoard->moveIndex].gamePhase = chessBoard->gamePhase;	
		
	short movePiece = GetMovePiece(move);
	short capturePiece = GetCapturePiece(move);
	bool wtm = chessBoard->whiteToMove;
	if (wtm) {
		short from = GetFrom(move);
		short to = GetTo(move);
		
		do_processMove(chessBoard, movePiece, from, to, -capturePiece, true);	
		short promotionPiece = GetPromotionPiece(move);
		if (promotionPiece != 0) {
			do_processPromotion(chessBoard, promotionPiece, to, movePiece, true);
			if (movePiece != PAWN) {
				cerr << "WHAT THE HELL -- NON-PAWN PROMOTES? " << MoveToString(move) << endl;
				raise(SIGABRT);
			}
		}
		else if (movePiece == KING) {
			if (IsKingsideCastleMove(move)) {
				chessBoard->whiteHasCastled = true;
				do_processKingsideCastle(chessBoard, movePiece, true);
			}
			else if (IsQueensideCastleMove(move)) {
				chessBoard->whiteHasCastled = true;
				do_processQueensideCastle(chessBoard, movePiece, true);
			}
		}
		do_updateCastlingPrivileges(chessBoard, movePiece, GetFrom(move));
	}
	else {
		short from = GetFrom(move);
		short to = GetTo(move);
		do_processMove(chessBoard, -movePiece, from, to, capturePiece, false);
		short promotionPiece = -GetPromotionPiece(move);
		if (promotionPiece != 0) {
			if (movePiece != PAWN) {
				cerr << "WHAT THE HELL -- NON-PAWN PROMOTES? " << MoveToString(move) << endl;
				raise(SIGABRT);
			}
			do_processPromotion(chessBoard, promotionPiece, to, -movePiece, false);
		}
		else if (movePiece == KING) {
			if (IsKingsideCastleMove(move)) {
				chessBoard->blackHasCastled = true;
				do_processKingsideCastle(chessBoard, -movePiece, false);
			}
			else if (IsQueensideCastleMove(move)) {
				chessBoard->blackHasCastled = true;
				do_processQueensideCastle(chessBoard, -movePiece, false);
			}
		}
		do_updateCastlingPrivileges(chessBoard, -movePiece, GetFrom(move));
		chessBoard->fullmoveClock++;
	}
	
	update_bitboards(chessBoard);
	//setGamePhase(chessBoard);
	
	chessBoard->moveHistory[chessBoard->moveIndex] = move;
	++chessBoard->moveIndex;
	chessBoard->boardHashes[chessBoard->moveIndex] = chessBoard->zobristHashKey;
	chessBoard->whiteCheckHistory[chessBoard->moveIndex] = chessBoard->whiteInCheck;
	chessBoard->blackCheckHistory[chessBoard->moveIndex] = chessBoard->blackInCheck;

#if SANITY_CHECK
	if (!internalConsistencyCheck(chessBoard))
		cerr << "got error after processing " << MoveToString(move) << endl;	
#endif
	
	if (chessBoard->blackPieces & chessBoard->whitePieces) {
		cerr << "white and black piece occupy the same space after processing " << MoveToString(move) << " -- was white to move? " << wtm << endl;
		cerr << board_to_string(chessBoard) << endl;
                cerr << "white pieces" << std::endl;
		cerr << bitboard_to_string(chessBoard->whitePieces) << endl;
                cerr << "black pieces " << std::endl;
		cerr << bitboard_to_string(chessBoard->blackPieces) << endl;
                cerr << "intersection " << std::endl;
		cerr << bitboard_to_string(chessBoard->blackPieces & chessBoard->whitePieces) << endl;
		raise(SIGABRT);
	}
	
}

void unprocessMove(ChessBoard * chessBoard, int move) {
	short movePiece = GetMovePiece(move);
	short capturePiece = GetCapturePiece(move);
	short to = GetTo(move);
	short from = GetFrom(move);
	
#if SANITY_CHECK
	if (!internalConsistencyCheck(chessBoard))
		cerr << "got error before unprocessing " << MoveToString(move) << endl;	
#endif
	
	if (!chessBoard->whiteToMove) {
		short promotionPiece = GetPromotionPiece(move);
		if (promotionPiece != 0 && movePiece == PAWN)
			do_unprocessPromotion(chessBoard, promotionPiece, movePiece, from, to, true);
		do_processMove(chessBoard, movePiece, to, from, 0, true);
		chessBoard->enPassantOffset = chessBoard->historyBoardInfo[chessBoard->moveIndex - 1].enPassantOffset;			
		if (capturePiece != 0) {
			do_unprocessCapture(chessBoard, movePiece, from, to, -capturePiece, true);
		}
		else if (movePiece == KING) {
			if (IsKingsideCastleMove(move)) {
				do_unprocessKingsideCastle(chessBoard, movePiece, true);
				chessBoard->whiteHasCastled = false;
			}
			else if (IsQueensideCastleMove(move)) {
				do_unprocessQueensideCastle(chessBoard, movePiece, true);
				chessBoard->whiteHasCastled = false;
			}
		}
	}
	else {
		short promotionPiece = GetPromotionPiece(move);
		if (promotionPiece != 0 && movePiece == PAWN) {
			do_unprocessPromotion(chessBoard, -promotionPiece, -movePiece, from, to, false);
		}
		do_processMove(chessBoard, -movePiece, to, from, 0, false);
		chessBoard->enPassantOffset = chessBoard->historyBoardInfo[chessBoard->moveIndex - 1].enPassantOffset;			
		if (capturePiece != 0) {
			do_unprocessCapture(chessBoard, -movePiece, from, to, capturePiece, false);
		}
		else if (movePiece == KING) {
			if (IsKingsideCastleMove(move)) {
				do_unprocessKingsideCastle(chessBoard, -movePiece, false);
				chessBoard->blackHasCastled = false;
			}
			else if (IsQueensideCastleMove(move)) {
				do_unprocessQueensideCastle(chessBoard, -movePiece, false);
				chessBoard->blackHasCastled = false;
			}
		}
		chessBoard->fullmoveClock--;
	}
	
	chessBoard->moveHistory[chessBoard->moveIndex] = 0;
	chessBoard->whiteCheckHistory[chessBoard->moveIndex] = false;
	chessBoard->blackCheckHistory[chessBoard->moveIndex] = false;
	--chessBoard->moveIndex;
	chessBoard->whiteInCheck = chessBoard->whiteCheckHistory[chessBoard->moveIndex];
	chessBoard->blackInCheck = chessBoard->blackCheckHistory[chessBoard->moveIndex];

	revertCastlingPrivileges(chessBoard, &chessBoard->historyBoardInfo[chessBoard->moveIndex]);
	chessBoard->gamePhase = chessBoard->historyBoardInfo[chessBoard->moveIndex].gamePhase;
	
#if SANITY_CHECK
	if (!internalConsistencyCheck(chessBoard))
		cerr << "got error after unprocessing " << MoveToString(move) << endl;	
#endif
	
	if (chessBoard->blackPieces & chessBoard->whitePieces) {
		cerr << "get unerror after processing " << MoveToString(move) << " -- was white to move? " << !chessBoard->whiteToMove << endl;
		cerr << board_to_string(chessBoard) << endl;
		cerr << bitboard_to_string(chessBoard->whitePieces) << endl;
		cerr << bitboard_to_string(chessBoard->blackPieces) << endl;
		raise(SIGABRT);
	}
}

void do_unprocessPromotion(ChessBoard * chessBoard, short replacePiece,
		short movePiece, short from, short to, bool whiteToMove) {
	BITBOARD toMask = offset_to_mask(to);
	set_board_contents(chessBoard, from, movePiece);

	/*short movePieceScore = getElementScore(movePiece, from);
	short replacePieceScore = getElementScore(replacePiece, from);
	chessBoard->materialScore -= replacePieceScore;
	chessBoard->materialScore += movePieceScore;*/

	if (whiteToMove) {
		//chessBoard->whiteMaterialScore = chessBoard->whiteMaterialScore + movePieceScore - replacePieceScore;
		chessBoard->whitePawns |= toMask;
		switch (replacePiece) {
		case QUEEN:
			chessBoard->whiteQueens ^= toMask;
			break;
		case ROOK:
			chessBoard->whiteRooks ^= toMask;
			break;
		case BISHOP:
			chessBoard->whiteBishops ^= toMask;
			break;
		case KNIGHT:
			chessBoard->whiteKnights ^= toMask;
			break;
		}
	}
	else {
		//chessBoard->blackMaterialScore = chessBoard->blackMaterialScore - movePieceScore + replacePieceScore;
		chessBoard->blackPawns |= toMask;
		switch (replacePiece) {
		case -QUEEN:
			chessBoard->blackQueens ^= toMask;
			break;
		case -ROOK:
			chessBoard->blackRooks ^= toMask;
			break;
		case -BISHOP:
			chessBoard->blackBishops ^= toMask;
			break;
		case -KNIGHT:
			chessBoard->blackKnights ^= toMask;
			break;
		default:
			cerr << "couldn't find piece to replace??" << endl;
		}
	}

	// should be it
}


void do_processPromotion(ChessBoard * chessBoard, short replacePiece, short toSpace, short movePiece, bool whiteToMove) {
	BITBOARD toMask = offset_to_mask(toSpace);
	
	set_board_contents(chessBoard, toSpace, replacePiece);

	/*short movePieceScore = getElementScore(movePiece, toSpace);
	short replacePieceScore = getElementScore(replacePiece, toSpace);
	chessBoard->materialScore -= movePieceScore;
	chessBoard->materialScore += replacePieceScore;*/
	
	if (whiteToMove) {
		chessBoard->whitePawns ^= toMask;
		//int oldScore = chessBoard->whiteMaterialScore;
		//chessBoard->whiteMaterialScore = chessBoard->whiteMaterialScore - movePieceScore + replacePieceScore; 
		//if (chessBoard->whiteMaterialScore < 0 && oldScore > 0)
		//	cerr << "bork bork bork bork -- " << movePieceScore << " and " << replacePieceScore << " changed " << oldScore << endl;
		switch (replacePiece) {
		case QUEEN:
			chessBoard->whiteQueens ^= toMask;
			break;
		case ROOK:
			chessBoard->whiteRooks ^= toMask;
			break;
		case KNIGHT:
			chessBoard->whiteKnights ^= toMask;
			break;
		case BISHOP:
			chessBoard->whiteBishops ^= toMask;
			break;
		}
	}
	else {
		chessBoard->blackPawns ^= toMask;
		//chessBoard->blackMaterialScore = chessBoard->blackMaterialScore + movePieceScore - replacePieceScore; 

		switch (replacePiece) {
		case -QUEEN:
			chessBoard->blackQueens ^= toMask;
			break;
		case -ROOK:
			chessBoard->blackRooks ^= toMask;
			break;
		case -KNIGHT:
			chessBoard->blackKnights ^= toMask;
			break;
		case -BISHOP:
			chessBoard->blackBishops ^= toMask;
			break;
		}
	}
	
//	if (whiteToMove)
//	else
//		chessBoard->blackPawns ^= toMask;
//	
//	switch (replacePiece) {
//	case QUEEN_WHITE:
//		chessBoard->whiteQueens ^= toMask;
//		break;
//	case QUEEN_BLACK:
//		chessBoard->blackQueens ^= toMask;
//		break;
//	case ROOK_WHITE:
//		chessBoard->whiteRooks ^= toMask;
//		break;
//	case ROOK_BLACK:
//		chessBoard->blackRooks ^= toMask;
//		break;
//	case KNIGHT_WHITE:
//		chessBoard->whiteKnights ^= toMask;
//		break;
//	case KNIGHT_BLACK:
//		chessBoard->blackKnights ^= toMask;
//		break;
//	case BISHOP_WHITE:
//		chessBoard->whiteBishops ^= toMask;
//		break;
//	case BISHOP_BLACK:
//		chessBoard->blackBishops ^= toMask;
//		break;
//	}

	// should be it
}

void do_processMove(ChessBoard * chessBoard, short item, short fromOffset, short toOffset, short capturedPiece, bool whiteToMove) {
	short newEnPassantOffset = -1;

	BITBOARD modifyMask = offset_to_mask(fromOffset) | offset_to_mask(toOffset);

	if (whiteToMove) {
		switch (item) {
		case PAWN:
			chessBoard->whitePawns ^= modifyMask;
			if (toOffset == fromOffset - 16) {  // double move
				newEnPassantOffset = (short) (fromOffset - 8);
			}
			break;
		case KNIGHT:
			chessBoard->whiteKnights ^= modifyMask;
			break;
		case BISHOP:
			chessBoard->whiteBishops ^= modifyMask;
			break;
		case ROOK:
			chessBoard->whiteRooks ^= modifyMask;
			
			break;
		case QUEEN:
			chessBoard->whiteQueens ^= modifyMask;
			break;
		case KING:
			chessBoard->whiteKings ^= modifyMask;
			chessBoard->whiteKingOffset = toOffset;
			
			break;
		}

		chessBoard->whitePieces ^= modifyMask;
	}
	else {
		switch (item) {
		case -PAWN:
			chessBoard->blackPawns ^= modifyMask;

			if (toOffset == fromOffset + 16) {  // double move
				newEnPassantOffset = (short) (fromOffset + 8);
			}
			break;
		case -KNIGHT:
			chessBoard->blackKnights ^= modifyMask;
			break;
		case -BISHOP:
			chessBoard->blackBishops ^= modifyMask;
			break;
		case -ROOK:
			chessBoard->blackRooks ^= modifyMask;
			
			break;
		case -QUEEN:
			chessBoard->blackQueens ^= modifyMask;
			break;
		case -KING:
			chessBoard->blackKings ^= modifyMask;
			chessBoard->blackKingOffset = toOffset;
			
			break;
		}

		chessBoard->blackPieces ^= modifyMask;
	}

	if (capturedPiece != 0) {
		do_processCapture(chessBoard, item, fromOffset, toOffset, capturedPiece, chessBoard->whiteToMove);
		chessBoard->occupied ^= offset_to_mask(fromOffset);
		chessBoard->occupied90R ^= offset_to_mask(rotate90R[fromOffset]);
		chessBoard->occupied45L ^= offset_to_mask(rotate45L[fromOffset]);
		chessBoard->occupied45R ^= offset_to_mask(rotate45R[fromOffset]);
	}
	else {
		chessBoard->occupied ^= modifyMask;
		chessBoard->occupied90R ^= offset_to_mask(rotate90R[fromOffset]) | offset_to_mask(rotate90R[toOffset]);
		chessBoard->occupied45L ^= offset_to_mask(rotate45L[fromOffset]) | offset_to_mask(rotate45L[toOffset]);
		chessBoard->occupied45R ^= offset_to_mask(rotate45R[fromOffset]) | offset_to_mask(rotate45R[toOffset]);
	}
	toggleWhiteToMove(chessBoard);

	chessBoard->enPassantOffset = newEnPassantOffset;

	set_board_contents(chessBoard, fromOffset, EMPTY);
	set_board_contents(chessBoard, toOffset, item);
}

void do_processCapture(ChessBoard * chessBoard, short capturingItem, int fromOffset, short toOffset, short capturedPiece, bool whiteToMove) {
	// might need from/to offset for en passant
	short capturedOffset = toOffset;
	bool enPassant = false;
	if (toOffset == chessBoard->enPassantOffset && chessBoard->boardContents[toOffset] == EMPTY) {
		if (capturingItem == PAWN_BLACK && capturedPiece == PAWN_WHITE) {
			capturedOffset = (short) (toOffset - 8);
			enPassant = true;
		}
		if (capturingItem == PAWN_WHITE && capturedPiece == PAWN_BLACK) {
			capturedOffset = (short) (toOffset + 8);
			enPassant = true;
		}
	}
	BITBOARD removeMask = ~offset_to_mask(capturedOffset);
	switch (capturedPiece) {
	case PAWN_WHITE:
		chessBoard->whitePawns &= removeMask;
		break;
	case PAWN_BLACK:
		chessBoard->blackPawns &= removeMask;
		break;
	case BISHOP_WHITE:
		chessBoard->whiteBishops &= removeMask;
		break;
	case BISHOP_BLACK:
		chessBoard->blackBishops &= removeMask;
		break;
	case ROOK_WHITE:
		chessBoard->whiteRooks &= removeMask;
		break;
	case ROOK_BLACK:
		chessBoard->blackRooks &= removeMask;
		break;
	case KNIGHT_WHITE:
		chessBoard->whiteKnights &= removeMask;
		break;
	case KNIGHT_BLACK:
		chessBoard->blackKnights &= removeMask;
		break;
	case QUEEN_WHITE:
		chessBoard->whiteQueens &= removeMask;
		break;
	case QUEEN_BLACK:
		chessBoard->blackQueens &= removeMask;
		break;
		// these cases can't hurt
	case KING_WHITE:
		chessBoard->whiteKings &= removeMask;
		chessBoard->whiteKingOffset = -1;
		break;
	case KING_BLACK:
		chessBoard->blackKings &= removeMask;
		chessBoard->blackKingOffset = -1;
		break;
	}

	if (capturedPiece > 0) {
		chessBoard->whitePieces &= removeMask;
	} else {
		chessBoard->blackPieces &= removeMask;
	}
	
	/*short elementScore = getElementScore(capturedPiece, capturedOffset);
	chessBoard->materialScore -= elementScore;
	if (whiteToMove) {
		chessBoard->blackMaterialScore += elementScore;
	}
	else {
		chessBoard->whiteMaterialScore -= elementScore;
	}*/

	if (enPassant) {
		chessBoard->occupied ^= offset_to_mask(capturedOffset) | offset_to_mask(toOffset);
		chessBoard->occupied45L ^= offset_to_mask(rotate45L[capturedOffset]) | offset_to_mask(rotate45L[toOffset]);
		chessBoard->occupied45R ^= offset_to_mask(rotate45R[capturedOffset]) | offset_to_mask(rotate45R[toOffset]);
		chessBoard->occupied90R ^= offset_to_mask(rotate90R[capturedOffset]) | offset_to_mask(rotate90R[toOffset]);
	}

	set_board_contents(chessBoard, capturedOffset, EMPTY);
}

void do_unprocessCapture(ChessBoard * chessBoard, short capturingItem, int fromOffset, short toOffset, short capturedPiece, bool whiteToMove) {
	// might need from/to offset for en passant
	short capturedOffset = toOffset;
	if (toOffset == chessBoard->enPassantOffset) {
		if (capturingItem == PAWN_BLACK && capturedPiece == PAWN_WHITE) {
			capturedOffset = (short) (toOffset - 8);
		}
		if (capturingItem == PAWN_WHITE && capturedPiece == PAWN_BLACK) {
			capturedOffset = (short) (toOffset + 8);
		}
	}
	BITBOARD addMask = offset_to_mask(capturedOffset);
	
	switch (capturedPiece) {
	case PAWN_WHITE:
		chessBoard->whitePawns |= addMask;
		break;
	case PAWN_BLACK:
		chessBoard->blackPawns |= addMask;
		break;
	case BISHOP_WHITE:
		chessBoard->whiteBishops |= addMask;
		break;
	case BISHOP_BLACK:
		chessBoard->blackBishops |= addMask;
		break;
	case ROOK_WHITE:
		chessBoard->whiteRooks |= addMask;
		break;
	case ROOK_BLACK:
		chessBoard->blackRooks |= addMask;
		break;
	case KNIGHT_WHITE:
		chessBoard->whiteKnights |= addMask;
		break;
	case KNIGHT_BLACK:
		chessBoard->blackKnights |= addMask;
		break;
	case QUEEN_WHITE:
		chessBoard->whiteQueens |= addMask;
		break;
	case QUEEN_BLACK:
		chessBoard->blackQueens |= addMask;
		break;
		// these cases can't hurt
	case KING_WHITE:
		chessBoard->whiteKings |= addMask;
		chessBoard->whiteKingOffset = toOffset;
		// whiteKings = toOffset;
		break;
	case KING_BLACK:
		chessBoard->blackKings |= addMask;
		chessBoard->blackKingOffset = toOffset;
		// blackKings = toOffset;
		break;
	}

	/*short elementScore = getElementScore(capturedPiece, capturedOffset);
	chessBoard->materialScore += elementScore;*/

	if (IsPieceWhite(capturedPiece)) {
		chessBoard->whitePieces |= addMask;
		//cerr << chessBoard->whiteMaterialScore << " and " << elementScore << endl;
		//chessBoard->whiteMaterialScore += elementScore;
	}
	else {
		chessBoard->blackPieces |= addMask;
		//chessBoard->blackMaterialScore -= elementScore;
	}

	chessBoard->occupied |= addMask;
	chessBoard->occupied45L |= offset_to_mask(rotate45L[capturedOffset]);
	chessBoard->occupied45R |= offset_to_mask(rotate45R[capturedOffset]);
	chessBoard->occupied90R |= offset_to_mask(rotate90R[capturedOffset]);

	set_board_contents(chessBoard, capturedOffset, capturedPiece);
}

void toggleWhiteToMove(ChessBoard * chessBoard) {
	chessBoard->whiteToMove = !chessBoard->whiteToMove;
	chessBoard->zobristHashKey ^= zobristWhiteToMove;
}

//void revertCastlingPrivileges(ChessBoard * chessBoard, int move) {
//	int oldPrivileges = getCastleRightsBits(move);
//	//System.err.println("retrieved old privileges: " + oldPrivileges);
//	setWhiteCanCastleKingside(chessBoard, (oldPrivileges & 8) != 0);
//	setWhiteCanCastleQueenside(chessBoard, (oldPrivileges & 4) != 0);
//	setBlackCanCastleKingside(chessBoard, (oldPrivileges & 2) != 0);
//	setBlackCanCastleQueenside(chessBoard, (oldPrivileges & 1) != 0);
//}

void revertCastlingPrivileges(ChessBoard * chessBoard, MiscBoardInfo* boardInfo) {
	//int oldPrivileges = getCastleRightsBits(move);
	//System.err.println("retrieved old privileges: " + oldPrivileges);
	setWhiteCanCastleKingside(chessBoard, boardInfo->whiteCanCastleKingside);
	setWhiteCanCastleQueenside(chessBoard, boardInfo->whiteCanCastleQueenside);
	setBlackCanCastleKingside(chessBoard, boardInfo->blackCanCastleKingside);
	setBlackCanCastleQueenside(chessBoard, boardInfo->blackCanCastleQueenside);
}

void do_unprocessQueensideCastle(ChessBoard * chessBoard, short movePiece, bool whiteToMove) {
	if (movePiece == KING_WHITE) {
		BITBOARD updateMask = offset_to_mask(D1) | offset_to_mask(A1);
		chessBoard->whiteRooks ^= updateMask;
		chessBoard->whitePieces ^= updateMask;

		set_board_contents(chessBoard, D1, EMPTY);
		set_board_contents(chessBoard, A1, ROOK_WHITE);

		chessBoard->occupied ^= (updateMask);
		chessBoard->occupied90R ^= (offset_to_mask(rotate90R[D1]) 
				| offset_to_mask(rotate90R[A1]));
		chessBoard->occupied45L ^= (offset_to_mask(rotate45L[D1]) 
				| offset_to_mask(rotate45L[A1]));
		chessBoard->occupied45R ^= (offset_to_mask(rotate45R[D1]) 
				| offset_to_mask(rotate45R[A1]));
	} else {
		BITBOARD updateMask = offset_to_mask(D8) | offset_to_mask(A8);
		chessBoard->blackRooks ^= updateMask;
		chessBoard->blackPieces ^= updateMask;

		set_board_contents(chessBoard, D8, EMPTY);
		set_board_contents(chessBoard, A8, ROOK_BLACK);

		chessBoard->occupied ^= (updateMask);
		chessBoard->occupied90R ^= (offset_to_mask(rotate90R[D8]) 
				| offset_to_mask(rotate90R[A8]));
		chessBoard->occupied45L ^= (offset_to_mask(rotate45L[D8]) 
				| offset_to_mask(rotate45L[A8]));
		chessBoard->occupied45R ^= (offset_to_mask(rotate45R[D8]) 
				| offset_to_mask(rotate45R[A8]));
	}
}

void do_unprocessKingsideCastle(ChessBoard * chessBoard, short movePiece, bool whiteToMove) {
	if (movePiece == KING_WHITE) {
		BITBOARD updateMask = offset_to_mask(F1) | offset_to_mask(H1);
		chessBoard->whiteRooks ^= updateMask;
		chessBoard->whitePieces ^= updateMask;
//			System.err.println("unprocessing kingside castling");
//			System.err.println(BitUtils.bitBoardAsString(whiteRooks));
//			System.err.println(BitUtils.bitBoardAsString(whiteRooks));

		set_board_contents(chessBoard, F1, EMPTY);
		set_board_contents(chessBoard, H1, ROOK_WHITE);

		chessBoard->occupied ^= (offset_to_mask(H1) | 
				offset_to_mask(F1));
		chessBoard->occupied90R ^= (offset_to_mask(rotate90R[H1]) | 
				offset_to_mask(rotate90R[F1]));
		chessBoard->occupied45L ^= (offset_to_mask(rotate45L[H1]) | 
				offset_to_mask(rotate45L[F1]));
		chessBoard->occupied45R ^= (offset_to_mask(rotate45R[H1]) | 
				offset_to_mask(rotate45R[F1]));

		// System.err.println(BitUtils.bitBoardAsString(occupied));
		// System.err.println(BitUtils.bitBoardAsString(whiteRooks));
		// System.err.println(BitUtils.bitBoardAsString(whiteKings));
	} else {
		BITBOARD updateMask = offset_to_mask(H8) | offset_to_mask(F8);
		chessBoard->blackRooks ^= updateMask;
		chessBoard->blackPieces ^= updateMask;
		
		set_board_contents(chessBoard, F8, EMPTY);
		set_board_contents(chessBoard, H8, ROOK_BLACK);

		chessBoard->occupied ^= updateMask;
		chessBoard->occupied90R ^= (offset_to_mask(rotate90R[H8]) 
				| offset_to_mask(rotate90R[F8]));
		chessBoard->occupied45L ^= (offset_to_mask(rotate45L[H8]) 
				| offset_to_mask(rotate45L[F8]));
		chessBoard->occupied45R ^= (offset_to_mask(rotate45R[H8]) 
				| offset_to_mask(rotate45R[F8]));
	}
}
void do_processKingsideCastle(ChessBoard * chessBoard, short movePiece, bool whiteToMove) {
	if (whiteToMove) {
		BITBOARD updateMask = offset_to_mask(H1) | offset_to_mask(F1);
		chessBoard->whiteRooks ^= updateMask;
		chessBoard->whitePieces ^= updateMask;

		set_board_contents(chessBoard, H1, EMPTY);
		set_board_contents(chessBoard, F1, ROOK_WHITE);

		chessBoard->occupied ^= updateMask;
		chessBoard->occupied90R ^= (offset_to_mask(rotate90R[H1]) 
				| offset_to_mask(rotate90R[F1]));
		chessBoard->occupied45L ^= (offset_to_mask(rotate45L[H1]) 
				| offset_to_mask(rotate45L[F1]));
		chessBoard->occupied45R ^= (offset_to_mask(rotate45R[H1]) 
				| offset_to_mask(rotate45R[F1]));
	} else {
		BITBOARD updateMask = offset_to_mask(H8) | offset_to_mask(F8);
		chessBoard->blackRooks ^= updateMask;
		chessBoard->blackPieces ^= updateMask;

		set_board_contents(chessBoard, H8, EMPTY);
		set_board_contents(chessBoard, F8, ROOK_BLACK);

		chessBoard->occupied ^= (updateMask);
		chessBoard->occupied90R ^= (offset_to_mask(rotate90R[H8]) 
				| offset_to_mask(rotate90R[F8]));
		chessBoard->occupied45L ^= (offset_to_mask(rotate45L[H8]) 
				| offset_to_mask(rotate45L[F8]));
		chessBoard->occupied45R ^= (offset_to_mask(rotate45R[H8]) 
				| offset_to_mask(rotate45R[F8]));
	}
}

void do_processQueensideCastle(ChessBoard * chessBoard, short movePiece, bool whiteToMove) {
	if (whiteToMove) {
		BITBOARD updateMask = offset_to_mask(A1) | offset_to_mask(D1);
		chessBoard->whiteRooks ^= updateMask;
		chessBoard->whitePieces ^= updateMask;

		set_board_contents(chessBoard, A1, EMPTY);
		set_board_contents(chessBoard, D1, ROOK_WHITE);			

		chessBoard->occupied ^= (updateMask);
		chessBoard->occupied90R ^= (offset_to_mask(rotate90R[A1]) 
				| offset_to_mask(rotate90R[D1]));
		chessBoard->occupied45L ^= (offset_to_mask(rotate45L[A1]) 
				| offset_to_mask(rotate45L[D1]));
		chessBoard->occupied45R ^= (offset_to_mask(rotate45R[A1]) 
				| offset_to_mask(rotate45R[D1]));
	} else {
		BITBOARD updateMask = offset_to_mask(A8) | offset_to_mask(D8);
		chessBoard->blackRooks ^= updateMask;
		chessBoard->blackPieces ^= updateMask;

		set_board_contents(chessBoard, A8, EMPTY);
		set_board_contents(chessBoard, D8, ROOK_BLACK);

		chessBoard->occupied ^= (updateMask);
		chessBoard->occupied90R ^= (offset_to_mask(rotate90R[A8]) 
				| offset_to_mask(rotate90R[D8]));
		chessBoard->occupied45L ^= (offset_to_mask(rotate45L[A8]) 
				| offset_to_mask(rotate45L[D8]));
		chessBoard->occupied45R ^= (offset_to_mask(rotate45R[A8]) 
				| offset_to_mask(rotate45R[D8]));
	}
}

void do_updateCastlingPrivileges(ChessBoard * chessBoard,
		short movePiece, short from) {
	if (movePiece == KING_WHITE) {
		setWhiteCanCastleKingside(chessBoard, false);
		setWhiteCanCastleQueenside(chessBoard, false);
	}
	else if (movePiece == KING_BLACK) {
		setBlackCanCastleKingside(chessBoard, false);
		setBlackCanCastleQueenside(chessBoard, false);
	}
	else if (movePiece == ROOK_WHITE) {
		if (from == A1)
			setWhiteCanCastleQueenside(chessBoard, false);
		if (from == H1)
			setWhiteCanCastleKingside(chessBoard, false);
	}
	else if (movePiece == ROOK_BLACK) {
		if (from == A8)
			setBlackCanCastleQueenside(chessBoard, false);
		if (from == H8)
			setBlackCanCastleKingside(chessBoard, false);
	}
}

void sanityCheckMove(ChessBoard * chessBoard, int move) {
	short from = GetFrom(move);
	short to = GetTo(move);
	short capturePiece = (chessBoard->whiteToMove ? -1 : 1) * GetCapturePiece(move);
	short promotion = GetPromotionPiece(move);
	short movePiece = (chessBoard->whiteToMove ? 1 : - 1) * GetMovePiece(move);
	if (chessBoard->boardContents[from] != movePiece) {
		cerr << "internal error: moved piece is not at that square: " << MoveToString(move) << endl;
		cerr << "FROM : " << offset_to_string(from) << endl;
		cerr << "TO : " << offset_to_string(to) << endl;
		cerr << board_to_string(chessBoard) << endl;
		raise(SIGABRT);
		
		throw InternalEngineError();
	}
	if (promotion != 0 && movePiece != (chessBoard->whiteToMove ? 1 : - 1) * PAWN) {
		cerr << "internal error: promotion a piece that is not a pawn: " << MoveToString(move) << endl;
		cerr << "FROM : " << offset_to_string(from) << endl;
		cerr << "TO : " << offset_to_string(to) << endl;
		cerr << board_to_string(chessBoard) << endl;
		raise(SIGABRT);
		
		throw InternalEngineError();
	}
	if (capturePiece != 0 && chessBoard->boardContents[to] != capturePiece && to != chessBoard->enPassantOffset) {
		cerr << "internal error: capture piece is not at that square: " << MoveToString(move) << endl;
		cerr << "FROM : " << offset_to_string(from) << endl;
		cerr << "TO : " << offset_to_string(to) << endl;
		cerr << "expected: " << capturePiece << endl;
		cerr << "got: " << chessBoard->boardContents[to] << endl;
		cerr << board_to_string(chessBoard) << endl;
		raise(SIGABRT);
		
		throw InternalEngineError();
	}
}

void processNullMove(ChessBoard * chessBoard) {
	chessBoard->historyBoardInfo[chessBoard->moveIndex].enPassantOffset = chessBoard->enPassantOffset;
	chessBoard->historyBoardInfo[chessBoard->moveIndex].whiteCanCastleKingside = chessBoard->whiteCanCastleKingside;
	chessBoard->historyBoardInfo[chessBoard->moveIndex].whiteCanCastleQueenside = chessBoard->whiteCanCastleQueenside;
	chessBoard->historyBoardInfo[chessBoard->moveIndex].blackCanCastleKingside = chessBoard->blackCanCastleKingside;
	chessBoard->historyBoardInfo[chessBoard->moveIndex].blackCanCastleQueenside = chessBoard->blackCanCastleQueenside;
	chessBoard->historyBoardInfo[chessBoard->moveIndex].gamePhase = chessBoard->gamePhase;

	toggleWhiteToMove(chessBoard);
	update_bitboards(chessBoard);
	setGamePhase(chessBoard);
	
	chessBoard->moveHistory[chessBoard->moveIndex] = 0;
	++chessBoard->moveIndex;
	chessBoard->whiteCheckHistory[chessBoard->moveIndex] = chessBoard->whiteInCheck;
	chessBoard->blackCheckHistory[chessBoard->moveIndex] = chessBoard->blackInCheck;
}

void unprocessNullMove(ChessBoard * chessBoard) {
	toggleWhiteToMove(chessBoard);

	chessBoard->enPassantOffset = chessBoard->historyBoardInfo[chessBoard->moveIndex - 1].enPassantOffset;			

	chessBoard->moveHistory[chessBoard->moveIndex] = 0;
	chessBoard->whiteCheckHistory[chessBoard->moveIndex] = false;
	chessBoard->blackCheckHistory[chessBoard->moveIndex] = false;
	--chessBoard->moveIndex;
	chessBoard->whiteInCheck = chessBoard->whiteCheckHistory[chessBoard->moveIndex];
	chessBoard->blackInCheck = chessBoard->blackCheckHistory[chessBoard->moveIndex];

	revertCastlingPrivileges(chessBoard, &chessBoard->historyBoardInfo[chessBoard->moveIndex]);
	chessBoard->gamePhase = chessBoard->historyBoardInfo[chessBoard->moveIndex].gamePhase;
}
