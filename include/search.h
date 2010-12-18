#ifndef SEARCH_H_
#define SEARCH_H_

#if defined(WIN32) || defined(__CYGWIN__)
#include <windows.h>
#endif


#include "board.h"
#include "movelinkedlist.h"
#include "movepriorityqueue.h"
#include "transpositiontable.h"

#include <set>

class TranspositionTable;
extern TranspositionTable transpositionTable;

#define HASH_DEBUG				0
#define TIMEOUT_VALUE			10
#define CHECK_INTERVAL			5000

typedef struct search_statistics {
	BITBOARD nodes;
	BITBOARD cutoffs;
	BITBOARD qnodes;
	BITBOARD qcutoffs;
	BITBOARD hash_hits;
	BITBOARD hash_stores;
	int qdepth;
	
	search_statistics() {
		nodes = 0;
		cutoffs = 0;
		qnodes = 0;
		qcutoffs = 0;
		hash_hits = 0;
		hash_stores = 0;
		qdepth = 0;
	}
} search_statistics;

typedef struct search_info {
#ifdef WIN32
	FILETIME startTime;
#else
	struct timeval startTime;
#endif
	MovePriorityQueue * lastOrdering;
	MovePriorityQueue * nextOrdering;
	bool lastMoveWasNullMove;
} search_info;

class TimeoutException {
};

typedef struct search_options {
  bool analysisMode;
  bool randomMode;
  bool noisyMode;
  bool outputNodesPerMove;
  float TimeoutValue;

  search_options() : analysisMode(false), randomMode(false), noisyMode(false), outputNodesPerMove(false) { }
} search_options;

extern int timeToNextCheck;
extern int currentSearchDepth;

extern TranspositionTable table;

int getMove(ChessBoard * board, search_options* options);
int getMoveFromSearch(ChessBoard * board, search_options* options);
int getMove_iterativeDeepening(ChessBoard * board, search_options* options);
bool shouldAbortIterativeDeepening(int score);
int alphaBetaSearch(ChessBoard * board,  
		    short startingDepth, short depthLeft, short ply,
		    bool isInitialCall, 
		    int alpha, int beta, 
		    search_info* searchInfo, 
		    search_statistics* stats,
		    search_options* options
		    );
void addKillerMove(ChessBoard * board, int currentMove, int depth);
int quiescentSearch(ChessBoard * board,  
		    int alpha, int beta, int qdepth, 
		    search_info* searchInfo, 
		    search_statistics * stats,
		    search_options* options);
void getMovesForBoard(ChessBoard * board, MovePriorityQueue& mpq);
void getQuiescentMovesForBoard(ChessBoard * board, MovePriorityQueue& capturesAndChecks);
void updateVariationWithLine(MoveLinkedList * variation, MoveLinkedList * line, int currentMove);

#ifdef WIN32
double getSecondsDiff(FILETIME * start, FILETIME * end);
#else
double getSecondsDiff(timeval * tv1, timeval * tv2);
#endif
#ifdef WIN32
void getCurrentTime(FILETIME * currentTime);
#else
void getCurrentTime(timeval * currentTime);
#endif
void checkTimeout(ChessBoard* board, search_info* searchInfo, search_statistics* stats, search_options* options);

// stats only output if noisy search is on
void outputStatsHeader(search_options* options);
void outputStats(ChessBoard * board, const search_statistics& stats, int depth, int score, MoveLinkedList& line, double diff, search_options* options);

void extractPV(ChessBoard * board, MoveLinkedList& line);
void extractPVHelper(ChessBoard * board, MoveLinkedList& line, std::set<BITBOARD>& zobristKeys);

int getNextMove(ChessBoard * board, bool whiteToMove, bool quiescentMoves, int ply);
int* getAndSortMovesForBoard(ChessBoard * board, bool whiteToMove, bool quiescentMoves, int ply);
int getPriorityForMove(ChessBoard * board, int move);

bool movePushesPawnToSeventhRank(ChessBoard * board, int move, bool whiteToMove);
bool movePushesPawnToSixthRank(ChessBoard * board, int move, bool whiteToMove);

bool checkForInputDuringSearch();

int getNextMove(ChessBoard * board, bool whiteToMove, bool quiescent, int ply);

#endif /*SEARCH_H_*/
