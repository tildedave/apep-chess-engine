/*
 * CECPListener.cpp
 *
 *  Created on: Nov 26, 2010
 *      Author: dhking
 */

#include "cecp/CECPListener.h"

CECPListener::CECPListener(std::istream& input, std::ostream& output) :
  input_(input),
  output_(output)
{
}

CECPListener::~CECPListener() {
	// TODO Auto-generated destructor stub
}
