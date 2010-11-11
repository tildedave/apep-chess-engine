#include "commonboards.h"
#include "board.h"
#include <stdlib.h>
#include <time.h>

extern int randomSeed;

BITBOARD knightMoves[64];
BITBOARD kingMoves[64];

// white and black pawns have different moves and attacks
BITBOARD pawnMovesWhite[64];
BITBOARD pawnMovesBlack[64];
BITBOARD pawnAttacksWhite[64];
BITBOARD pawnAttacksBlack[64];

BITBOARD plus1[64];
BITBOARD plus7[64];
BITBOARD plus8[64];
BITBOARD plus9[64];
BITBOARD minus1[64];
BITBOARD minus7[64];
BITBOARD minus8[64];
BITBOARD minus9[64];

// direction from offset to offset
short direction[64][64]; 

// and rotated bitboards for queens/rooks/bishops
BITBOARD rankAttacks[64][64];
BITBOARD fileAttacks[64][64];
BITBOARD diag45RAttacks[64][64];
BITBOARD diag45LAttacks[64][64];

// has a different index for each possible piece that can be at a possible location
BITBOARD zobristArray[12][64];
BITBOARD zobristWhiteToMove;
BITBOARD zobristWhiteCanCastleKingside;
BITBOARD zobristWhiteCanCastleQueenside;
BITBOARD zobristBlackCanCastleKingside;
BITBOARD zobristBlackCanCastleQueenside;
BITBOARD zobristEnPassantSquare;

BITBOARD MASK[64];
short firstOnePrecomputed[65536];
short lastOnePrecomputed[65536];

BITBOARD whitePassedPawnMask[64];
BITBOARD blackPassedPawnMask[64];

BITBOARD centerSquares;

short rotate90R[64] = {
		56, 48, 40, 32, 24, 16,  8,   0,
		57, 49, 41, 33, 25, 17,  9,   1,
		58, 50, 42, 34, 26, 18, 10,  2,
		59, 51, 43, 35, 27, 19, 11,  3,
		60, 52, 44, 36, 28, 20, 12,  4,
		61, 53, 45, 37, 29, 21, 13,  5,
		62, 54, 46, 38, 30, 22, 14,  6,
		63, 55, 47, 39, 31, 23, 15,  7
};

short rotate45R[64] = {
		0, 2, 5, 9, 14, 20, 27, 35,  
		1,  4,  8, 13, 19, 26, 34, 42, 
		3,  7, 12, 18, 25, 33, 41, 48,
		6, 11, 17, 24, 32, 40, 47, 53,  
		10, 16, 23, 31, 39, 46, 52, 57,
		15, 22, 30, 38, 45, 51, 56, 60,
		21, 29, 37, 44, 50, 55, 59, 62,
		28, 36, 43, 49, 54, 58, 61, 63
};

short rotate45L[64] = {
		28, 36, 43, 49, 54, 58, 61, 63,
		21, 29, 37, 44, 50, 55, 59, 62,
		15, 22, 30, 38, 45, 51, 56, 60,
		10, 16, 23, 31, 39, 46, 52, 57,
		6, 11, 17, 24, 32, 40, 47, 53,  
		3,  7, 12, 18, 25, 33, 41, 48,
		1,  4,  8, 13, 19, 26, 34, 42, 
		0, 2, 5, 9, 14, 20, 27, 35
};

short shiftRank[64] = {
		1,  1,  1,  1,  1,  1,  1,  1, 
		9,  9,  9,  9,  9,  9,  9,  9, 
		17, 17, 17, 17, 17, 17, 17, 17, 
		25, 25, 25, 25, 25, 25, 25, 25, 
		33, 33, 33, 33, 33, 33, 33, 33, 
		41, 41, 41, 41, 41, 41, 41, 41, 
		49, 49, 49, 49, 49, 49, 49, 49,
		57, 57, 57, 57, 57, 57, 57, 57
};

