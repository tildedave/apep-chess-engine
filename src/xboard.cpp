#include <istream>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#include "move.h"
#include "moveprocess.h"
#include "movegen.h"
#include "xboard.h"
#include "eval.h"
#include "search.h"

extern log4cplus::Logger logger;
ChessBoard xboardBoard;
std::list<std::string> analysisMessages;

XboardModule::XboardModule() :
  forceMode_(true),
  computerMode_(false),
  AnalysisMode_(false),
  TimeoutValue_(TIMEOUT_VALUE),
  timeLeft_(0),
  opponentTimeLeft_(0)
{
}

std::string
XboardModule::MoveToXboardString(int move) {
	short fromOffset = GetFrom(move);
	short toOffset = GetTo(move);
	short promotionPiece = GetPromotionPiece(move);
	std::string promotionString = "";
	if (promotionPiece != 0) {
		if (IsQueen(promotionPiece))
			promotionString += "q";
		else if (IsKnight(promotionPiece))
			promotionString += "n";
		else if (IsRook(promotionPiece))
			promotionString += "r";
		else if (IsBishop(promotionPiece))
			promotionString += "b";
	}

	std::string str = offset_to_string(fromOffset) + offset_to_string(toOffset) + promotionString;
	return str;
}

int
CoordStringToMove(ChessBoard * board, const std::string& str) {
	std::string firstOffset = str.substr(0, 2);
	std::string secondOffset = str.substr(2, 2);
	bool whiteToMove = board->whiteToMove;

	char promotionChar = 0;
	if (str.length() > 4) {
		promotionChar = str[4];
	}

	int from = string_to_offset(firstOffset);
	int to = string_to_offset(secondOffset);

	int movePiece = (whiteToMove ? 1 : -1) * board->boardContents[from];
	int capturePiece = (whiteToMove ? -1 : 1) * board->boardContents[to];

	if (movePiece == PAWN && to == board->enPassantOffset) {
		// can't move to the en passant offset normally, so this is safe
		capturePiece = PAWN;
	}

	int promotionPiece = 0;
	if (promotionChar != 0) {
		switch (promotionChar) {
		case 'q':
			promotionPiece = QUEEN;
			break;
		case 'n':
			promotionPiece = KNIGHT;
			break;
		case 'b':
			promotionPiece = BISHOP;
			break;
		case 'r':
			promotionPiece = ROOK;
			break;
		}
	}

	// we really don't need separate logic here
	if (promotionPiece != 0) {
		return MakePromotion(movePiece,from, to, capturePiece, promotionPiece);
	}
	else {
		return MakeCapture(movePiece, from, to, capturePiece);
	}
}

