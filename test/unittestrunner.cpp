#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestResult.h>
#include <cppunit/ui/text/TestRunner.h>
#include "commontest.h"
#include "boardtest.h"
#include "evaltest.h"
#include "movetest.h"

int randomSeed = -1;

int main(int argc, char** argv) {
	initialize_common_boards();

	CppUnit::TextUi::TestRunner runner;
	runner.addTest( BoardTest::suite() );
	runner.addTest( CommonTest::suite() );
	runner.addTest( EvalTest::suite() );
	runner.addTest( MoveTest::suite() );

	runner.run();	
	return 0;
}

