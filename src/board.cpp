#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "movepriorityqueue.h"
#include "moveprocess.h"
#include "movegen.h"
#include "move.h"
#include "eval.h"
#include "transpositiontable.h"

#include <csignal>
#include <iostream>

ChessBoard defaultStartingBoard;

// do the actual computation
bool isKingInCheckImpl(ChessBoard* board, short king);

int initialize_board(ChessBoard * board) {
	board->whiteCanCastleKingside = false;
	board->whiteCanCastleQueenside = false;
	board->blackCanCastleKingside = false;
	board->blackCanCastleQueenside = false;
	board->whiteToMove = true;
	board->halfmoveClock = 0;
	board->fullmoveClock = 1;
	board->enPassantOffset = -1;

	board->occupied = 0;
	board->occupied45L = 0;
	board->occupied45R = 0;
	board->occupied90R = 0;

	board->whiteHasCastled = false;
	board->blackHasCastled = false;

	/*board->materialScore = 0;
	board->whiteMaterialScore = 0;
	board->blackMaterialScore = 0;*/
	
	board->whitePieces = 0;
	board->blackPieces = 0;
	board->whitePawns = 0;
	board->blackPawns = 0;
	board->whiteKnights = 0;
	board->blackKnights = 0;
	board->whiteBishops = 0;
	board->blackBishops = 0;
	board->whiteRooks = 0;
	board->blackRooks = 0;
	board->whiteQueens = 0;
	board->blackQueens = 0;
	board->whiteKings = 0;
	board->blackKings = 0;
	
	board->whiteInCheck = false;
	board->blackInCheck = false;
	board->whiteKingOffset = 0;
	board->blackKingOffset = 0;

	board->zobristHashKey = zobristWhiteToMove;
	board->gamePhase = PHASE_OPENING;

	board->moveIndex = 0;

	for(int i = 0; i < 64; ++i) {
		board->boardContents[i] = 0;
	}

	for(int i = 0; i < MAX_MOVES; ++i) {
	    board->whiteCheckHistory[i] = false;
	    board->blackCheckHistory[i] = false;
	    // TODO: maybe make these pointers ... eh
	    board->historyBoardInfo[i].whiteCanCastleKingside = false;
	    board->historyBoardInfo[i].blackCanCastleKingside = false;
	    board->historyBoardInfo[i].whiteCanCastleQueenside = false;
	    board->historyBoardInfo[i].blackCanCastleQueenside = false;
	    board->historyBoardInfo[i].enPassantOffset = -1;
	    board->historyBoardInfo[i].gamePhase = PHASE_OPENING;
	}
	
	// not sure why these have to be initialized.  must have
	// a bug with my pointer arithmetic somewhere
	for (int i = 0; i < MAX_DEPTH; ++i) {
		for(int j = 0; j < MAX_BRANCH; ++j) {
			board->moveList[i][j] = 0;
			board->moveSortList[i][j] = 0;
			if (i == 0)
				board->checkmateTestMoves[i] = 0;
		}
	}
	return 1;
}

int copy_board(ChessBoard * dest, ChessBoard * src) {
	dest->whiteCanCastleKingside = src->whiteCanCastleKingside;
	dest->whiteCanCastleQueenside = src->whiteCanCastleQueenside;
	dest->blackCanCastleKingside = src->blackCanCastleKingside;
	dest->blackCanCastleQueenside = src->blackCanCastleQueenside;
	dest->whiteToMove = src->whiteToMove;
	dest->halfmoveClock = src->halfmoveClock;
	dest->fullmoveClock = src->fullmoveClock;
	dest->enPassantOffset = src->enPassantOffset;

	dest->occupied = src->occupied;
	dest->occupied45L = src->occupied45L;
	dest->occupied45R = src->occupied45R;
	dest->occupied90R = src->occupied90R;
	
	dest->gamePhase = src->gamePhase;

	dest->whitePieces = src->whitePieces;
	dest->blackPieces = src->blackPieces;
	dest->whitePawns = src->whitePawns;
	dest->blackPawns = src->blackPawns;
	dest->whiteKnights = src->whiteKnights;
	dest->blackKnights = src->blackKnights;
	dest->whiteBishops = src->whiteBishops;
	dest->blackBishops = src->blackBishops;
	dest->whiteRooks = src->whiteRooks;
	dest->blackRooks = src->blackRooks;
	dest->whiteQueens = src->whiteQueens;
	dest->blackQueens = src->blackQueens;
	dest->whiteKings = src->whiteKings;
	dest->blackKings = src->blackKings;
	
	dest->whiteInCheck = src->whiteInCheck;
	dest->blackInCheck = src->blackInCheck;
	dest->whiteKingOffset = src->whiteKingOffset;
	dest->blackKingOffset = src->blackKingOffset;

	dest->zobristHashKey = src -> zobristHashKey;

	dest->moveIndex = src->moveIndex;

	for(int i = 0; i < 64; ++i) {
	    dest->boardContents[i] = src->boardContents[i];
	}

	for(int i = 0; i < MAX_MOVES; ++i) {
	    dest->whiteCheckHistory[i] = src->whiteCheckHistory[i];
	}

	for(int i = 0; i < MAX_MOVES; ++i) {
	    dest->blackCheckHistory[i] = src->blackCheckHistory[i];
	}

	return 1;
}

