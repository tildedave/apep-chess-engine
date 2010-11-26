#pragma once

#include <iostream>
#include <sstream>

#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>

#include "cecp/CECPListener.h"

class CECPListenerTest : public CppUnit::TestCase {

public:

	static CppUnit::Test *suite()
	{
		CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite( "CECPListenerTest" );
		suiteOfTests->addTest( new CppUnit::TestCaller<CECPListenerTest>(
				"testReceivesXBoardMessage",
				&CECPListenerTest::testReceivesXBoardMessage ) );

		return suiteOfTests;
		return 0;
	}

	void testReceivesXBoardMessage() {
		std::stringstream input;
		input << "xboard" << std::endl;
		std::stringstream output;

		CECPListener receiver(input, output);


	}
};
