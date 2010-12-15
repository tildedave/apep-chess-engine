#pragma once 

#include "module.h"
#include <list>
#include <string>

class TacticsModule : public Module {

 public:
  TacticsModule(const std::string& fenString, const std::string& expected, int timeout);

  void run();

  bool wasSuccessful();
  std::string getMoveString();

 private:

  std::string fenString_;
  std::string expected_;
  bool wasSuccessful_;
  std::string moveString_;
  int timeout_;
};

class TacticsFileModule : public Module {

 public:

  TacticsFileModule(const std::string& filename, int timeout);

  void run();

  int getSucceededTestCount();
  int getFailedTestCount();
  int getTestCount();

 private:

  void outputStats();
  
  std::string filename_;
  int succeededTestsCount_;
  int totalTestsCount_;
  int failedTestsCount_;
  int timeout_;

  std::list<std::string> failedNamesTests_;
  std::map<std::string, std::string> testToMoveStringMapping_;
  
};
