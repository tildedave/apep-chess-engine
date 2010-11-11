#include "transpositiontable.h"

bool ancientSwitch = false;

TranspositionTable::TranspositionTable() {
	this->hashTable = new HashEntry*[HASH_SIZE];
	for(int i = 0; i < HASH_SIZE; ++i)
		this->hashTable[i] = NULL;
}

TranspositionTable::~TranspositionTable() {
	for(int i = 0; i < HASH_SIZE; ++i)
		if (this->hashTable[i] != NULL)
			delete (this->hashTable[i]);
	delete[] this->hashTable;
}

bool 
TranspositionTable::hasKey(HASHKEY zobristHashKey) {
		return (this->hashTable[hash(zobristHashKey)] != NULL);
}

int 
TranspositionTable::probeHash(HASHKEY zobristHashKey, short depth, int alpha, int beta) {
	HashEntry * theEntry = getKey(zobristHashKey);

	if (theEntry != NULL) {
		if (theEntry->zobristHashKey == zobristHashKey) {
			if (theEntry->depth >= depth) {
				if (theEntry->flags == HASH_EXACT) {
					return theEntry->value;
				}
				if (theEntry->flags == HASH_ALPHA && theEntry->value <= alpha) {
					return alpha;
				}
				if (theEntry->flags == HASH_BETA && theEntry->value >= beta) {
					return beta;
				}
			}
			// remember best move?  how?
		}
	}

	return 0;
}

void 
TranspositionTable::recordHash(HASHKEY zobristHashKey, int bestMove, short depth, short value, short flags, search_statistics * stats) {
		HashEntry * he = getKey(zobristHashKey);
		
		if (he != NULL) {
			if (depth < he->depth && ancientSwitch == he->ancient) {
				return;
			}
			delete he;
		}
		he = new HashEntry();
		
		stats->hash_stores++;

		he->zobristHashKey = zobristHashKey;
		he->bestMove = bestMove;
		he->depth = depth;
		he->value = value;
		he->flags = flags;
		
		hashTable[hash(zobristHashKey)] = he;
	}

TranspositionTable::HashEntry* 
TranspositionTable::getKey(HASHKEY zobristHashKey) {
	int key = hash(zobristHashKey);
	if (key >= 0 && key < HASH_SIZE)
		return (hashTable[key]);
	else {
		cerr << "ERROR READING HASH KEY: " << key << " -- key is out of range of hash table!" << endl;
		return NULL;
	}
}

int 
TranspositionTable::hash(HASHKEY zobristHashKey) {
	int key = (int) (zobristHashKey % HASH_SIZE);
	return key;
}

bool 
TranspositionTable::hasValue(HASHKEY zobristHashKey, short depth, int alpha,
		int beta) {
	HashEntry * theEntry = getKey(zobristHashKey);

	if (theEntry != NULL) {
		if (theEntry->zobristHashKey == zobristHashKey) {
			if (theEntry->depth >= depth) {
				if (theEntry->flags == HASH_EXACT) {
					return true;
				}
				if (theEntry->flags == HASH_ALPHA && theEntry->value <= alpha) {
					return true;
				}
				if (theEntry->flags == HASH_BETA && theEntry->value >= beta) {
					return true;
				}
			}	
		}
	}

	return false;
}
	
int TranspositionTable::getMove(HASHKEY zobristHashKey) {
	HashEntry * he = getKey(zobristHashKey);
	if (he != NULL)
		return he->bestMove;
	return 0;
}

void 
TranspositionTable::clear() {
	for(int i = 0; i < HASH_SIZE; ++i) {
		delete this->hashTable[i];
		this->hashTable[i] = NULL;
	}
}