std::string board_to_string(ChessBoard* board) {
  std::string boardString;
  std::string fenString;  

  board_to_string_internal(board, boardString, fenString);

  std::ostringstream ss;
  ss << "hash key: " << board->zobristHashKey;
  ss << " phase: " << board->gamePhase;
  //cerr << board->whiteMaterialScore << endl;
  //cerr << board->blackMaterialScore << endl;
  return boardString + "\n" + fenString + "\n" + ss.str();
}

void board_to_string_internal(ChessBoard * board, std::string& boardString, std::string& fenString) {
	for(int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			short contents = board->boardContents[i * 8 + j];
			std::string elementString = item_to_string(contents);
			boardString.append(elementString);
			if (contents != 0) {
				fenString.append(elementString);
			}
			else { 
				fenString.append("1");
			}
		}
		boardString += "\n";
		if (i != 7)
			fenString.append("/");
	}

	fenString += ' ';
	fenString += (board->whiteToMove ? "w" : "b");
	fenString += ' ';
	bool canCastle = false;
	if (board->whiteCanCastleKingside) {
		fenString += 'K';
		canCastle = true;
	}
	if (board->whiteCanCastleQueenside) {
		fenString += 'Q';
		canCastle = true;
	}
	if (board->blackCanCastleKingside) {
		fenString += 'k';
		canCastle = true;
	}
	if (board->blackCanCastleQueenside) {
		fenString += 'q';
		canCastle = true;
	}
	if (!canCastle)
		fenString += '-';
	fenString += ' ';
	// en passant square
	if (board->enPassantOffset != -1)
		fenString += offset_to_string(board->enPassantOffset);
	else
		fenString += '-';
	fenString += ' ';
	fenString += int_to_string(board->halfmoveClock);
	fenString += ' ';
	fenString += int_to_string(board->fullmoveClock);
}

bool loadBoardFromFEN(ChessBoard * board, std::string fenString) {
	ancientSwitch = !ancientSwitch;
	initialize_board(board);

	int strIndex = 0;
	int x = 0;
	int y = 0;
	// first, process board position
	while (fenString.at(strIndex) != ' ') {
		char currChar = fenString.at(strIndex);
		if (x >= 8 && currChar != '/') {
			std::cerr << ("error in FEN notation!  column too wide -- "
					+ strIndex) << std::endl;
			return false;
		}

		if (is_digit(currChar) == true) {
			// which digit
			int currDigit = char_to_int(currChar);
			for (int j = 0; j < currDigit; ++j) {
				//chessBoard.boardContents[x + 8 * y] = EMPTY;
				set_board_contents(board, (short) (x + 8 * y), EMPTY);
				++x;
			}
		} else {
			if (currChar == '/') {
				x = 0;
				y++;
			} else {
				short piece = piece_from_char(currChar);
				if (piece == EMPTY) {
					std::cerr << "error in FEN notation!  couldn't figure out piece for " << currChar << std::endl;
					return false;
				}

				set_board_contents(board, (short) (x + 8 * y), piece);
				//chessBoard.boardContents[x + 8 * y] = piece;
				++x;
			}
		}

		++strIndex;
	}

	// burn the blank space
	++strIndex;
	if (fenString.at(strIndex) == 'w') {
		// white's turn
		board->whiteToMove = true;
	} else if (fenString.at(strIndex) == 'b') {
		board->whiteToMove = false;
		board->zobristHashKey ^= zobristWhiteToMove;
	} else {
		std::cerr << "error in FEN -- couldn't determine whose turn" << std::endl;
		return false;
	}

	++strIndex;
	// should be a blank space
	if (fenString.at(strIndex) != ' ') {
		std::cerr << "error in FEN -- no whitespace after whose turn" << std::endl;
		return false;
	}

	++strIndex;

	// castling

	while (fenString.at(strIndex) != ' ') {
		switch (fenString.at(strIndex)) {
		case 'K':
			board->whiteCanCastleKingside = true;
			break;
		case 'Q':
			board->whiteCanCastleQueenside = true;
			break;
		case 'k':
			board->blackCanCastleKingside = true;
			break;
		case 'q':
			board->blackCanCastleQueenside = true;
			break;
		}

		++strIndex;
	}

	// burn the space
	++strIndex;

	// en passant square -- either '-' or a square.
	if (fenString.at(strIndex) == '-') {
		board->enPassantOffset = -1;
	} else {
		// first should be letter
		char fileChar = fenString[strIndex];
		int file = file_to_column(fileChar);
		++strIndex;
		// ugly
		//char rank = Integer.toString(8 - Character.digit(fenString.at(strIndex), 10)).at(0);
		int rank = atoi(&fenString.at(strIndex));
		// TODO: possibly bugged
		board->enPassantOffset = get_offset(file, 8 - rank);
	}

	++strIndex;
	// should be a blank space
//	if (fenString.at(strIndex) != ' ') {
//		std::cerr << "error in FEN -- no blank square after en passant square" << std::endl;
//		std::cerr << fenString.at(strIndex) << std::endl;
//		return;
//	}

	// do these later

	// halfmove clock
	// fullmove clock

	initialize_bitboards(board);
	board->boardHashes[board->moveIndex] = board->zobristHashKey;

	return true;
}

