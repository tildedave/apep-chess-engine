#include "search.h"
#include "eval.h"
#include "movegen.h"
#include "move.h"
#include "moveprocess.h"
#include "xboard.h"
#include "openbook.h"

#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include <iomanip>
#include <stack>
#include <map>
#include <csignal>

#include <sstream>
#include <log4cxx/logger.h>

#define DO_OLD_SORT 			0
#define SEARCH_DEBUG			0
#define HASH_MOVE				1

// transposition table
TranspositionTable transpositionTable;
int timeToNextCheck;
bool AnalysisMode = false;
float TimeoutValue = TIMEOUT_VALUE;
extern log4cxx::LoggerPtr logger;

int getMove(ChessBoard * board) {
	int bookMove = getMoveForPosition(board);
	if (bookMove == -1) {
		board->gamePhase = PHASE_MIDDLEGAME;
		return getMove(board, true);
	}
	else
		return bookMove;
}

int getMove(ChessBoard * board, bool noisy) {
	return getMove_iterativeDeepening(board, noisy);
}

int getMove_iterativeDeepening(ChessBoard * board, bool noisy) {
	MoveLinkedList line;
	
#ifdef WIN32
	SYSTEMTIME beginSystemTime;
	GetSystemTime(&beginSystemTime);
	FILETIME beginTime;
	SystemTimeToFileTime(&beginSystemTime, &beginTime);
#else
	struct timeval beginTime;
	gettimeofday(&beginTime, NULL);
#endif
	
	int initialScore = evaluateBoard<false>(board);
	int theMove = 0;
	int i = 0;
	search_info searchInfo;
	search_statistics stats;
	int alpha = -INFINITE_VALUE;
	int beta = INFINITE_VALUE;
	int ASPIRATION_WINDOW = 50;
	bool haveLastEvaluation = false;
	int lastEvaluation = 0;
	
	while(true) {
		i = i + 2;
#ifdef WIN32
		FILETIME start, end;
		SYSTEMTIME startSystemTime, endSystemTime;

		GetSystemTime(&startSystemTime);
		SystemTimeToFileTime(&startSystemTime, &start);
#else
		struct timeval start, end;
		gettimeofday(&start, NULL);
#endif

		searchInfo.lastOrdering = NULL;
		searchInfo.nextOrdering = NULL;
		searchInfo.startTime = beginTime;
		searchInfo.lastMoveWasNullMove = false;

		try {
			timeToNextCheck = CHECK_INTERVAL;
			if (!haveLastEvaluation) {
				alpha = -INFINITE_VALUE;	
				beta = INFINITE_VALUE;
			}
			else {
				alpha = lastEvaluation - ASPIRATION_WINDOW;
				beta = lastEvaluation + ASPIRATION_WINDOW;
			}
			int score = alphaBetaSearch(board, i, i, 0, true, alpha, beta, &searchInfo, &stats);

			if ((score == alpha && alpha != -INFINITE_VALUE) ||
				(score == beta && beta != INFINITE_VALUE)) {
				haveLastEvaluation = false;
				// aspiration window failed: do a re-search
				// TODO: http://chessprogramming.wikispaces.com/Aspiration+Windows
				// Possibly implement gradual widening
				i = i - 2;
				continue;
			}
			else {
				lastEvaluation = score;
				haveLastEvaluation = true;
			}

			MoveLinkedList extractedPV;
			extractPV(board, extractedPV);

			if (extractedPV.size() != 0) {
				theMove = extractedPV.getFirst();
			}
			else {
				transpositionTable.clear();
				i = i - 2;
				continue;
			}
#ifdef WIN32
			GetSystemTime(&endSystemTime);
			SystemTimeToFileTime(&endSystemTime, &end);
#else
			gettimeofday(&end, NULL);
#endif
			double diff = getSecondsDiff(&start, &end);
			if (!AnalysisMode)
				outputStats(board, stats, i, score, extractedPV, diff, noisy);
		}
		catch (TimeoutException e) {
			// shouldn't time out before we've gotten at least one move!
			if (noisy) {
				cerr << "\ttimed out" << endl;
			}
			break;
		}
	}
	
	transpositionTable.clear();

	ancientSwitch = !ancientSwitch;
	return theMove;
}

