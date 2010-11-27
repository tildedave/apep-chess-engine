/*
 * CECPListener.cpp
 *
 *  Created on: Nov 26, 2010
 *      Author: dhking
 */

#include "cecp/CECPListener.h"

#include <istream>
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/fileappender.h>
#include <log4cxx/simplelayout.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/helpers/exception.h>

// TODO: separate receive/process threads
// TODO: separate search analysis thread, communicate w/named pipe

CECPListener::CECPListener(std::istream& input, std::ostream& output) :
  input_(input),
  output_(output),
  messages_()
{
	this->setupLogging();
}

CECPListener::~CECPListener() {
}

void
CECPListener::setupLogging()
{
    log4cxx::FileAppender *fileAppender = new log4cxx::FileAppender(log4cxx::LayoutPtr(new log4cxx::PatternLayout("%5p [%t] (%F:%L) - %m%n")), "apep.log", false);
    log4cxx::helpers::Pool p;
    fileAppender->activateOptions(p);
    log4cxx::BasicConfigurator::configure(log4cxx::AppenderPtr(fileAppender));
    log4cxx::Logger::getRootLogger()->setLevel(log4cxx::Level::getDebug());
    this->logger = log4cxx::Logger::getLogger("logger");
}

std::vector<std::string>&
CECPListener::messages() {
	return messages_;
}

void
CECPListener::receive() {
	std::string line;
	std::getline(this->input_, line);
	LOG4CXX_INFO(this->logger, "received message " << line);
	this->messages_.push_back(line);
}

void
CECPListener::process() {
}