std::string boardToFEN(ChessBoard * board) {
  std::string fenString = "";
  std::string boardString = "";

  board_to_string_internal(board, boardString, fenString);
  return fenString;
}

void set_board_contents(ChessBoard * board, short offset, short contents) {
	if (board->boardContents[offset] != EMPTY) {
	  short oldContents = board->boardContents[offset];
	  board->zobristHashKey ^= zobristArray[getZobristOffsetForContents(oldContents)][offset];
	}
	board->boardContents[offset] = contents;
	if (contents != EMPTY) {
	  board->zobristHashKey ^= zobristArray[getZobristOffsetForContents(contents)][offset];
	}
}

int getZobristOffsetForContents(short contents) {
	switch (contents) {
	case PAWN_WHITE:
		return 0;
	case PAWN_BLACK:
		return 6;
	case KNIGHT_WHITE:
		return 1;
	case KNIGHT_BLACK:
		return 7;
	case BISHOP_WHITE:
		return 2;
	case BISHOP_BLACK:
		return 8;
	case ROOK_WHITE:
		return 3;
	case ROOK_BLACK:
		return 9;
	case QUEEN_WHITE:
		return 4;
	case QUEEN_BLACK:
		return 10;
	case KING_WHITE:
		return 5;
	case KING_BLACK:
		return 11;
	}
	
	return -1;
}

void initialize_pieces(ChessBoard * board) {
	for (int offset = 0; offset < 64; ++offset) {
		short contents = board->boardContents[offset];
		if (contents != EMPTY) {
			BITBOARD currSquare = offset_to_mask(offset);
			switch (contents) {
			case PAWN_WHITE:
				board->whitePawns |= currSquare;
				break;
			case PAWN_BLACK:
				board->blackPawns |= currSquare;
				break;
			case KNIGHT_WHITE:
				board->whiteKnights |= currSquare;
				break;
			case KNIGHT_BLACK:
				board->blackKnights |= currSquare;
				break;
			case BISHOP_WHITE:
				board->whiteBishops |= currSquare;
				break;
			case BISHOP_BLACK:
				board->blackBishops |= currSquare;
				break;
			case ROOK_WHITE:
				board->whiteRooks |= currSquare;
				break;
			case ROOK_BLACK:
				board->blackRooks |= currSquare;
				break;
			case QUEEN_WHITE:
				board->whiteQueens |= currSquare;
				break;
			case QUEEN_BLACK:
				board->blackQueens |= currSquare;
				break;
			case KING_WHITE:
				board->whiteKings |= currSquare;
				break;
			case KING_BLACK:
				board->blackKings |= currSquare;
				break;
			}
			
			// hash key has already been correctly updated
			//short elementScore = getElementScore(contents, offset);

			if (IsPieceWhite(contents)) {
				board->whitePieces |= currSquare;
				//cerr << board->whiteMaterialScore << " vs " << elementScore << endl;
				//board->whiteMaterialScore += elementScore;
			} else {
				board->blackPieces |= currSquare;
				//board->blackMaterialScore -= elementScore;
			}

			board->occupied |= currSquare;
			board->occupied90R |= offset_to_mask(rotate90R[offset]);
			board->occupied45R |= offset_to_mask(rotate45R[offset]);
			board->occupied45L |= offset_to_mask(rotate45L[offset]);
		}
		
	}

	if (board->whiteKings != 0)
		board->whiteKingOffset = FirstOne(board->whiteKings);
	if (board->blackKings != 0)
		board->blackKingOffset = FirstOne(board->blackKings);
	
	board->zobristHashKey ^= zobristWhiteToMove;
	board->zobristHashKey ^= zobristWhiteCanCastleKingside;
	board->zobristHashKey ^= zobristWhiteCanCastleQueenside;
	board->zobristHashKey ^= zobristBlackCanCastleKingside;
	board->zobristHashKey ^= zobristBlackCanCastleQueenside;
}


void initialize_bitboards(ChessBoard * board) {
	initialize_pieces(board);
	update_bitboards(board);
	board->whiteCheckHistory[board->moveIndex] = board->whiteInCheck;
	board->blackCheckHistory[board->moveIndex] = board->blackInCheck;
}

