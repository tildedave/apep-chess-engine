#ifndef TRANSPOSITIONTABLE_
#define TRANSPOSITIONTABLE_

#include "common.h"
#include "search.h"

// got this idea from mediocre chess.  switch
// the switch every move, so that the future
// values overwrite past ones
extern bool ancientSwitch;

struct search_statistics;

//#define HASH_SIZE 		262144
#define HASH_SIZE 		2097152
#define HASH_EXACT		0
#define HASH_ALPHA 		1
#define HASH_BETA 		2
#define HASH_UNKNOWN 	3

class TranspositionTable {
	
public:
	
	TranspositionTable();
	~TranspositionTable();
	void clear();
	
	int probeHash(HASHKEY zobristHashKey, short depth, int alpha, int beta);
	void recordHash(HASHKEY zobristHashKey, int bestMove, short depth, short value, short flags, search_statistics * stats);
	bool hasValue(HASHKEY zobristHashKey, short depth, int alpha, int beta);
	int getMove(HASHKEY zobristHashKey);
	
protected:

	struct HashEntry {
		HASHKEY zobristHashKey;
		int bestMove;
		short depth : 6;
		short flags : 3;
		bool ancient; 
		short value;
	};
	
	HashEntry** hashTable;
	
	HashEntry* getKey(HASHKEY zobristKey);
	bool hasKey(HASHKEY zobristKey);
	int hash(HASHKEY zobristKey);
	
private:
	// you are never, never, never allowed to use these
	// only one transposition table.  I don't want to 
	// contemplate a world where this is not the case
	TranspositionTable(const TranspositionTable& tt);
	TranspositionTable& operator=(TranspositionTable& tt);

};

#endif /*TRANSPOSITIONTABLE_*/