bool playerToMoveIsInCheck(ChessBoard * board) {
	return (board->whiteInCheck && board->whiteToMove) || (board->blackInCheck && !board->whiteToMove);
}

bool movePushesPawnToSeventhRank(ChessBoard * board, int move, bool whiteToMove) {
	if (whiteToMove && GetMovePiece(move) == PAWN) {
		short destSquare = GetTo(move);
		if (destSquare >= A7 && destSquare <= H7)
			return true;
	}
	else if (!whiteToMove && GetMovePiece(move) == PAWN) {
		short destSquare = GetTo(move);
		if (destSquare >= A2 && destSquare <= H2)
			return true;
	}
	
	return false;
}

bool movePushesPawnToSixthRank(ChessBoard * board, int move, bool whiteToMove) {
	if (whiteToMove && GetMovePiece(move) == PAWN) {
		short destSquare = GetTo(move);
		if (destSquare >= A6 && destSquare <= H6)
			return true;
	}
	else if (!whiteToMove && GetMovePiece(move) == PAWN) {
		short destSquare = GetTo(move);
		if (destSquare >= A3 && destSquare <= H3)
			return true;
	}
	
	return false;
}



int getSearchDepthWithExtensions(short  & depthLeft, ChessBoard *& board, int & nextMove)
{
    int searchDepth = depthLeft - 1;
    if(playerToMoveIsInCheck(board)){
        // check extension
        searchDepth += 1;
    }
    else if(movePushesPawnToSeventhRank(board, nextMove, !board->whiteToMove)){
    	searchDepth += 1;
    }
    else if(movePushesPawnToSixthRank(board, nextMove, !board->whiteToMove)){
    	searchDepth += 2;
    }

    return searchDepth;
}


bool shouldDoNullMove(ChessBoard *board, search_info *searchInfo)
{
    return !board->whiteInCheck && !board->blackInCheck && !searchInfo->lastMoveWasNullMove && 
     	board->gamePhase != PHASE_ENDGAME;
}

double getSecondsSinceSearchStarted(search_info *searchInfo)
{
#ifdef WIN32
	FILETIME currentTime;
#else
	timeval currentTime;
#endif
	getCurrentTime(&currentTime);
	return getSecondsDiff(&searchInfo->startTime, &currentTime);
}