void update_bitboards(ChessBoard * board) {
	compute_checks(board);
}

void compute_checks(ChessBoard * board) {
	if (isKingInCheckImpl(board, KING_WHITE))
		board->whiteInCheck = true;
	else
		board->whiteInCheck = false;
	if (isKingInCheckImpl(board, KING_BLACK))
		board->blackInCheck = true;
	else
		board->blackInCheck = false;

	/*
	cerr << "white in check: " << board->whiteInCheck << endl;
	cerr << "black in check: " << board->blackInCheck << endl;
	*/

}

bool isKingInCheck(ChessBoard * board, short king) {
	if (king == KING_WHITE)
		return board->whiteInCheck;
	if (king == KING_BLACK)
		return board->blackInCheck;

	// error
	return false;
}


// TODO: this is far too complicated.  only needs to check the attacks-to 
// bitboard against the opponent's pieces
bool isKingInCheckImpl(ChessBoard * board, short king) {
    bool white = IsPieceWhite(king);
    BITBOARD attacksToBoard;
    if (white) {
    	attacksToBoard = get_black_attacksto_bitboard(board, board->whiteKingOffset);
    }
    else {
    	attacksToBoard = get_white_attacksto_bitboard(board, board->blackKingOffset);
    }

    return (attacksToBoard != 0);

/*
	int kingOffset;
	BITBOARD ownPieces;
	bool white = IsPieceWhite(king);
	if (white) {
		// kingOffset = BitUtils.FirstOne(whiteKings);
		kingOffset = board->whiteKingOffset;
		ownPieces = board->whitePieces;
	} else {
		// kingOffset = BitUtils.FirstOne(blackKings);
		kingOffset = board->blackKingOffset;
		ownPieces = board->blackPieces;
	}

	if (kingOffset != -1) {
		if (white) {
   	   	        if ((pawnAttacksWhite[kingOffset] & board->blackPawns) != 0) {
			    return true;
			}
			if ((knightMoves[kingOffset] & board->blackKnights) != 0)
				return true;
			if ((get_bishop_attack_board(board, kingOffset) & (board->blackQueens | board->blackBishops)) != 0)
				return true;
			if ((get_rook_attack_board(board, kingOffset) & (board->blackQueens | board->blackRooks)) != 0)
				return true;
			if ((kingMoves[kingOffset] & board->blackKings) != 0)
				return true;
			return false;
		}
		else {
			if ((board->whitePawns & pawnAttacksBlack[kingOffset]) != 0)
				return true;
			if ((board->whiteKnights & knightMoves[kingOffset]) != 0)
				return true;
			if (((board->whiteQueens | board->whiteBishops) & get_bishop_attack_board(board, kingOffset)) != 0)
				return true;
			if (((board->whiteQueens | board->whiteRooks) & get_rook_attack_board(board, kingOffset)) != 0)
				return true;
			if ((board->whiteKings & (kingMoves[kingOffset])) != 0)
				return true;
			return false;
		}
	}
	return false;
*/
}

bool isNotPawnMoveOrCapture(int move) {
  return !IsPawn(GetMovePiece(move)) && GetCapturePiece(move) == 0;
}


BITBOARD get_queen_attack_board(ChessBoard* board, short offset) {
	return get_bishop_attack_board(board, offset) | get_rook_attack_board(board, offset);	
}

BITBOARD get_bishop_attack_board(ChessBoard* board, short offset) {
	BITBOARD occ45Rattacks = get_diag45R_attacks(board, offset);
	BITBOARD occ45Lattacks = get_diag45L_attacks(board, offset);

	return occ45Rattacks | occ45Lattacks;	
}

BITBOARD get_rook_attack_board(ChessBoard* board, short offset) {
	BITBOARD rookFileAttacks = get_file_attacks(board, offset);
	BITBOARD rookRankAttacks = get_rank_attacks(board, offset);
	
	return rookFileAttacks | rookRankAttacks;
}

BITBOARD get_diag45L_attacks(ChessBoard* board, short offset) {
	int occ45L = (int) ((board->occupied45L >> shift45L[offset]) & 63);
	BITBOARD occ45Lattacks = diag45LAttacks[offset][occ45L];
	return occ45Lattacks;
}

BITBOARD get_diag45R_attacks(ChessBoard* board, short offset) {
	int occ45R = (int) ((board->occupied45R >> shift45R[offset]) & 63);
	BITBOARD occ45Rattacks = diag45RAttacks[offset][occ45R];
	return occ45Rattacks;
}

BITBOARD get_file_attacks(ChessBoard* board, short offset) {
	int occFile = (int) ((board->occupied90R >> shiftFile[offset]) & 63);
	BITBOARD occFileAttacks = fileAttacks[offset][occFile];
	return occFileAttacks;
}

BITBOARD get_rank_attacks(ChessBoard* board, short offset) {
	int occRank = (int) ((board->occupied >> shiftRank[offset]) & 63);
	BITBOARD occRankAttacks = rankAttacks[offset][occRank];
	return occRankAttacks;
}

