#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <log4cxx/logger.h>

class CECPListener {
 public:
        CECPListener(std::istream& input, std::ostream& output);
        virtual ~CECPListener();

        std::vector<std::string>& messages();
        void receive();
        void process();

 protected:
        std::istream& input_;
        std::ostream& output_;
        std::vector<std::string> messages_;
        log4cxx::LoggerPtr logger;

 private:
        void setupLogging();

};