int alphaBetaSearch(ChessBoard * board, 
					short startingDepth, short depthLeft,
					short ply, 
					bool isInitialCall, int alpha, int beta,   
					search_info* searchInfo,
					search_statistics* stats) {
	int hashFunction = HASH_ALPHA;
	board->currentSearchDepth = depthLeft;
	board->searchPhase[ply] = SEARCH_START;
	board->moveNum[ply] = -1;

	--timeToNextCheck;
	if (timeToNextCheck == 0) {
		checkTimeout(board, searchInfo, stats);
		timeToNextCheck = CHECK_INTERVAL;
	}
	
	if (transpositionTable.hasValue(board->zobristHashKey, depthLeft, alpha, beta)) {
		int hashedValue = transpositionTable.probeHash(board->zobristHashKey, depthLeft, alpha, beta);
		stats->hash_hits++;
		
		if (HASH_DEBUG) cerr << "got hash hit: " << endl << board_to_string(board) << " with hash key " << board->zobristHashKey << endl << " has value " << hashedValue << endl;
		
		return hashedValue;
	}

	if (depthLeft <= 0) {
		int eval = quiescentSearch(board, alpha, beta, ply, searchInfo, stats);

		int storeFunction;
		if (eval >= beta)
			storeFunction = HASH_BETA;
		else if (eval <= alpha) {
			storeFunction = HASH_ALPHA;
		}
		else { 
			storeFunction = HASH_EXACT;
		}
		
		if (HASH_DEBUG) cerr << "storing from qsearch (" << storeFunction << " -- " << alpha << " <= " << beta << ") " << eval << " for " << endl << board_to_string(board) << " with hash key " << board->zobristHashKey << endl;
		
		int qMove = transpositionTable.getMove(board->zobristHashKey);
		transpositionTable.recordHash(board->zobristHashKey, qMove, 1, eval, storeFunction, stats);

		return eval;
	}

	board->searchPhase[ply] = SEARCH_NULLMOVE;
	if (shouldDoNullMove(board, searchInfo)) {
		processNullMove(board);
		stats->nodes++;
		try {
			int R  = 2;
			searchInfo->lastMoveWasNullMove = true;
			int value = -alphaBetaSearch(board, startingDepth, depthLeft - R - 1, ply + 1, false,
					-beta, -alpha, searchInfo, stats);
			searchInfo->lastMoveWasNullMove = false;
			if (value >= beta) {
				// cutoff
				unprocessNullMove(board);
				return value;
			}

			unprocessNullMove(board);
		} 
		catch (TimeoutException e) {
			unprocessNullMove(board);
			throw e;
		}
	}

	short kingToCheck = board->whiteToMove ? KING_WHITE : KING_BLACK;
	bool hasLegalMove = false;
	int bestMove = 0;

	board->searchPhase[ply] = SEARCH_HASHMOVE;
	while (board->searchPhase[ply] != SEARCH_DONE) {
		int nextMove = getNextMove(board, board->whiteToMove, false, ply);
		try {
			processMove(board, nextMove);
			stats->nodes++;
			if (!isKingInCheck(board, kingToCheck)) {

				int searchDepth = getSearchDepthWithExtensions(depthLeft, board, nextMove);
				int startingNodes = stats->nodes;

				hasLegalMove = true;
				searchInfo->lastMoveWasNullMove = false;
				int value = -alphaBetaSearch(board, startingDepth, searchDepth, ply + 1, false, -beta, -alpha, searchInfo, stats);

				// TODO: get rid of this and just go with ply
				board->currentSearchDepth = depthLeft;

				if (value >= beta) {
#if SEARCH_DEBUG
					if (isInitialCall) {
						for(int i = depthLeft; i > 1; i--)
							cerr << "   ";
						cerr << "beta cutoff caused by " << MoveToString(nextMove) << " (score: "  << alpha << ")" << endl;
					}
#endif
					++stats->cutoffs;
					unprocessMove(board, nextMove);
					addKillerMove(board, nextMove, depthLeft);

					if (HASH_DEBUG) cerr << "storing (cutoff) " << beta << "(" << alpha << " <= " << beta << ")" << " for " << endl << board_to_string(board) << " with hash key " << board->zobristHashKey << endl;
					transpositionTable.recordHash(board->zobristHashKey, nextMove, depthLeft, (short) beta, HASH_BETA, stats);

					return beta;
				}

				if (value > alpha) {
#if SEARCH_DEBUG
					for(int i = depthLeft; i > 1; i--)
						cerr << "   ";
					cerr << "new best move " << MoveToString(nextMove) << " with a score of " << value << " (old best: " << alpha << ")" <<endl;
#endif
					if (AnalysisMode && isInitialCall) {
						MoveLinkedList line;
						line.add(nextMove);
						extractPV(board, line);
						double time = getSecondsSinceSearchStarted(searchInfo);

						outputStats(board, *stats, startingDepth, value, line, time, true);
					}

					alpha = value;
					bestMove = nextMove;
					hashFunction = HASH_EXACT;
				}


				if (isInitialCall) {

					double time = getSecondsSinceSearchStarted(searchInfo);
					int differentNodes = startingNodes - stats->nodes;
					cerr << "\t";
					cerr << " "
						 << MoveToString(nextMove)
						 << differentNodes << " "
						 << " K1: " << MoveToString(board->killerMoves[startingDepth])
						 << " K2: " << MoveToString(board->secondaryKillerMoves[startingDepth])
						 << endl;
				}

				#if SEARCH_DEBUG
				if (isInitialCall) {
					for(int i = depthLeft; i > 1; i--)
						cerr << "   ";
					cerr << "move " << MoveToString(nextMove) << " has score of " << value << " (current best: " << alpha << ")" << endl;

					// TODO: put line back in 
					for(int i = depthLeft; i > 1; i--)
						cerr << "   ";
				}
#endif
			}
		}
		catch (TimeoutException e) {
			unprocessMove(board, nextMove);
			throw e;
		}
		unprocessMove(board, nextMove);
	}
	
	if (!hasLegalMove) {
		short king = board->whiteToMove ? KING_WHITE : KING_BLACK;
		if (isKingInCheck(board, king)) {
			// if we're going to be in checkmate, delay it as long as possible
			// (favor checkmates later in the search tree)
			alpha = -CHECKMATE + ply;
		}
		else {  // stalemate
			// TODO: contempt factor
			alpha = 0;
		}
		hashFunction = HASH_EXACT;
	}
	
	if (HASH_DEBUG) cerr << "storing (alpha) " << alpha << " (" << alpha << " <= " << beta << ") for " << endl << board_to_string(board) << " with hash key " << board->zobristHashKey << endl;
	transpositionTable.recordHash(board->zobristHashKey, bestMove, depthLeft, alpha, hashFunction, stats);
	
	return alpha;
}