bool internalConsistencyCheck(ChessBoard* board, ChessBoard* backBoard) {
	for (short offset = 0; offset < 64; ++offset) {
		short squareContents = board->boardContents[offset];
		BITBOARD checkMask = offset_to_mask(offset);
		switch (squareContents) {
		case PAWN_WHITE:
			if ((board->whitePawns & checkMask) == 0) {
				cerr << "error with white pawns" << endl;
				raise(SIGABRT);
			}
			break;
		case PAWN_BLACK:
			if ((board->blackPawns & checkMask) == 0) {
				cerr << "error with black pawns" << endl;
				raise(SIGABRT);
			}
			break;
		case KNIGHT_WHITE:
			if((board->whiteKnights & checkMask) == 0) { 
				cerr << "error with white knights" << endl; 
				raise(SIGABRT);
			}
			break;
		case KNIGHT_BLACK:
			if((board->blackKnights & checkMask) == 0) { 
				cerr << "error with white knights" << endl; 
				raise(SIGABRT);
			}
			break;
		case BISHOP_WHITE:
			if((board->whiteBishops & checkMask) == 0) { 
				cerr << "error with white bishops" << endl; 
				raise(SIGABRT);
			}
			break;
		case BISHOP_BLACK:
			if((board->blackBishops & checkMask) == 0) { 
				cerr << "error with white bishops" << endl; 
				raise(SIGABRT);
			}
			break;
		case ROOK_WHITE:
			if((board->whiteRooks & checkMask) == 0) { 
				cerr << "error with white rook" << endl; 
				raise(SIGABRT);
			}
			break;
		case ROOK_BLACK:
			if((board->blackRooks & checkMask) == 0) { 
				cerr << "error with white rooks" << endl;  
				raise(SIGABRT);
			}
			break;
		case QUEEN_WHITE:
			if((board->whiteQueens & checkMask) == 0) { 
				cerr << "error with white queens" << endl;  raise(SIGABRT); 
			}
			break;
		case QUEEN_BLACK:
			if((board->blackQueens & checkMask) == 0) { 
				cerr << "error with white queens" << endl;  raise(SIGABRT); 
			}
			break;
		case KING_WHITE:
			if((board->whiteKings & checkMask) == 0) { 
				cerr << "error with white kings" << endl;  raise(SIGABRT);
			}
			if(board->whiteKingOffset != offset) { 
				cerr << "error with white kings" << endl;  raise(SIGABRT);
			}
			break;
		case KING_BLACK:
			if ((board->blackKings & checkMask) == 0) { 
				cerr << "error with black kings -- " << endl << bitboard_to_string(board->blackKings) << endl << offset_to_string(offset) << endl;  raise(SIGABRT);
			}
			if (board->blackKingOffset != offset) { 
				cerr << "error with black king offset" << endl;  raise(SIGABRT); 
			}
			break;
		}

		if (squareContents == EMPTY) {
			if((board->occupied & checkMask) != 0) { cerr << "error with occupied at " << offset_to_string(offset) << endl; raise(SIGABRT); }
			if((board->occupied45L & offset_to_mask(rotate45L[offset])) != 0) { cerr << "error with occupied45L" << endl;  raise(SIGABRT); }
			if((board->occupied45R & offset_to_mask(rotate45R[offset])) != 0) { cerr << "error with occupied45R" << endl;  raise(SIGABRT); }
			if((board->occupied90R & offset_to_mask(rotate90R[offset])) != 0) { cerr << "error with occupied90R" << endl;  raise(SIGABRT); }
		} else {
			if((board->occupied & checkMask) == 0) { 
				cerr << "error with occupied -- " << offset_to_string(offset) << " has " << squareContents << ", but is empty in the occupied map" << endl;
				cerr << board_to_string(board) << endl;
				cerr << bitboard_to_string(board->blackPawns) << endl;
				cerr << bitboard_to_string(board->occupied) << endl;
				cerr << board_to_string(backBoard) << endl;
				raise(SIGABRT); 
			}
			if((board->occupied45L & offset_to_mask(rotate45L[offset])) == 0) { cerr << "error with occupied45L" << endl;  raise(SIGABRT); }
			if((board->occupied45R & offset_to_mask(rotate45R[offset])) == 0) { cerr << "error with occupied45R" << endl;  raise(SIGABRT); }
			if((board->occupied90R & offset_to_mask(rotate90R[offset])) == 0) { cerr << "error with occupied90R" << endl;  raise(SIGABRT); }
		}
	}

	return true;
}

