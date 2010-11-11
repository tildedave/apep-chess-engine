#ifndef BITTEST_H_
#define BITTEST_H_

#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include "../common.h"

class BitTest : public CppUnit::TestCase {

public:
	
	  static CppUnit::Test *suite()
	  {
	    CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite( "BitTest" );
	    suiteOfTests->addTest( new CppUnit::TestCaller<BitTest>( 
	                                   "testFirstOne", 
	                                   &BitTest::testFirstOne ) );
	    return suiteOfTests;
	  }
	
	void testFirstOne() {
		BITBOARD i = 0x1000000000000001ULL;
		for(int j = 0; j < 64; ++j) {
			if (j != 0)
				i = i << 1;
			short firstOneOld = FirstOne(i);
			short firstOneNew = FirstOneNew(i);
			std::string message = "different FirstOne for ";
			message += BITBOARD_to_string(i);
			message += "\n" + bitboard_to_string(i);
			CPPUNIT_ASSERT_EQUAL_MESSAGE(message, firstOneOld, firstOneNew);
		}
		
		cerr << "finished testing first ones" << endl;
	}
	
};

#endif /*BITTEST_H_*/
