#ifndef XBOARD_H_
#define XBOARD_H_

#include "common.h"
#include <list>

#include "module.h"

extern std::list<std::string> analysisMessages;
int CoordStringToMove(ChessBoard * board, const std::string& str);

class XboardModule : public Module {

 public:
  
  XboardModule();
  void run();

 private:

  void doXboardMove(ChessBoard * chessBoard, bool whiteToMove);
  void doMove(ChessBoard* board, int reply);
  bool validateMove(ChessBoard* board, int reply);

  void sendBoardInformation(ChessBoard * chessBoard);
  void searchForMove(ChessBoard * board, bool whiteToMove);
  void analyzeBoard(ChessBoard * board, bool whiteToMove);
  std::string MoveToXboardString(int move);
  bool isMoveString(const std::string& str);
  void setTimeoutValue(ChessBoard * board);


  // PROTOCOL ACTIONS
  void protocolNew();
  void protocolAnalyze();
  void protocolProtover2();
  void protocolRandom();
  void protocolForce();
  void protocolGo();
  void protocolMove(std::string& line);
  void protocolTime(std::string& line);
  void protocolOtime(std::string& line);
  void protocolEval();
  void protocolSetBoard(std::string& line);
  void protocolUndo();
  void protocolSt(std::string& line);
  void protocolRemove();

  // PRIVATE MEMBERS
  bool forceMode_;
  bool computerMode_;
  bool AnalysisMode_;

  float TimeoutValue_;
  int timeLeft_;
  int opponentTimeLeft_;
};

#endif /*XBOARD_H_*/