bool boardsEqual(ChessBoard* board1, ChessBoard* board2) {
	if (board1 == board2)
		return true;
	
	if (board1->blackBishops != board2->blackBishops) {
		cerr << "Black bishops not equal" << endl;
		return false;
	}
	if (board1->blackCanCastleKingside != board2->blackCanCastleKingside) {
		cerr <<"different black can castle kingside values" << endl;
		return false;
	}
	if (board1->blackCanCastleQueenside != board2->blackCanCastleQueenside) {
		cerr <<"different black can castle queenside values" << endl;
		return false;
	}
	if (board1->blackKings != board2->blackKings) {
		cerr <<"Black kings not equal" << endl;
		return false;
	}
	if (board1->blackKnights != board2->blackKnights) {
		cerr <<"black knights not equal" << endl;
		return false;
	}
	if (board1->blackPawns != board2->blackPawns) {
		cerr <<"black pawns not equal" << endl;
		cerr << bitboard_to_string(board1->blackPawns) << endl;
		cerr << "vs" << endl;
		cerr << bitboard_to_string(board2->blackPawns) << endl;
		return false;
	}
	if (board1->blackPieces != board2->blackPieces) {
		cerr <<"black pieces not equal" << endl;
		return false;
	}
	if (board1->blackQueens != board2->blackQueens) {
		cerr <<"black queens not equal" << endl;
		return false;
	}
	if (board1->blackRooks != board2->blackRooks) {
		cerr <<"black rooks not equal" << endl;
		return false;
	}
	/*if (board1->whiteMaterialScore != board2->whiteMaterialScore) {
		cerr << "different white material score" << endl;
		return false;
	}
	if (board1->blackMaterialScore != board2->blackMaterialScore) {
		cerr << "different black material score" << endl;
		return false;
	}*/
	for(int i = 0; i < 64; ++i) {
	    if (board1->boardContents[i] != board2->boardContents[i]) {
		cerr << "board contents not equal at " << offset_to_string(i) << endl;
		cerr << board1->boardContents[i]  << " vs " << board2->boardContents[i] << endl;
		return false;
	    }
	}
//	if (board1->!Arrays.equals(boardContents, board2->boardContents)) {
//		cerr <<"board contents not equal" << endl;
//		return false;
//	}
	if (board1->enPassantOffset != board2->enPassantOffset) {
		cerr <<"different en passant values" << endl;
		return false;
	}
	if (board1->fullmoveClock != board2->fullmoveClock)
		return false;
	if (board1->halfmoveClock != board2->halfmoveClock)
		return false;
	if (board1->occupied != board2->occupied)
		return false;
	if (board1->occupied45L != board2->occupied45L)
		return false;
	if (board1->occupied45R != board2->occupied45R)
		return false;
	if (board1->occupied90R != board2->occupied90R)
		return false;
	if (board1->whiteBishops != board2->whiteBishops)
		return false;
	if (board1->whiteCanCastleKingside != board2->whiteCanCastleKingside) {
		cerr <<"different white can castle kingside values" << endl;
		return false;
	}
	if (board1->whiteCanCastleQueenside != board2->whiteCanCastleQueenside) {
		cerr <<"different white can castle queenside values" << endl;
		return false;
	}
	if (board1->gamePhase != board2->gamePhase) {
		cerr << "different game phases" << endl;
		return false;
	}
	if (board1->whiteKings != board2->whiteKings)
		return false;
	if (board1->whiteKnights != board2->whiteKnights)
		return false;
	if (board1->whitePawns != board2->whitePawns) {
		cerr <<"white pawns not equal" << endl;
		return false;
	}
	if (board1->whitePieces != board2->whitePieces) {
		cerr <<"white pieces not equal" << endl;
		return false;
	}
	if (board1->whiteQueens != board2->whiteQueens) {
		cerr <<"white queens not equal" << endl;			
		return false;
	}
	if (board1->whiteRooks != board2->whiteRooks) {
		cerr <<"white rooks not equal" << endl;
		return false;
	}
	if (board1->whiteToMove != board2->whiteToMove) {
		cerr <<"different white to move values" << endl;
		return false;
	}
	if (board1->whiteKingOffset != board2->whiteKingOffset) {
		cerr <<"diff white king offsets" << endl;
		return false;
	}
	if (board1->blackKingOffset != board2->blackKingOffset) {
		cerr <<"diff black king offsets" << endl;
		return false;
	}
	if (board1->whiteInCheck != board2->whiteInCheck) {
		cerr <<"diff white in check" << endl;
		return false;
	}
	if (board1->blackInCheck != board2->blackInCheck) {
		cerr <<"diff black in check" << endl;
		return false;
	}
	if (board1->zobristHashKey != board2->zobristHashKey) {
		cerr <<"different zobrist hash key" << endl;
		return false;
	}
	for(int i = 0; i < MAX_MOVES; ++i) {
	    if (board1->whiteCheckHistory[i] != board2->whiteCheckHistory[i]) {
		cerr << "white check history not equal" << endl;
		return false;
	    }
	}
	for(int i = 0; i < MAX_MOVES; ++i) {
	    if (board1->blackCheckHistory[i] != board2->blackCheckHistory[i]) {
		cerr << "black check history not equal at " << i << endl;
		return false;
	    }
	}
	if (board1->moveIndex != board2->moveIndex) {
		cerr <<"different move index" << endl;
		return false;
	}
	/*if (board1->materialScore != board2->materialScore) {
		cerr << "different material score -- " << board1->materialScore << " vs " << board2->materialScore << endl;
		return false;
	}*/
	return true;
}

