#include "openbook.h"
#include "move.h"
#include "xboard.h"

#include <stdlib.h>
#include <memory.h>

#include <vector>
#include <utility>
#include <fstream>
#include <sstream>
#include <log4cplus/logger.h>

std::multimap<HASHKEY, Coord> openingBook;
extern log4cplus::Logger logger;

int getMoveForPosition(ChessBoard * board) {

	std::pair<std::multimap<HASHKEY, Coord>::iterator, 
			  std::multimap<HASHKEY, Coord>::iterator> positionStartEnd;

	cerr << board_to_string(board) << endl;
	positionStartEnd = openingBook.equal_range(board->zobristHashKey);

	std::vector<Coord> coords;
	int totalWeight = 0;

	for(std::multimap<HASHKEY, Coord>::iterator itPosition = positionStartEnd.first;
		itPosition != positionStartEnd.second; 
		++itPosition) {
		coords.push_back(itPosition->second);
		totalWeight += itPosition->second.weight;
	}

	// select a move given the weights
	if (coords.empty())
		return -1;

	Coord theCoord;

	if (coords.size() == 1)
		theCoord = coords.front();
	else {
		int whichOne = rand() % totalWeight;
		int weightSoFar = 0;
		for(std::vector<Coord>::iterator itCoords = coords.begin(); itCoords != coords.end(); ++itCoords) {
			if (itCoords->weight + weightSoFar > whichOne) {
				theCoord = *itCoords;
				break;
			}
			weightSoFar += itCoords->weight;
		}

		// convert it to a move
	}
	if (!MoveIsLegal(board, theCoord.move)) {
		cerr << "move from opening book: " << MoveToString(theCoord.move) << " is not legal!" << endl;
		return -1;
	}

	LOG4CPLUS_DEBUG(logger, "book move " << MoveToString(theCoord.move));	
	return theCoord.move;
}

bool loadOpeningBook(const std::string& file) {
	cerr << "loading opening book " << file << "..." << endl;

	//std::ofstream openBookData("openbookdata.txt");

	bool isFirst = true;

	std::ifstream inputStream(file.c_str());
	if (!inputStream) {
		cerr << "could not load opening book " << file << endl;
		return false;
	}

	while (!inputStream.eof()) {
		ChessBoard nextBoard;
		ChessBoard nextBoard2;
		memset(&nextBoard, 0, sizeof(ChessBoard));
		/*
		memset(&nextBoard2, 0, sizeof(ChessBoard));*/
		std::string fenString;
		std::getline(inputStream, fenString);

		if (fenString == "#END#")
			break;

		if (!loadBoardFromFEN(&nextBoard, fenString)) {
			cerr << "ERROR: opening book contains invalid FEN: " << fenString << endl;
			std::string burnString;
			std::getline(inputStream, burnString);
			continue;
		}

		loadBoardFromFEN(&nextBoard2, fenString);

		HASHKEY zobristKey = nextBoard.zobristHashKey;

		std::string coordString;
		std::getline(inputStream, coordString);

		std::stringstream coordInputStream(coordString);
		while(!coordInputStream.eof()) {
			std::string indivCoordString;
			coordInputStream >> indivCoordString;
			
			if (indivCoordString == "")
				continue;
			
			std::string::size_type openBrace = indivCoordString.find("{");
			std::string::size_type closeBrace = indivCoordString.find("}");
			std::string movePart = indivCoordString.substr(0, openBrace);
			std::string bracePart = indivCoordString.substr(openBrace + 1, (closeBrace - openBrace) - 1);

			//cerr << "move: " << movePart << " with weight of " << bracePart << endl;

			//cerr << "converting " << movePart << " to string from " << coordString << endl;
			int move = CoordStringToMove(&nextBoard, movePart);
			int weight = string_to_int(bracePart);

			Coord theCoord;
			theCoord.move = move;
			theCoord.weight = weight;

			//openBookData << board_to_string(&nextBoard) << endl << movePart << " vs " << MoveToString(move) << endl;

			openingBook.insert(std::pair<HASHKEY, Coord>(zobristKey, theCoord));
		}
	}

	cerr << "finished loading opening book " << file << endl;
	return true;
}