short shiftFile[64] = {
		57, 49, 41, 33, 25, 17, 9, 1, 
		57, 49, 41, 33, 25, 17, 9, 1, 
		57, 49, 41, 33, 25, 17, 9, 1, 
		57, 49, 41, 33, 25, 17, 9, 1, 
		57, 49, 41, 33, 25, 17, 9, 1, 
		57, 49, 41, 33, 25, 17, 9, 1, 
		57, 49, 41, 33, 25, 17, 9, 1, 
		57, 49, 41, 33, 25, 17, 9, 1
};

short shift45R[64] = {
		1,  2,  4,  7, 11, 16, 22, 29,
		2,  4,  7, 11, 16, 22, 29, 36,
		4,  7, 11, 16, 22, 29, 36, 42,
		7, 11, 16, 22, 29, 36, 42, 47,
		11, 16, 22, 29, 36, 42, 47, 51,
		16, 22, 29, 36, 42, 47, 51, 54,
		22, 29, 36, 42, 47, 51, 54, 56,
		29, 36, 42, 47, 51, 54, 56, 57
};

short shift45L[64] = {
		29, 36, 42, 47, 51, 54, 56, 57,
		22, 29, 36, 42, 47, 51, 54, 56,
		16, 22, 29, 36, 42, 47, 51, 54,
		11, 16, 22, 29, 36, 42, 47, 51,
		7, 11, 16, 22, 29, 36, 42, 47,
		4,  7, 11, 16, 22, 29, 36, 42,
		2,  4,  7, 11, 16, 22, 29, 36,
		1,  2,  4,  7, 11, 16, 22, 29
};