BITBOARD get_attacksto_bitboard(ChessBoard* board, short offset) {
	BITBOARD attackMask = 0;
	BITBOARD bishops = (board->whiteBishops | board->blackBishops | board->whiteQueens | board->blackQueens);
	if (bishops != 0) {
		attackMask |= bishops & get_bishop_attack_board(board, offset);
	}
	attackMask |= (board->whiteKnights | board->blackKnights) & knightMoves[offset];
	BITBOARD rooks = (board->whiteRooks | board->blackRooks | board->whiteQueens | board->blackQueens);
	if (rooks != 0) {
		attackMask |= rooks & get_rook_attack_board(board, offset);
	}
	attackMask |= (board->whiteKings | board->blackKings) & kingMoves[offset];

	attackMask |= board->whitePawns & pawnAttacksBlack[offset];
	attackMask |= board->blackPawns & pawnAttacksWhite[offset];

	return attackMask;
}

BITBOARD get_white_attacksto_bitboard(ChessBoard* board, short offset) {
    BITBOARD attackMask = 0;
	BITBOARD bishops = (board->whiteBishops | board->whiteQueens);
	if (bishops != 0) {
		attackMask |= bishops & get_bishop_attack_board(board, offset);
	}
	attackMask |= (board->whiteKnights) & knightMoves[offset];
	BITBOARD rooks = (board->whiteRooks | board->whiteQueens);
	if (rooks != 0) {
		attackMask |= rooks & get_rook_attack_board(board, offset);
	}
	attackMask |= (board->whiteKings) & kingMoves[offset];

	attackMask |= board->whitePawns & pawnAttacksBlack[offset];

	return attackMask;
}

BITBOARD get_black_attacksto_bitboard(ChessBoard* board, short offset) {
    BITBOARD attackMask = 0;
	BITBOARD bishops = (board->blackBishops | board->blackQueens);
	if (bishops != 0) {
		attackMask |= bishops & get_bishop_attack_board(board, offset);
	}
	attackMask |= (board->blackKnights) & knightMoves[offset];
	BITBOARD rooks = (board->blackRooks | board->blackQueens);
	if (rooks != 0) {
		attackMask |= rooks & get_rook_attack_board(board, offset);
	}
	attackMask |= (board->blackKings) & kingMoves[offset];

	attackMask |= board->blackPawns & pawnAttacksWhite[offset];

	return attackMask;
}

BITBOARD getAttacksBitboard(ChessBoard* board, bool whiteToMove) {
  BITBOARD attackMask = 0;
  BITBOARD bishops = (whiteToMove) ? 
    (board->whiteBishops | board->whiteQueens) : 
    (board->blackBishops | board->blackQueens);
  while(bishops != 0) {
    short offset = FirstOne(bishops);
    bishops ^= offset_to_mask(offset);
    attackMask |= get_bishop_attack_board(board, offset);
  }

  BITBOARD rooks = (whiteToMove) ? 
    (board->whiteRooks | board->whiteQueens) :
    (board->blackRooks | board->blackQueens);
  while(rooks != 0) {
    short offset = FirstOne(rooks);
    rooks ^= offset_to_mask(offset);
    attackMask |= get_rook_attack_board(board, offset);
  }

  BITBOARD knights = (whiteToMove) ? 
    (board->whiteKnights) : (board->blackKnights);
  while (knights != 0) {
    short offset = FirstOne(knights);
    knights ^= offset_to_mask(offset);
    attackMask |= knightMoves[offset];
  }

  BITBOARD pawns = (whiteToMove) ? 
    (board->whitePawns) : (board->blackPawns);
  while (pawns != 0) {
    short offset = FirstOne(pawns);
    pawns ^= offset_to_mask(offset);
    attackMask |= (whiteToMove) ? (pawnMovesWhite[offset]) : (pawnMovesBlack[offset]);
  }

  BITBOARD kings = (whiteToMove) ? (board->whiteKings) : (board->blackKings);
  while (kings != 0) {
    short offset = FirstOne(kings);
    kings ^= offset_to_mask(offset);
    attackMask |= kingMoves[offset];
  }

  return attackMask;
}

void setWhiteCanCastleKingside(ChessBoard * board, bool b) {
	if (board->whiteCanCastleKingside != b)
		board->zobristHashKey ^= zobristWhiteCanCastleKingside;
	board->whiteCanCastleKingside = b;		
}

void setWhiteCanCastleQueenside(ChessBoard * board, bool b) {
	if (board->whiteCanCastleQueenside != b)
		board->zobristHashKey ^= zobristWhiteCanCastleQueenside;
	board->whiteCanCastleQueenside = b;		
}

