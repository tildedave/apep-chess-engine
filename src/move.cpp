#include "move.h"

bool isCapture(ChessBoard * board, short movePiece, short to) {
	if (board->boardContents[to] != EMPTY)
		return true;
	else if (movePiece == PAWN_WHITE && to == board->enPassantOffset)
		return true;
	else if (movePiece == PAWN_BLACK && to == board->enPassantOffset)
		return true;
	return false;
}

std::string MoveToString(int move) {
	if (move == -1)
		return "<HT>";
	
	short movePiece = GetMovePiece(move);
	short capturedPiece = GetCapturePiece(move);
	short from = GetFrom(move);
	short to = GetTo(move);
		
		
	if (movePiece == KING) {
		// if it moves to the right  
		if ((from - to) == E1 - G1)
			return "O-O";
		if ((from - to) == E1 - C1)
			return "O-O-O";
	}

	std::string str;
	if (capturedPiece == 0) {
		str = piece_to_notation_string(movePiece, to, false) + offset_to_string(to);
	}
	else {
		str = piece_to_notation_string(movePiece, from, true) + "x" + offset_to_string(to); 
	}
	
	short promotionPiece = GetPromotionPiece(move);
	if (promotionPiece != 0) {
		// don't need to worry about other arguments since you can't promote to pawn
		str += "=" + piece_to_notation_string(promotionPiece, 0, false);
	}
	
	return str;
}

std::string piece_to_notation_string(short contents, short offset, bool isCapture) {
	switch (contents) {
	case PAWN_WHITE:
	case PAWN_BLACK:
		if (isCapture) 
			return file_to_string(File(offset));
		else
			return "";
	case KNIGHT_WHITE:
	case KNIGHT_BLACK:
		return "N";
	case BISHOP_WHITE:
	case BISHOP_BLACK:
		return "B";
	case ROOK_WHITE:
	case ROOK_BLACK:
		return "R";
	case QUEEN_WHITE:
	case QUEEN_BLACK:
		return "Q";
	case KING_WHITE:
	case KING_BLACK:
		return "K";
	}
	
	return "?";
}

bool MoveIsLegal(ChessBoard * chessBoard, int currentMove) {
	short movePiece = GetMovePiece(currentMove);
	short from = GetFrom(currentMove);
	short to = GetTo(currentMove);
	short capturePiece = GetCapturePiece(currentMove);
	
	if (chessBoard->whiteToMove) {
		return chessBoard->boardContents[from] == movePiece && chessBoard->boardContents[to] == -capturePiece;
	}
	else {
		return chessBoard->boardContents[from] == -movePiece && chessBoard->boardContents[to] == capturePiece;
	}
}