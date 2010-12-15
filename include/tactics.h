#pragma once 

#include "module.h"
#include <vector>
#include <string>

class TacticsModule : public Module {

 public:
  TacticsModule(const std::string& fenString, const std::string& expected);

  void run();

  bool wasSuccessful();
  std::string getMoveString();

 private:

  bool wasSuccessful_;
  std::string moveString_;

};

class TacticsFileModule : public Module {

 public:

  TacticsFileModule(const std::string& filename);

  void run();

  int getSucceededTestCount();
  int getFailedTestCount();
  int getTestCount();

 private:

  void outputStats();
  
  int succeededTestsCount_;
  int totalTestsCount_;
  int failedTestsCount_;
  std::vector<std::string> failedNamesTests_;
  std::map<std::string, std::string> testToMoveStringMapping_;
  
};
