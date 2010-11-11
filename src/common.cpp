#include "common.h"
#include <sstream>

extern short firstOnePrecomputed[65536];
extern short lastOnePrecomputed[65536];

const BITBOARD firstOne_mask1 = 0xFFFF000000000000ULL;
const BITBOARD firstOne_mask2 = 0x0000FFFF00000000ULL;
const BITBOARD firstOne_mask3 = 0x00000000FFFF0000ULL;
const BITBOARD firstOne_mask4 = 0x000000000000FFFFULL;
//
//BITBOARD offset_to_mask(short i) {
//	if (i >= 0 && i < 64) {
//		return mask[i];
//	}
//	else {
//		return ((BITBOARD) 1) << i;
//	}
//}

static const short offset_vflip_table[] = {
	56, 57, 58, 59, 60, 61, 62, 63,
	48, 49, 50, 51, 52, 53, 54, 55,
	40, 41, 42, 43, 44, 45, 46, 47,
	32, 33, 34, 35, 36, 37, 38, 39,
	24, 25, 26, 27, 28, 29, 30, 31,
	16, 17, 18, 19, 20, 21, 22, 23,
	8,   9, 10, 11, 12, 13, 14, 15,
	0,   1,  2,  3,  4,  5,  6,  7
};

short offset_vflip(short offset) {
	return offset_vflip_table[offset];
}

short FirstOneNew(BITBOARD l) {
	if (!l)
		return -1;
	if (l & 65535)
		return (firstOnePrecomputed[l & 65535]);
	else if ((l >> 16) & 65535) { 
		return (firstOnePrecomputed[(l >> 16) & 65535] + 16);
	}
	else if ((l >> 32) & 65535) {
		return (firstOnePrecomputed[(l >> 32) & 65536] + 32);
	}
	else {
		return (firstOnePrecomputed[(l >> 48) & 65536] + 48);
	}
}

short FirstOne(BITBOARD l) {
 if ((l & firstOne_mask4) != 0) {
    return (short) firstOnePrecomputed[(int) (l & firstOne_mask4)];
  }
  else if ((l & firstOne_mask3) != 0) {
    return (short) (firstOnePrecomputed[(int) ((l & firstOne_mask3) >> 16)] + 16);
  }
  else if ((l & firstOne_mask2) != 0) { 
    return (short) (firstOnePrecomputed[(int) ((l & firstOne_mask2) >> 32)] + 32);
  }
  //else if ((l & firstOne_mask1) != 0) {
  else {
    return (short) (firstOnePrecomputed[(int) ((l & firstOne_mask1) >> 48)] + 48);
  }
  return -1;
	
//	if (!l)
//		return -1;
//	if (l & 65535) 
//		return (firstOnePrecomputed[l & 65535]);
//	else if ((l >> 16) & 65535) 
//		return (firstOnePrecomputed[(l >> 16) & 65535] + 16);
//	else if ((l >> 32) & 65535)
//		return (firstOnePrecomputed[(l >> 32) & 65536] + 32);
//	else
//		return (firstOnePrecomputed[(l >> 48) & 65536] + 48);
}

short LastOne(BITBOARD l) {
//	for(int i = 63; i >= 0; --i) {
//		if ((l & ((BITBOARD) 1 << i)) != 0) 
//			return i;
//	}

//	return -1;
	  if (l >> 48)
	    return (lastOnePrecomputed[l >> 48]) + 48;
	  if ((l >> 32) & 65535)
	    return (lastOnePrecomputed[(l >> 32) & 65535] + 32);
	  if ((l >> 16) & 65535)
	    return (lastOnePrecomputed[(l >> 16) & 65535] + 16);
	  return (lastOnePrecomputed[l & 65535]);

}

short NumOnes(BITBOARD l) {
	int count = 0;
	while(l) {
		++count;
		l &= (l - 1);
	}
	return count;
}


std::string item_to_string(short contents) {
	switch (contents) {
	case EMPTY :
		return ".";
	case PAWN_WHITE :
		return "P";
	case PAWN_BLACK :
		return "p";
	case BISHOP_WHITE :
		return "B";
	case BISHOP_BLACK :
		return "b";
	case KNIGHT_WHITE :
		return "N";
	case KNIGHT_BLACK :
		return "n";
	case ROOK_WHITE:
		return "R";
	case ROOK_BLACK:
		return "r";
	case QUEEN_WHITE:
		return "Q";
	case QUEEN_BLACK:
		return "q";
	case KING_WHITE:
		return "K";
	case KING_BLACK:
		return "k";		
	}
	
	return "?";
}

std::string bitboard_to_string(BITBOARD bitboard) {
	std::string return_string;
	
	for(int y = 0; y < 8; ++y) {
		for(int x = 0; x < 8; ++x) {
			BITBOARD offset = ((BITBOARD) 1) << (8 * y + x);
			if ((bitboard & offset) == 0) {
				return_string += '0';
			}
			else  {
				return_string += '1';
			}
		}
		return_string += '\n';
	}
	
	return return_string;
}

