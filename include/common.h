#ifndef COMMON_H_
#define COMMON_H_

// I don't think that using the STL will have a major impact on our speed,  
// since most of these functions are only used for debugging. 
#include <string>
#include <iostream>
#include <limits.h>
#include <stdlib.h>

using std::string;
using std::cout;
using std::cerr;
using std::endl;

#ifdef WIN32
typedef unsigned __int64 BITBOARD;
#else
typedef unsigned long long BITBOARD;
#endif
typedef BITBOARD HASHKEY;

#define MAX_BITBOARD ULONG_LONG_MAX

#define INFINITE_VALUE		INT_MAX

#define EMPTY (0)

#define PAWN			(1)
#define KNIGHT			(2)
#define KING			(3)
#define BISHOP			(5)
#define ROOK			(6)
#define QUEEN			(7)


#define PAWN_WHITE 		(1)
#define KNIGHT_WHITE 	(2)
#define KING_WHITE 		(3)
#define BISHOP_WHITE 	(5)
#define ROOK_WHITE 		(6)
#define QUEEN_WHITE		(7)

#define PAWN_BLACK 		(-1)
#define KNIGHT_BLACK 	(-2)
#define KING_BLACK 		(-3)
#define BISHOP_BLACK 	(-5)
#define ROOK_BLACK 		(-6)
#define QUEEN_BLACK 	(-7)

#define WHITE 			1
#define BLACK			-1

#define File(x)			(x & 7)
#define Rank(x)			(x >> 3)
#define IsPieceWhite(x) (x > 0)

#define IsPawn(x)		(x == PAWN_WHITE || x == PAWN_BLACK)
#define IsKnight(x)		(x == KNIGHT_WHITE || x == KNIGHT_BLACK)
#define IsBishop(x)		(x == BISHOP_WHITE || x == BISHOP_BLACK)
#define IsRook(x)		(x == ROOK_WHITE || x == ROOK_BLACK)
#define IsQueen(x)		(x == QUEEN_WHITE || x == QUEEN_BLACK)
#define IsKing(x)		(x == KING_WHITE || x == KING_BLACK)

#define SEARCH_START		0
#define SEARCH_NULLMOVE 	1
#define SEARCH_HASHMOVE 	2
#define SEARCH_ALLMOVES 	3
#define SEARCH_DONE 		4

#define PHASE_OPENING		1
#define PHASE_MIDDLEGAME	2
#define PHASE_ENDGAME		3

#define A8 0
#define B8 1
#define C8 2
#define D8 3
#define E8 4
#define F8 5
#define G8 6
#define H8 7
#define A7 8
#define B7 9
#define C7 10
#define D7 11
#define E7 12
#define F7 13
#define G7 14
#define H7 15
#define A6 16
#define B6 17
#define C6 18
#define D6 19
#define E6 20
#define F6 21
#define G6 22
#define H6 23
#define A5 24
#define B5 25
#define C5 26
#define D5 27
#define E5 28
#define F5 29
#define G5 30
#define H5 31
#define A4 32
#define B4 33
#define C4 34
#define D4 35
#define E4 36
#define F4 37
#define G4 38
#define H4 39
#define A3 40
#define B3 41
#define C3 42
#define D3 43
#define E3 44
#define F3 45
#define G3 46
#define H3 47
#define A2 48
#define B2 49
#define C2 50
#define D2 51
#define E2 52
#define F2 53
#define G2 54
#define H2 55
#define A1 56
#define B1 57
#define C1 58
#define D1 59
#define E1 60
#define F1 61
#define G1 62
#define H1 63

#define RANK1 0xFF00000000000000ULL
#define RANK2 0x00FF000000000000ULL
#define RANK3 0x0000FF0000000000ULL
#define RANK4 0x000000FF00000000ULL
#define RANK5 0x00000000FF000000ULL
#define RANK6 0x0000000000FF0000ULL
#define RANK7 0x000000000000FF00ULL
#define RANK8 0x00000000000000FFULL

#define FILE1 0x8080808080808080ULL
#define FILE2 0x4040404040404040ULL
#define FILE3 0x2020202020202020ULL
#define FILE4 0x1010101010101010ULL
#define FILE5 0x0808080808080808ULL
#define FILE6 0x0404040404040404ULL
#define FILE7 0x0202020202020202ULL
#define FILE8 0x0101010101010101ULL

#define MAX_MOVES 		255
#define MAX_DEPTH 		50
#define MAX_BRANCH 		120

extern const BITBOARD firstOne_mask1;
extern const BITBOARD firstOne_mask2;
extern const BITBOARD firstOne_mask3;
extern const BITBOARD firstOne_mask4;
extern short firstOnePrecomputed[65536];
extern BITBOARD MASK[64];

// COMMON BIT FUNCTIONS
//BITBOARD offset_to_mask(short offset);
#define offset_to_mask(x) MASK[x]
short FirstOne(BITBOARD l);
short FirstOneNew(BITBOARD item);
short LastOne(BITBOARD item);
short NumOnes(BITBOARD l);

short offset_vflip(short offset);

// INPUT/OUTPUT FUNCTIONS
std::string offset_to_string(short offset);
std::string file_to_string(short file);
std::string item_to_string(short item);
std::string bitboard_to_string(BITBOARD bitboard);
std::string int6_to_string(int i);
short string_to_offset(const std::string& str);
short file_to_column(char file);

short get_offset(int file, int rank);
short piece_from_char(char c);

// UTILITY FUNCTIONS (I miss you, JRE)
std::string int_to_string(int i);
std::string BITBOARD_to_string(BITBOARD i);
int string_to_int(const std::string& s);
bool is_digit(char c);
int char_to_int(char c);
BITBOARD random_int64();

// LOGGING INITIALIZATION
void setupLogging(std::string filename = "apep.log");

class InternalEngineError {
	
};

#endif /*COMMON_H_*/