void
XboardModule::protocolNew()
{
    // CECP: Reset the board to the standard chess starting position. Set White
    // on move. Leave force mode and set the engine to play Black. Associate
    // the engine's clock with Black and the opponent's clock with White.
    // Reset clocks and time controls to the start of a new game. Stop clocks.
    // Do not ponder on this move, even if pondering is on. Remove any search
    // depth limit previously set by the sd command.
    memset(&xboardBoard, 0, sizeof (ChessBoard));
    loadBoardFromFEN(&xboardBoard, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    cerr << board_to_string(&xboardBoard) << endl;
    forceMode_ = false;
    TimeoutValue_ = TIMEOUT_VALUE;
}

void
XboardModule::protocolProtover2()
{
    std::cout << "feature ";
    std::cout << "myname=\"apep 0.1.0\" ";
    std::cout << "setboard=1 ";
    std::cout << "sigint=0 ";
    std::cout << "sigterm=0 ";
    std::cout << "done=1 ";
    std::cout << endl;
}

void
XboardModule::protocolRandom()
{
    // TODO: random additions to Chess Player's evaluation
    // CECP: This command is specific to GNU Chess 4. You can either ignore it completely
    // (that is, treat it as a no-op) or implement it as GNU Chess does. The command
    // toggles "random" mode (that is, it sets random = !random). In random mode, the
    // engine adds a small random value to its evaluation function to vary its play.
    // The "new" command sets random mode off.
}

void
XboardModule::protocolForce()
{
    // CECP: Set the engine to play neither color ("force mode"). Stop clocks.
    // The engine should check that moves received in force mode are legal
    // and made in the proper turn, but should not think, ponder, or make
    // moves of its own.
    forceMode_ = true;
}

void
XboardModule::protocolGo()
{
    // CECP: Leave force mode and set the engine to play the color that is on move.
    // Associate the engine's clock with the color that is on move, the opponent's
    // clock with the color that is not on move. Start the engine's clock. Start
    // thinking and eventually make a move.
    // TODO: launch thread to find move
    forceMode_ = false;
    searchForMove(&xboardBoard, xboardBoard.whiteToMove);
}


bool
XboardModule::validateMove(ChessBoard* board, int reply) {
  int moveList[MAX_BRANCH];

  int* endCaptures = generateCaptures(board, board->whiteToMove, moveList);
  int* endMoveList = generateNonCaptures(board, board->whiteToMove, endCaptures);

  for(int* i = moveList; i != endMoveList; ++i) {
    if (*i == reply) {
      return true;
    }
  }

  return false;
}

void
XboardModule::protocolMove(std::string & line)
{
    int move = CoordStringToMove(&xboardBoard, line);
    LOG4CPLUS_INFO(logger, "got move string " << line << " (" << MoveToString(move) << ")");

    if (!validateMove(&xboardBoard, move)) {
      LOG4CPLUS_INFO(logger, "got illegal move " << MoveToString(move));
      std::cout << "Illegal move: " << line << std::endl;
      return;
    }

    processMove(&xboardBoard, move);
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("board now ")
    	<< LOG4CPLUS_TEXT(boardToFEN(&xboardBoard)));
    internalConsistencyCheck(&xboardBoard);
    cerr << board_to_string(&xboardBoard) << endl;
    // TODO: verify that you're not moving into check
    sendBoardInformation(&xboardBoard);
    if(getGameResult(&xboardBoard) == 0)
        // get another move
        searchForMove(&xboardBoard, xboardBoard.whiteToMove);

}

void
XboardModule::protocolSetBoard(std::string & line)
{
    // CECP: The setboard command is the new way to set up
    // positions, beginning in protocol version 2. It is not
    // used unless it has been selected with the feature
    // command. Here FEN is a position in Forsythe-Edwards
    // Notation, as defined in the PGN standard.
    int startFEN = line.find(" ", 0) + 1;
    std::string fenString = line.substr(startFEN, std::string::npos);
    memset(&xboardBoard, 0, sizeof (ChessBoard));
    loadBoardFromFEN(&xboardBoard, fenString);
    cout << board_to_string(&xboardBoard) << endl;
    forceMode_ = false;
}

void
XboardModule::protocolUndo()
{
    if(xboardBoard.moveIndex != 0){
        cerr << "got undo -- " << xboardBoard.moveIndex << endl;
        int lastMove = xboardBoard.moveHistory[xboardBoard.moveIndex - 1];
        unprocessMove(&xboardBoard, lastMove);
        cerr << "now: " << endl;
        cerr << "undid " << MoveToString(lastMove) << endl;
        cerr << board_to_string(&xboardBoard) << endl;
    }
}

void
XboardModule::protocolRemove()
{
	// If the user asks to retract a move, xboard will send you the "remove" command. It sends
	// this command only when the user is on move. Your engine should undo the last two moves
	// (one for each player) and continue playing the same color.

	protocolUndo();
	protocolUndo();
}

void
XboardModule::protocolAnalyze()
{
    AnalysisMode_ = true;
    analyzeBoard(&xboardBoard, xboardBoard.whiteToMove);
    cout << board_to_string(&xboardBoard) << endl;
}