void addKillerMove(ChessBoard * board, int currentMove, int depth) {
	if (GetCapturePiece(currentMove) == 0) {
		// only if it's not a capture -- got this tip from rebel
		// basically, don't make a move a killer move if it's already
		// going to be searched highly
		if (board->killerMoves[depth] != currentMove && board->secondaryKillerMoves[depth] != currentMove) {
			board->secondaryKillerMoves[depth] = board->killerMoves[depth];
			board->killerMoves[depth] = currentMove;
		}
	}
}


int quiescentSearch(ChessBoard * board,  
					int alpha, int beta, int ply, 
					search_info * searchInfo,
					search_statistics * stats) {
	stats->nodes++;

	int hashFunction = 0;
	if (ply < MAX_DEPTH) {
		hashFunction = HASH_ALPHA;
		board->searchPhase[ply] = SEARCH_START;
		board->moveNum[ply] = -1;
	}

	if (transpositionTable.hasValue(board->zobristHashKey, 0, alpha, beta)) {
		int hashedValue = transpositionTable.probeHash(board->zobristHashKey, 0, alpha, beta);
		stats->hash_hits++;
		
		if (HASH_DEBUG) cerr << "got hash hit in qsearch: " << endl << board_to_string(board) << " with hash key " << board->zobristHashKey << endl << " has value " << hashedValue << endl;
		
		return hashedValue;
	}

	board->currentSearchDepth = -1;
	int value = evaluateBoard<false>(board);
	if (IsCheckmate(value)) {
		if (value > 0) 
			// penalize later checkmates
			value -= ply;
		else
			// reward later checkmates
			value += ply;
		transpositionTable.recordHash(board->zobristHashKey, 0, 0, value, HASH_EXACT, stats);
		return value;
	}

	if (ply > stats->qdepth) {
		stats->qdepth = ply;
	}

	if (value >= beta) {
		// cutoff
		transpositionTable.recordHash(board->zobristHashKey, 0, 0, beta, HASH_BETA, stats);
		return beta;
	}
	if (value > alpha) {
		alpha = value;
		hashFunction = HASH_EXACT;
	}

	int bestCapture = 0;

	if (ply < MAX_DEPTH) {
		board->searchPhase[ply] = SEARCH_HASHMOVE;
		
		bool hasLegalMove = false;
		
		while(board->searchPhase[ply] != SEARCH_DONE) {
			int nextCapture = getNextMove(board, board->whiteToMove, true, ply);

			if (nextCapture != -1) {
				short currentKing = board->whiteToMove ? KING_WHITE : KING_BLACK;

				try {
					//cerr << "next capture: " << nextCapture << endl;
					processMove(board, nextCapture);
					stats->nodes++;

					if (!isKingInCheck(board, currentKing)) {
						hasLegalMove = true;

						int qscore = -quiescentSearch(board, -beta, -alpha, ply + 1, searchInfo, stats);

						if (qscore >= beta) {
							unprocessMove(board, nextCapture);
							stats->cutoffs++;
							transpositionTable.recordHash(board->zobristHashKey, nextCapture, 1, beta, HASH_BETA, stats);

							return beta;
						}

						if (qscore > alpha) {
							alpha = qscore;
							bestCapture = nextCapture;

							hashFunction = HASH_EXACT;
						}
					}
				}
				catch (TimeoutException e) {
					unprocessMove(board, nextCapture);
					throw e;
				}
				unprocessMove(board, nextCapture);
			}
		}
	}

	transpositionTable.recordHash(board->zobristHashKey, bestCapture, 1, value, hashFunction, stats);
	
	return alpha;
}