void setBlackCanCastleKingside(ChessBoard * board, bool b) {
	if (board->blackCanCastleKingside != b)
		board->zobristHashKey ^= zobristBlackCanCastleKingside;
	board->blackCanCastleKingside = b;		
}

void setBlackCanCastleQueenside(ChessBoard * board, bool b) {
	if (board->blackCanCastleQueenside != b)
		board->zobristHashKey ^= zobristBlackCanCastleQueenside;
	board->blackCanCastleQueenside = b;
}

bool sideToMoveIsInCheck(ChessBoard* board) {
  return (board->whiteToMove && isKingInCheck(board, KING_WHITE)) || 
    (!board->whiteToMove && isKingInCheck(board, KING_BLACK));
}

bool checkForInsufficientMatingMaterial(ChessBoard* board) {
  if ((board->whitePawns | board->blackPawns) != 0)
    return false;
  
  
  if ((board->whiteRooks | board->blackRooks | board->whiteQueens | board->blackQueens) == 0) {
    // no rooks or queens, so we can check in depth

    if ((board->whiteBishops | board->blackBishops | board->whiteKnights | board->blackKnights) == 0) {
      return true;
    }

    if ((board->whiteBishops | board->blackBishops) == 0) {
      if ((board->blackKnights == 0) && 
	  (NumOnes(board->whiteKnights) <= 1)) {
	return true;
      }
	
      if ((board->whiteKnights == 0) && 
	  (NumOnes(board->blackKnights) <= 1)) {
	return true;
      }
    }

    if ((board->whiteKnights | board->blackKnights) == 0) {
      if ((board->blackBishops == 0) && 
	  (NumOnes(board->whiteBishops) <= 1)) {
	return true;
      }

      if ((board->whiteBishops == 0) && 
	  (NumOnes(board->blackKnights) <= 1)) {
	return true;
      }

      bool whiteOnLight = ((board->whiteBishops & lightSquares) != 0);
      bool whiteOnDark = ((board->whiteBishops & darkSquares) != 0);
      bool blackOnLight = ((board->blackBishops & lightSquares) != 0);
      bool blackOnDark = ((board->blackBishops & darkSquares) != 0);

      if (whiteOnLight && !whiteOnDark && blackOnLight && !blackOnDark) {
	return true;
      }

      if (whiteOnDark && !whiteOnLight && blackOnDark && !blackOnLight) {
	return true;
      }
    }
  }

  return false;
}

int getGameResult(ChessBoard * board) {
  // TODO: faster checkmate testing.  for now we just 
  // generate all possible moves and test them

  // ABLE-TO-MATE TESTING
  if (checkForInsufficientMatingMaterial(board)) {
    return 3;
  }

  // CHECKMATE/STALEMATE TESTING
  short kingToCheck;
  int* startMoves = board->checkmateTestMoves;
  int* endMoves;
		
  if (board->whiteToMove) {
    endMoves = generateCaptures(board, true, startMoves);
    endMoves = generateNonCaptures(board, true, endMoves);
    kingToCheck = KING_WHITE;
  }
  else { 
    endMoves = generateCaptures(board, false, startMoves);
    endMoves = generateNonCaptures(board, false, endMoves);
    kingToCheck = KING_BLACK;
  }
	
  for(int* currentMove = board->checkmateTestMoves; 
      currentMove < endMoves; 
      ++currentMove) {
    //cerr << "\t" << MoveToString(*currentMove) << endl;
    int move = *currentMove;

    processMove(board, move);
    if (!isKingInCheck(board, kingToCheck)) {
      unprocessMove(board, move);
      return 0;
    }
    unprocessMove(board, move);
  }
	
  // NO LEGAL MOVES -- if it's white to move and white is in check, checkmate
  if (isKingInCheck(board, kingToCheck)) {
    if (board->whiteToMove)
      return -1;
    else 
      // opposite for black
      return 1;
  }
	
  // STALEMATE
  return 2;
}

void setGamePhase(ChessBoard * board) {
	// only set it to the endgame really -- we'll set it to be middlegame
	if (board->gamePhase == PHASE_OPENING)
		return;
	if (board->gamePhase == PHASE_ENDGAME)
		return;
	else if (board->eval->pieceWhiteMaterialScore <= 800 && board->eval->pieceBlackMaterialScore <= 800) {

		board->gamePhase = PHASE_ENDGAME;
		//cerr << "endgame time because of lack of material" << endl;
		//cerr << board->whiteMaterialScore << endl;
		//cerr << board->blackMaterialScore << endl;
		//cerr << board_to_string(board) << endl;
		return;
	}
	else if (board->blackPieces == (board->blackPawns | board->blackKings) &&
			board->whitePieces == (board->whitePawns | board->whiteKings)) {
		board->gamePhase = PHASE_ENDGAME;
		return;
	}
}