void
XboardModule::protocolTime(std::string & line)
{
    std::string::size_type st = line.find(" ", 0);
    std::string timeString = line.substr(st + 1, std::string::npos);
    this->timeLeft_ = string_to_int(timeString);
}

void
XboardModule::protocolOtime(std::string & line)
{
    std::string::size_type st = line.find(" ", 0);
    std::string timeString = line.substr(st + 1, std::string::npos);
    this->opponentTimeLeft_ = string_to_int(timeString);
}

void
XboardModule::protocolEval()
{
    cout << board_to_string(&xboardBoard) << endl;
    //int material = getMaterialScore(&xboardBoard);
    //cout << "material score: " << material << endl;
    //int position = getPositionScore<true>(&xboardBoard, &std::cout);
    int total = evaluateBoard<true>(&xboardBoard, &std::cout);
    cout << "total: " << total << endl;
}

void
XboardModule::protocolSt(std::string & line)
{
    std::string searchTime = line.substr(3);
    TimeoutValue_ = string_to_int(searchTime);
    std::cout << "got st " << TimeoutValue_ << endl;
}

void
XboardModule::run() {
	bool shouldContinue = true;

	while (shouldContinue) {
		std::string line;
		bool gotMessage = false;

		if (!AnalysisMode_) {
		  if (std::cin.eof()) {
		    break;
		  }
		  else {
		    std::getline(std::cin, line);
		    LOG4CPLUS_INFO(logger, "received message " << line);
		    gotMessage = true;
		  }
		}
		else {
			if (analysisMessages.empty()) {
				protocolAnalyze();
			}
			else {
				line = analysisMessages.front();
				LOG4CPLUS_INFO(logger, "received message during analysis " << line);

				analysisMessages.pop_front();
				gotMessage = true;
			}
		}

		if (line.find("xboard",0) == 0) {
			// CECP: This command will be sent once immediately after your engine
			// process is started. You can use it to put your engine into
			// "xboard mode" if that is needed. If your engine prints a prompt
			// to ask for user input, you must turn off the prompt and output
			// a newline when the "xboard" command comes in.
			std::cout << endl;
		}
		else if (line.find("protover 2", 0) == 0) {
			protocolProtover2();
		}
		else if (line == "new") {
			protocolNew();
		}
		else if (line.find("quit", 0) == 0) {
			shouldContinue = false;
		}
		else if (line == ("random")) {
			protocolRandom();
		}
		else if (line == ("force")) {
			protocolForce();
		}
		else if (line == ("go")) {
			protocolGo();
		}
		else if (line == "playother") {
			// CECP: This command is new in protocol version 2. It is not sent unless you enable it
			// with the feature command.) Leave force mode and set the engine to play the color
			// that is not on move. Associate the opponent's clock with the color that is on
			// move, the engine's clock with the color that is not on move. Start the opponent's
			// clock. If pondering is enabled, the engine should begin pondering. If the engine
			// later receives a move, it should start thinking and eventually reply.
		}
		else if (line == "analyze") {
			protocolAnalyze();
		}
		else if (line == "exit") {
			AnalysisMode_ = false;
		}
		else if (isMoveString(line)) {
			protocolMove(line);
		}
		else if (line.find("usermove", 0) == 0) {
			// TODO
		}
		else if (line.find("time", 0) == 0) {
			protocolTime(line);
		}
		else if (line.find("otim", 0) == 0) {
			protocolOtime(line);
		}
		else if (line == "?") {
			// CECP: Move now. If your engine is thinking, it should
			// move immediately; otherwise, the command should be
			// ignored (treated as a no-op). It is permissible for your
			// engine to always ignore the ? command. The only bad
			// consequence is that xboard's Move Now menu command will
			// do nothing.

			// It is also permissible for your engine to move
			// immediately if it gets any command while thinking, as
			// BITBOARD as it processes the command right after moving, but
			// it's preferable if you don't do this. For example, xboard
			// may send post, nopost, easy, hard, force, quit, or other
			// commands while the engine is on move.

			// TODO
		}
		else if (line == "eval") {
			protocolEval();
		}
		else if (line == "draw") {
			// TODO
		}
		else if (line.find("result", 0) == 0) {
			// game over man, game over
		}
		else if (line.find("setboard", 0) == 0) {
			protocolSetBoard(line);
		}
		else if (line.find("edit", 0) == 0) {
			// TODO: fix this
		}
		else if (line.find("hint", 0) == 0) {

		}
		else if (line.find("undo", 0) == 0) {
			protocolUndo();
		}
		else if (line.find("white", 0) == 0) {
		}
		else if (line.find("black", 0) == 0) {
		}
		else if (line.find("st", 0) == 0) {
			protocolSt(line);
		}
		else if (line.find("remove") == 0) {
			protocolRemove();
		}
		else {
			cout << "didn't understand " << line << endl;
		}
	}
}