void updateVariationWithLine(MoveLinkedList * variation, MoveLinkedList * line, int currentMove) {
	variation->clear();
	if (currentMove != 0)
		variation->add(currentMove);
	variation->addAll(*line);
}

#ifdef WIN32
double getSecondsDiff(FILETIME * start, FILETIME * end) {
		// number of 100 nanosecond differences.  to see the number of 
		// seconds, we know that 100 nanoseconds = 100 * 10^-9 = 10^-7 seconds
		// so multiply this by 10^-7 and we have the number of seconds
		__int64 startTime = (((__int64) start->dwHighDateTime) << 32) + start->dwLowDateTime;
		__int64 endTime = (((__int64) end->dwHighDateTime) << 32) + end->dwLowDateTime;

		double diff = (endTime - startTime) / (10000000.0);
		return diff;
}
#else
double getSecondsDiff(timeval * start, timeval * end) {
	double diff = (end->tv_sec - start->tv_sec);
	diff += (end->tv_usec - start->tv_usec) / (1000.0 * 1000.0);
	
	return diff;
}
#endif

void checkTimeout(ChessBoard* board, search_info * searchInfo, search_statistics * stats) {
#ifdef WIN32
	SYSTEMTIME currentSystemTime;
	FILETIME currentTime;
	GetSystemTime(&currentSystemTime);
	SystemTimeToFileTime(&currentSystemTime, &currentTime);
#else
	timeval currentTime;
	gettimeofday(&currentTime, NULL);
#endif
	if (AnalysisMode) {
		bool hasInput = checkForInputDuringSearch();
		if (hasInput) {
			std::string userInput;
			std::getline(std::cin, userInput);
			LOG4CXX_INFO(logger, "received message during search " << userInput);

			if (userInput == "exit")
				throw TimeoutException();
			else if (userInput == ".") {
				double time = getSecondsDiff(&searchInfo->startTime, &currentTime);
				cout << "stat01: " << ((int) (time * 100)) 
					 << " " << stats->nodes  << endl;
			}
			else if (userInput == "") {
				// should not happen
			}
			else {
				LOG4CXX_INFO(logger, "did not recognize " << userInput);
				cerr << "got " << userInput << " during analysis and did not recognize it -- quitting" << endl;
				analysisMessages.push_back(userInput);
				throw TimeoutException();
			}
		}
	}

	double secondsDiff = getSecondsDiff(&searchInfo->startTime, &currentTime);
	if (secondsDiff > TimeoutValue) 
		throw TimeoutException();
}

