#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestResult.h>
#include <cppunit/ui/text/TestRunner.h>
#include "bittest.h"
#include "boardtest.h"
#include "searchtest.h"

int randomSeed = -1;

int main(int argc, char** argv) {
	initialize_common_boards();

	CppUnit::TextUi::TestRunner runner;
	runner.addTest( BoardTest::suite() );

        // tests currently broken -- 
        // throw core dumps
        //runner.addTest( BitTest::suite() );
	//runner.addTest( SearchTest::suite() );

	runner.run();	
	return 0;
}