void
XboardModule::analyzeBoard(ChessBoard * board, bool whiteToMove) {
	search_options options;
	options.analysisMode = true;
	options.noisyMode = true;
        options.outputNodesPerMove = true;
	options.TimeoutValue = INFINITE_VALUE;

	int reply = getMove(board, &options);
	// only get here after we are forced to time out
}

void
XboardModule::doMove(ChessBoard* board, int reply) {
  LOG4CPLUS_INFO(logger, "made move " << MoveToString(reply));
  processMove(board, reply);
  sendBoardInformation(board);
  internalConsistencyCheck(board);
  LOG4CPLUS_INFO(logger, "board now " << boardToFEN(board));
  cerr << board_to_string(board) << endl;
  cerr << "ready to wait for another move now " << endl;
}

void
XboardModule::searchForMove(ChessBoard * board, bool white) {
	setTimeoutValue(board);
	if (!forceMode_) {
	        search_options options;
                options.noisyMode = true;
		options.TimeoutValue = TimeoutValue_;

		int reply = getMove(board, &options);

		cout << "move " << MoveToXboardString(reply) << endl;

		doMove(board, reply);
	}
	else {
		cerr << "not in force move, not searching for move" << endl;
	}
}

bool
XboardModule::isMoveString(const std::string& str) {

	if (str.length() >= 4) {
		char fromFile = str[0];
		char fromRank = str[1];
		char toFile = str[2];
		char toRank = str[3];

		return file_to_column(fromFile) >= 0 && char_to_int(fromRank) > 0 &&
		file_to_column(toFile) >= 0 && char_to_int(toRank) > 0;
	}
	return false;
}

void
XboardModule::sendBoardInformation(ChessBoard * board) {
	int gameResult = getGameResult(board);
	bool outputBoardInfo = false;
	std::string resultString;

	if (gameResult == 1) {
	  outputBoardInfo = true;
	  resultString = "result 1-0 {White mates}";
	}
	else if (gameResult == -1) {
	  outputBoardInfo = true;
	  resultString = "result 0-1 {Black mates}";
	}
	else if (gameResult == 2) {
	  // stalemate
	  outputBoardInfo = true;
	  resultString = "result 1/2-1/2 {Draw}";
	}
	else if (gameResult == 3) {
	  outputBoardInfo = true;
	  resultString = "result 1/2-1/2 {Lack of material to force checkmate}";
	}

	if (outputBoardInfo) {
	  LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("result ")
	  		<< LOG4CPLUS_TEXT(resultString));
	  cout << resultString << std::endl;
	}
}

void
XboardModule::setTimeoutValue(ChessBoard * board) {
	// current time left is in global 'timeLeft' variable
	int movesUntilNextTimeControl = 41 - (board->fullmoveClock % 41);
	// so we have movesUntilNextTimeControl to distribute these
	// seconds.

	float amountOfTime = ((float) timeLeft_) / (movesUntilNextTimeControl * 100.0);
	cerr << "searching for " << amountOfTime << " seconds..." << endl;

	TimeoutValue_ = amountOfTime;
}