std::string offset_to_string(short offset) {
	int rank = 8 - (offset / 8);
	int file = offset % 8;
	
	std::string returnString = "";
	
	returnString += file_to_string(file);
	returnString += int_to_string(rank);
	
	return returnString;
}

std::string file_to_string(short file) {
	switch (file) {
	case 0:
		return "a";
	case 1:
		return "b";
	case 2:
		return "c";
	case 3:
		return "d";
	case 4:
		return "e";
	case 5:
		return "f";
	case 6:
		return "g";
	case 7:
		return "h";
	}
	
	return "?";
}

short get_offset(int file, int rank) {
	return 8 * rank + file;
}

std::string int_to_string(int i) {
	std::ostringstream ss;
	ss << i;
	return ss.str();
}

std::string BITBOARD_to_string(BITBOARD i) {
	std::ostringstream ss;
	ss << i;
	return ss.str();
}

int string_to_int(const std::string& s) {
	std::istringstream ss(s);
	int n;
	ss >> n;
	return n;
}


bool is_digit(char c) {
	switch (c) {
	case '0': 
		return true;
	case '1': 
		return true;
	case '2': 
		return true;
	case '3': 
		return true;
	case '4': 
		return true;
	case '5': 
		return true;
	case '6': 
		return true;
	case '7': 
		return true;
	case '8': 
		return true;
	case '9': 
		return true;
	}
	
	return false;
}

int char_to_int(char c) {
	switch (c) {
	case '0': 
		return 0;
	case '1': 
		return 1;
	case '2': 
		return 2;
	case '3': 
		return 3;
	case '4': 
		return 4;
	case '5': 
		return 5;
	case '6': 
		return 6;
	case '7': 
		return 7;
	case '8': 
		return 8;
	case '9': 
		return 9;
	}
	
	return 0;
}

short piece_from_char(char c) {
	switch (c) {
	case 'P':
		return PAWN_WHITE;
	case 'p':
		return PAWN_BLACK;
	case 'N':
		return KNIGHT_WHITE;
	case 'n':
		return KNIGHT_BLACK;
	case 'B':
		return BISHOP_WHITE;
	case 'b':
		return BISHOP_BLACK;
	case 'R':
		return ROOK_WHITE;
	case 'r':
		return ROOK_BLACK;
	case 'Q':
		return QUEEN_WHITE;
	case 'q':
		return QUEEN_BLACK;
	case 'K':
		return KING_WHITE;
	case 'k':
		return KING_BLACK;
	}
	
	return EMPTY;
}

BITBOARD random_int64() {
  BITBOARD randomInt64 = 0;
#ifdef WIN32
  BITBOARD lowerBits = rand();
  BITBOARD upperBits = rand();
#else
  BITBOARD lowerBits = random();
  BITBOARD upperBits = random();
#endif
  randomInt64 |= lowerBits;
  randomInt64 |= (upperBits << 32);
  return randomInt64;
}

std::string int6_to_string(int i) {
	std::string returnString;
	for(int j = 5; j > 0; --j) {
		if ((1 << j) & i)
			returnString += "1";
		else
			returnString += "0";
	}
	
	return returnString;
}

short file_to_column(char file) {
	switch(file) {
	case 'a':
		return 0;
	case 'b': 
		return 1;
	case 'c': 
		return 2;
	case 'd': 
		return 3;
	case 'e': 
		return 4;
	case 'f': 
		return 5;
	case 'g': 
		return 6;
	case 'h': 
		return 7;
	}
	
	return -1;
}
short string_to_offset(const std::string& str) {
	char fileChar = str[0];
	char rankChar = str[1];
	int rank = 8 - char_to_int(rankChar);
	int file = file_to_column(fileChar);
	
	return get_offset(file, rank);  
}

#ifdef _MSC_VER
    #include <intrin.h>
    #ifdef _WIN64
        #pragma intrinsic(_BitScanForward64)
        #pragma intrinsic(_BitScanReverse64)
        #define USING_INTRINSICS
    #endif
#elif defined(__GNUC__) && defined(__LP64__)
    static INLINE unsigned char _BitScanForward64(unsigned int* const Index, const U64 Mask)
    {
        U64 Ret;
        __asm__
        (
            "bsfq %[Mask], %[Ret]"
            :[Ret] "=r" (Ret)
            :[Mask] "mr" (Mask)
        );
        *Index = (unsigned int)Ret;
        return Mask?1:0;
    }
    static INLINE unsigned char _BitScanReverse64(unsigned int* const Index, const U64 Mask)
    {
        U64 Ret;
        __asm__
        (
            "bsrq %[Mask], %[Ret]"
            :[Ret] "=r" (Ret)
            :[Mask] "mr" (Mask)
        );
        *Index = (unsigned int)Ret;
        return Mask?1:0;
    }
    #define USING_INTRINSICS
#endif