void initialize_common_boards() {
	initializeZobristArray();
	initializeMask();
	initializeFirstOne();
	initializeKingMoves();
	initializeKnightMoves();
	initializePawnMoves();
	initializeDirectionMasks();
	initializeDirection();
	initializeFileAttacks();
	initializeRankAttacks();
	initializeDiag45RAttacks();
	initializeDiag45LAttacks();

	initializePassedPawnMask();

	loadBoardFromFEN(&defaultStartingBoard, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	centerSquares = 0;
	centerSquares |= offset_to_mask(D3);
	centerSquares |= offset_to_mask(E3);
	centerSquares |= offset_to_mask(D4);
	centerSquares |= offset_to_mask(E4);
	centerSquares |= offset_to_mask(D5);
	centerSquares |= offset_to_mask(E5);
	centerSquares |= offset_to_mask(D6);
	centerSquares |= offset_to_mask(E6);
}

void initializeZobristArray() {
	int seed = time(NULL);
	if (randomSeed != -1)
		seed = randomSeed;
	cerr << "seed: " << seed << endl;
#ifdef WIN32
  srand(seed);
#else
  srandom(seed);
  // not sure if we need this
  srand(seed);
#endif
  for(int i = 0; i < 12; ++i) {
    for(int j = 0; j < 64; ++j) {
      zobristArray[i][j] = random_int64();
    }
  }

  zobristWhiteToMove = random_int64();
  zobristWhiteCanCastleKingside = random_int64();
  zobristWhiteCanCastleQueenside = random_int64();
  zobristBlackCanCastleKingside = random_int64();
  zobristBlackCanCastleQueenside = random_int64();
  zobristEnPassantSquare = random_int64();
}

void initializeMask() {
	for(int i = 0; i < 64; ++i) {
		MASK[i] = ((BITBOARD) 1) << i;
	}
}

void initializeFirstOne() {
	int i = 0;
	
	firstOnePrecomputed[0] = -1;
	for(i = 1; i < 65536; ++i) {
	    for(int k = 0; k < 64; ++k) {
			if ((i & ((BITBOARD) 1 << k)) != 0) { 
				firstOnePrecomputed[i] = k;
				break;
			}
		}
	}

	lastOnePrecomputed[0] = -1;
	for(i = 1; i < 65536; ++i) {
	    for(int k = 63; k >= 0; --k) {
			if ((i & ((BITBOARD) 1 << k)) != 0) { 
				lastOnePrecomputed[i] = k;
				break;
			}
		}
	}
}

void initializeKnightMoves() {
//	knightMoves = new BITBOARD[64];
	// got tips on this from Beowulf
	for(int offset = 0; offset < 64; ++offset) {
		int y = Rank(offset);
		int x = File(offset);

		BITBOARD sqKnightMoves = 0;
		if (y > 0) {
			// can go down
			if (y > 1) {
				// can go down 2, side by 1
				if (x > 0) {
					sqKnightMoves |= offset_to_mask(offset - 17);
				}
				if (x < 7) {
					sqKnightMoves |= offset_to_mask(offset - 15);
				}
			}
			//can go down 1, side by 2
			if (x > 1)
				sqKnightMoves |= offset_to_mask(offset - 10);
			if (x < 6)
				sqKnightMoves |= offset_to_mask(offset - 6);
		}

		if (y < 7) {
			// can go up
			if (y < 6) {
				// can go up 2, side by 1
				if (x > 0)
					sqKnightMoves |= offset_to_mask(offset + 15);
				if (x < 7)
					sqKnightMoves |= offset_to_mask(offset + 17);
			}
			//can go up 1, side by 2
			if (x > 1)
				sqKnightMoves |= offset_to_mask(offset + 6);
			if (x < 6)
				sqKnightMoves |= offset_to_mask(offset + 10);
		}

		knightMoves[offset] = sqKnightMoves;
	}
}

void initializeKingMoves() {
//	kingMoves = new BITBOARD[64];
	for(int offset = 0; offset < 64; ++offset) {
		int x = File(offset);
		int y = Rank(offset);

		BITBOARD sqKingMoves = 0;
		bool canGoDown = (y > 0);
		bool canGoUp = (y < 7);
		bool canGoLeft = (x > 0);
		bool canGoRight = (x < 7);

		if (canGoDown) {
			if (canGoRight) {
				sqKingMoves |= offset_to_mask(offset - 8 + 1);
			}
			if (canGoLeft) {
				sqKingMoves |= offset_to_mask(offset - 8 - 1);
			}
			sqKingMoves |= offset_to_mask(offset - 8);
		}

		if (canGoRight) {
			sqKingMoves |= offset_to_mask(offset + 1);
		}
		if (canGoLeft) {
			sqKingMoves |= offset_to_mask(offset - 1);
		}

		if (canGoUp) {
			if (canGoRight) {
				sqKingMoves |= offset_to_mask(offset + 8 + 1);
			}
			if (canGoLeft) {
				sqKingMoves |= offset_to_mask(offset + 8 - 1);
			}
			sqKingMoves |= offset_to_mask(offset + 8);
		}

		kingMoves[offset] = sqKingMoves;
		//			System.err.println("King moves for " + new Square(x,y));
		//			outputBitBoard(sqKingMoves);
	}
}

void initializePawnMoves() {
//	pawnMovesWhite = new BITBOARD[64];
//	pawnMovesBlack = new BITBOARD[64];
//	pawnAttacksWhite = new BITBOARD[64];
//	pawnAttacksBlack = new BITBOARD[64];

	for(int offset = 0; offset < 64; ++offset) {
		BITBOARD sqPawnMovesWhite = 0;
		BITBOARD sqPawnMovesBlack = 0;
		BITBOARD sqPawnAttacksWhite = 0;
		BITBOARD sqPawnAttacksBlack = 0;

		int x = File(offset);
		int y = Rank(offset);

		// first initialize attacks
		if (x > 0) { 
			// can capture left
			if (y < 7) {
				// black can capture down-left
				sqPawnAttacksBlack |= offset_to_mask(offset + 8 - 1);
			}
			if (y > 0) {
				// white can capture up-left
				sqPawnAttacksWhite |= offset_to_mask(offset - 8 - 1);
			}
		}
		if (x < 7) {
			// can capture right
			if (y < 7) {
				// black can capture down-right
				sqPawnAttacksBlack |= offset_to_mask(offset + 8 + 1);
			}
			if (y > 0) {
				// white can capture up-left
				sqPawnAttacksWhite |= offset_to_mask(offset - 8 + 1);
			}
		}

		//			System.err.println("black pawn attacks for " + new Square(x,y));
		//			outputBitBoard(sqPawnAttacksBlack);
		//			System.err.println("white pawn attacks for " + new Square(x,y));
		//			outputBitBoard(sqPawnAttacksWhite);

		pawnAttacksBlack[offset] = sqPawnAttacksBlack;
		pawnAttacksWhite[offset] = sqPawnAttacksWhite;

		if (y != 0 && y != 7) {
			sqPawnMovesBlack |= offset_to_mask(offset + 8);
			sqPawnMovesWhite |= offset_to_mask(offset - 8);
			if (y == 1) {
				sqPawnMovesBlack |= offset_to_mask(offset + 16); 
			}
			if (y == 6) {
				sqPawnMovesWhite |= offset_to_mask(offset - 16); 
			}
		}

		pawnMovesBlack[offset] = sqPawnMovesBlack;
		pawnMovesWhite[offset] = sqPawnMovesWhite;
		
	}
}

void initializeDirectionMasks() {
	for(int offset = 0; offset < 64; ++offset) {
		BITBOARD directionMask = 0;
		for (int inc = 1; inc + File(offset) < 8; ++inc)
			directionMask |= offset_to_mask(offset + inc);

		plus1[offset] = directionMask;
	}
	for(int offset = 0; offset < 64; ++offset) {
		BITBOARD directionMask = 0;
		for (int inc = 1; File(offset) - inc >= 0 &&
		inc + Rank(offset) < 8; ++inc)
			directionMask |= offset_to_mask(offset + (inc * 7));

		plus7[offset] = directionMask;
	}
	for(int offset = 0; offset < 64; ++offset) {
		BITBOARD directionMask = 0;
		for (int inc = 1; inc + Rank(offset) < 8; ++inc)
			directionMask |= offset_to_mask(offset + (inc * 8));

		plus8[offset] = directionMask;
	}
	for(int offset = 0; offset < 64; ++offset) {
		BITBOARD directionMask = 0;
		for (int inc = 1; inc + File(offset) < 8 &&
		inc + Rank(offset) < 8; ++inc)
			directionMask |= offset_to_mask(offset + (inc * 9));

		plus9[offset] = directionMask;
	}
	for(int offset = 0; offset < 64; ++offset) {
		BITBOARD directionMask = 0;
		for (int inc = 1; -inc + File(offset) >= 0; ++inc)
			directionMask |= offset_to_mask(offset - inc);

		minus1[offset] = directionMask;
	}
	for(int offset = 0; offset < 64; ++offset) {
		BITBOARD directionMask = 0;
		for (int inc = 1; File(offset) + inc < 8 &&
		Rank(offset) - inc >= 0; ++inc)
			directionMask |= offset_to_mask(offset + (inc * -7));

		minus7[offset] = directionMask;
	}
	for(int offset = 0; offset < 64; ++offset) {
		BITBOARD directionMask = 0;
		for (int inc = 1; Rank(offset) - inc >= 0; ++inc)
			directionMask |= offset_to_mask(offset + (inc * -8));

		minus8[offset] = directionMask;
	}
	for(int offset = 0; offset < 64; ++offset) {
		BITBOARD directionMask = 0;
		for (int inc = 1; File(offset) - inc >= 0 &&
		Rank(offset) - inc >= 0; ++inc)
			directionMask |= offset_to_mask(offset + (inc * -9));

		minus9[offset] = directionMask;
	}
}

inline void fillInDirection(int i, BITBOARD ray, int dir) {
	while (ray != 0) {
		short j = FirstOne(ray);
		ray ^= offset_to_mask(j);
		direction[i][j] = (short) dir;
	}
}

void initializeDirection() {
	// direction[x][y] contains the direction from x to y (+1, +7, +8, +9, -1, -7, -8, -9), 
	// or zero otherwise.
	// got this idea from crafty
	for(int i = 0; i < 64; ++i) {
		fillInDirection(i, plus1[i], 1);
		fillInDirection(i, plus7[i], 7);
		fillInDirection(i, plus8[i], 8);
		fillInDirection(i, plus9[i], 9);
		fillInDirection(i, minus1[i], -1);
		fillInDirection(i, minus7[i], -7);
		fillInDirection(i, minus8[i], -8);
		fillInDirection(i, minus9[i], -9);
	}
}

// got this from http://www.atlanchess.com/html/rotated_bitboards.html
void initializeRankAttacks() {
	for (int offset = 0; offset < 64; ++offset) {
		// loop through every possibility of the contents of the 
		// middle 6 members of the board
		for(int contents = 0; contents < 64; ++contents) {
			rankAttacks[offset][contents] = 0;
			BITBOARD bits = plus1[offset];

			while (bits != 0) {
				// for each thing in the contents
				int sq = FirstOne(bits);

				// pretty sure we still want file here.
				int col = File(sq);

				// remove this one
				bits ^= MASK[sq];

				rankAttacks[offset][contents] |= MASK[sq];
				if (((contents << 1) & MASK[col]) != 0) {
					break;
				}

				//rankAttacks[offset][contents] |= MASK[sq];
			}
			bits = minus1[offset];
			while (bits != 0) {
				// for each thing in the contents
				int sq = LastOne(bits);
				int col = File(sq);
				// remove this one
				bits ^= MASK[sq];

				rankAttacks[offset][contents] |= MASK[sq];
				// see if we match up
				if (((contents << 1) & MASK[col]) != 0) {
					break;
				}
			}
		}
	}
}

void initializeFileAttacks() {
	for (int offset = 0; offset < 64; ++offset) {
		// loop through every possibility of the contents of the 
		// middle 6 members of the board
		for(int contents = 0; contents < 64; ++contents) {
			fileAttacks[offset][contents] = 0;
			BITBOARD bits = plus8[offset];
			while (bits != 0) {
//				cerr << "bits: " << bits << endl;
				// for each thing in the contents
				int sq = FirstOne(bits);
				int col = Rank(sq);

				// remove this one
				bits ^= MASK[sq];
				fileAttacks[offset][contents] |= MASK[sq];
				if (((contents << 1) & MASK[col]) != 0) {
					break;
				}
			}
			bits = minus8[offset];
			while (bits != 0) {
				// for each thing in the contents
//				cerr << "bits: " << bits << endl;
				int sq = LastOne(bits);
				int col = Rank(sq);
				// remove this one
				bits ^= MASK[sq];
				fileAttacks[offset][contents] |= MASK[sq];
				if (((contents << 1) & MASK[col]) != 0) {
					break;
				}
			}
//			cerr << "contents " << contents << " for " << offset_to_string(offset) << endl;
//			cerr << bitboard_to_string(fileAttacks[offset][contents]) << endl;
		}
	}
}

void initializeDiag45RAttacks() {
	for (int offset = 0; offset < 64; ++offset) {
		// loop through every possibility of the contents of the 
		// middle 6 members of the board
		for(int contents = 0; contents < 64; ++contents) {
			diag45RAttacks[offset][contents] = 0;
			BITBOARD bits = plus7[offset];
			while (bits != 0) {
				// for each thing in the contents
				int sq = FirstOne(bits);
				int col = File(sq);

				// remove this one
				bits ^= MASK[sq];
				diag45RAttacks[offset][contents] |= MASK[sq];
				if (((contents << 1) & MASK[col]) != 0) {
					break;
				}
			}
			bits = minus7[offset];
			while (bits != 0) {
				// for each thing in the contents
				int sq = LastOne(bits);
				int col = File(sq);
				// remove this one
				bits ^= MASK[sq];
				diag45RAttacks[offset][contents] |= MASK[sq];
				if (((contents << 1) & MASK[col]) != 0) {
					break;
				}
			}
		}
	}
}

void initializeDiag45LAttacks() {
	for (int offset = 0; offset < 64; ++offset) {
		// loop through every possibility of the contents of the 
		// middle 6 members of the board
		for(int contents = 0; contents < 64; ++contents) {
			diag45LAttacks[offset][contents] = 0;
			BITBOARD bits = plus9[offset];
			while (bits != 0) {
				// for each thing in the contents
				int sq = FirstOne(bits);
				int col = File(sq);

				// remove this one
				bits ^= MASK[sq];
				diag45LAttacks[offset][contents] |= MASK[sq];
				if (((contents << 1) & MASK[col]) != 0) {
					break;
				}
			}
			bits = minus9[offset];
			while (bits != 0) {
				// for each thing in the contents
				int sq = LastOne(bits);
				int col = File(sq);
				// remove this one
				bits ^= MASK[sq];
				diag45LAttacks[offset][contents] |= MASK[sq];
				if (((contents << 1) & MASK[col]) != 0) {
					break;
				}
			}
		}
	}
}

void initializePassedPawnMask() {
	// white pawn on A2 is passed if no black pawn on [A-B][3-7]
	for(int file = 0; file < 8; ++file) {
		for(int rank = 1; rank < 7; ++rank) {
			BITBOARD theBoard = 0;
			if (file > 0) {
				// check in the file to left
				int currentFile = file - 1;
				int leftAheadRank = rank + 1;
				while(leftAheadRank <= 7) {
					leftAheadRank++;
					theBoard |= offset_to_mask(get_offset(currentFile, 8 - leftAheadRank));
				}
			}
			// check the file ahead of us

			int aheadRank = rank + 1;
			while(aheadRank <= 7) {
				aheadRank++;
				theBoard |= offset_to_mask(get_offset(file, 8 - aheadRank));
			}

			if (file < 7) {
				// check in the file to the right

				int currentFile = file + 1;
				int rightAheadRank = rank + 1;

				while(rightAheadRank <= 7) {
					rightAheadRank++;
					theBoard |= offset_to_mask(get_offset(currentFile, 8 - rightAheadRank));
				}
			}

			short currentOffset = get_offset(file, 7 - rank);
			whitePassedPawnMask[currentOffset] = theBoard;
		}
	}

	for(int file = 0; file < 8; ++file) {
		for(int rank = 7; rank > 1; --rank) {
			BITBOARD theBoard = 0;
			short currentOffset = get_offset(file, 8 - rank);

			if (file > 0) {
				// check in the file to left
				int currentFile = file - 1;
				int leftAheadRank = rank;
				while(leftAheadRank > 1) {
					leftAheadRank--;
					theBoard |= offset_to_mask(get_offset(currentFile, 8 - leftAheadRank));
				}
			}
			// check the file ahead of us

			int aheadRank = rank;
			while(aheadRank > 1) {
				aheadRank--;
				theBoard |= offset_to_mask(get_offset(file, 8 - aheadRank));
			}

			if (file < 7) {
				// check in the file to the right

				int currentFile = file + 1;
				int rightAheadRank = rank;

				while(rightAheadRank > 1) {
					rightAheadRank--;
					theBoard |= offset_to_mask(get_offset(currentFile, 8 - rightAheadRank));
				}
			}

			blackPassedPawnMask[currentOffset] = theBoard;
		}
	}
}