void outputStatsHeader(bool noisy) {
	if (noisy) {
			cerr << "depth" << "\t" << 
			"score" << "\t" << 
			"time" << "\t" << 
			"NPS" << "\t" << 
			"nodes" << "\t" << 
			"cutoffs" << "\t"<< 
			"hits" << "\t" <<
			"replace" << "\t" <<
			"line" << endl;
		}
}

void outputStats(ChessBoard * board, const search_statistics& stats, int depth, int score, MoveLinkedList& line, double diff, bool noisy) {
	if (noisy) {
		cerr << depth << " " 
			 << score << " "
			 << (int) (diff * 100) << " "
			 << stats.nodes << " "
			 << line.toMoveString(board) << endl;
	}
}

void extractPV(ChessBoard * board, MoveLinkedList& line) {
	std::set<BITBOARD> zobristKeys;
	extractPVHelper(board, line, zobristKeys);
}

void extractPVHelper(ChessBoard * board, MoveLinkedList& line, std::set<BITBOARD>& zobristKeys) {
	int currentMove = transpositionTable.getMove(board->zobristHashKey);
	if (currentMove == 0) {
		return;
	}
	
	if (MoveIsLegal(board,currentMove)) {
		processMove(board, currentMove);
		line.add(currentMove);
		if (zobristKeys.find(board->zobristHashKey) == zobristKeys.end()) {
			zobristKeys.insert(board->zobristHashKey);
			extractPVHelper(board, line,zobristKeys);
		}
		else {
			line.add(-1);
		}
		unprocessMove(board, currentMove);
	}
//	else {
//		cerr << "move " << MoveToString(currentMove) << " from hash table is not legal on board" << endl;
//	}
}

int* getAndSortMovesForBoard(ChessBoard * board, bool whiteToMove, bool quiescentMoves, int ply) {
	int* startMoves = board->moveList[ply];
	int* endMoves;
	if (!quiescentMoves) {
		endMoves = generateCaptures(board, whiteToMove, startMoves);
		endMoves = generateNonCaptures(board, whiteToMove, endMoves);
	}
	else {
		endMoves = getQuiescentMoves(board, whiteToMove, startMoves);
	}
	

	int firstPriority = getPriorityForMove(board, *startMoves);
	int firstMove = *startMoves;
	board->moveList[ply][0] = firstMove;
	board->moveSortList[ply][0] = firstPriority;

	int k = 1;
	//cerr << startMoves << endl;
	for(int* ptr = startMoves + 1; ptr < endMoves; ++ptr) {
		int currentPriority = getPriorityForMove(board, *ptr);
		int theMove = *ptr;
//		mpq.add(*ptr, currentPriority);
		board->moveSortList[ply][k] = currentPriority;
		
		// insertion sort this sucker -- assume that 0, ... k - 1 are already sorted, most to least.  
		// add this current move at the end and bubble it backwards until it reaches 
		// the desired place.  this desired place is either the first j s.t. 
		// value[j] > currentPriority, or j = 0. 
		int j; //= k - 1;
		for (j = k - 1; j >= 0 && (board->moveSortList[ply][j] < currentPriority); --j) {
			// put A[j + 1] into A[j]
			board->moveList[ply][j + 1] = board->moveList[ply][j];
			board->moveSortList[ply][j + 1] = board->moveSortList[ply][j];
		}
		
		board->moveList[ply][j + 1] = theMove;
		board->moveSortList[ply][j + 1] = currentPriority;

		++k;
	}

	return endMoves;
}

