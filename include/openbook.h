#ifndef OPENBOOK_H_
#define OPENBOOK_H_

#include "common.h"
#include "board.h"

#include <map>

struct Coord {
	int move;
	int weight;
};

extern std::multimap<HASHKEY, Coord> openingBook;

int getMoveForPosition(ChessBoard * board);
bool loadOpeningBook(const std::string& file);

#endif