int getPriorityForMove(ChessBoard * chessBoard, int move) {
	int priority = 0;

	if (chessBoard->currentSearchDepth != -1 &&
		transpositionTable.getMove(chessBoard->zobristHashKey) == move) {
		priority = PRIORITY_HASH_MOVE;
	}
	else if (move == chessBoard->killerMoves[chessBoard->currentSearchDepth] ||
 	  		 move == chessBoard->secondaryKillerMoves[chessBoard->currentSearchDepth]) {
		priority = PRIORITY_KILLER_MOVE;
	}
	else {
		priority = getPriorityForMove_MVVLVA(chessBoard, move);
	}
	
	return priority;
}

// check for user input during search -- only do this in analyze mode
// got the windows version of this from Beowulf -- I am not very familiar
// with Win32 programming any more, sadly
bool checkForInputDuringSearch() {
#ifdef WIN32

#if defined(FILE_CNT)
	if (stdin->_cnt > 0)
		return stdin->_cnt;
#endif
	static int init = 0, pipe;
	static HANDLE stdinHandle;
	DWORD word;

	if (!init) {
		init = 1;
		stdinHandle = GetStdHandle(STD_INPUT_HANDLE);
		pipe = !GetConsoleMode(stdinHandle, &word);

		if (!pipe) {
			BOOL result = SetConsoleMode(stdinHandle, word & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
			FlushConsoleInputBuffer(stdinHandle);
		}
	}
	if (pipe) {
		int peekValue = PeekNamedPipe(stdinHandle, NULL, 0, NULL, &word, NULL);

		if (!peekValue || !word)
			return false;
		else
			return true;
	}
	else {
		GetNumberOfConsoleInputEvents(stdinHandle, &word);
		if (word <= 1) {
			return false;
		}
		else {
			return true;
		}
	}
#else
	struct timeval waitTime;
	waitTime.tv_sec = 0;
	waitTime.tv_usec = 0;
	fd_set stdinSet;
	
	FD_ZERO(&stdinSet);
	FD_SET(fileno(stdin), &stdinSet);
	select(fileno(stdin) + 1, &stdinSet, NULL, NULL, &waitTime);
	
	if (!FD_ISSET(fileno(stdin), &stdinSet))
		return false;
	else
		return true;
#endif
}

#ifdef WIN32
void getCurrentTime(FILETIME * time) {
	SYSTEMTIME currentSystemTime;
	GetSystemTime(&currentSystemTime);
	SystemTimeToFileTime(&currentSystemTime, time);
}
#else
void getCurrentTime(timeval * tv1) {
	gettimeofday(tv1, NULL);
}
#endif


int getNextMove(ChessBoard * board, bool whiteToMove, bool quiescent, int ply) {
	int hashMove = 0;
	int nextMove = 0;
	switch (board->searchPhase[ply]) {
	case SEARCH_HASHMOVE:
		transpositionTable.getMove(board->zobristHashKey);
		board->searchPhase[ply] = SEARCH_ALLMOVES;
		// TODO: if no hash move, do internal deepening search
		if (hashMove != 0 && MoveIsLegal(board, hashMove)) {
			return hashMove;
		}
		// otherwise fallthrough
	case SEARCH_ALLMOVES:
		if (board->moveNum[ply] == -1) {
			
			board->lastMove[ply] = getAndSortMovesForBoard(board, whiteToMove, quiescent, ply);

			if (board->lastMove[ply] == board->moveList[ply]) {
				board->searchPhase[ply] = SEARCH_DONE;
				return -1;
			}
			board->moveNum[ply] = 0;
		}
		
		nextMove = *(board->moveList[ply]  + board->moveNum[ply]);

		board->moveNum[ply] = board->moveNum[ply]  + 1;
		if (board->moveList[ply] + board->moveNum[ply] == board->lastMove[ply]) {
			board->searchPhase[ply] = SEARCH_DONE;
			board->moveNum[ply] = -1;
			board->lastMove[ply] = board->moveList[ply];
		}
		
		return nextMove;
	default:
		cerr << "don't know what to do!!  phase is " << board->searchPhase[ply] << endl;
		return -1;
	}
}
