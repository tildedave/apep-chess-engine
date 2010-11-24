#ifndef BITTEST_H_
#define BITTEST_H_

#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include "common.h"

class BitTest : public CppUnit::TestCase {

public:
	
	  static CppUnit::Test *suite()
	  {
	    CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite( "BitTest" );
	    suiteOfTests->addTest( new CppUnit::TestCaller<BitTest>( 
								"testFirstOne",
								&BitTest::testFirstOne ));
	    suiteOfTests->addTest( new CppUnit::TestCaller<BitTest>(
								"testFirstOneBlankBoard",
								&BitTest::testFirstOneFirstBit ) );
	    suiteOfTests->addTest( new CppUnit::TestCaller<BitTest>(
								"testFirstOneBlankBoard",
								&BitTest::testFirstOneBlankBoard ) );
	    suiteOfTests->addTest( new CppUnit::TestCaller<BitTest>(
								"testFirstOneSecondBlock",
								&BitTest::testFirstOneSecondBlock ) );
	    suiteOfTests->addTest( new CppUnit::TestCaller<BitTest>(
								"testFirstOneThirdBlock",
								&BitTest::testFirstOneThirdBlock ) );
	    suiteOfTests->addTest( new CppUnit::TestCaller<BitTest>(
								"testFirstOneFourthBlock",
								&BitTest::testFirstOneFourthBlock ) );
	    suiteOfTests->addTest( new CppUnit::TestCaller<BitTest>(
	    						"testLastOne",
	    						&BitTest::testLastOne ) );
	    return suiteOfTests;
	  }

	  void testFirstOne() {
		  BITBOARD i = 0x1000000000000000ULL;
		  short firstOne = FirstOne(i);
		  CPPUNIT_ASSERT_EQUAL((short) 60, firstOne);
	  }

	  void testFirstOneFirstBit() {
		  CPPUNIT_ASSERT_EQUAL((short) 63, FirstOne(0x8000000000000000ULL));
	  }

	  void testFirstOneBlankBoard() {
		  // not well defined when the board is empty -- this value
		  // should never be semantically used
		  CPPUNIT_ASSERT_EQUAL((short) 47, FirstOne(0ULL));
	  }

	  void testFirstOneSecondBlock() {
		  CPPUNIT_ASSERT_EQUAL((short) 42, FirstOne(0x0000040000000000ULL));
	  }

	  void testFirstOneThirdBlock() {
		  CPPUNIT_ASSERT_EQUAL((short) 16, FirstOne(0x0000000000010000ULL));
	  }

	  void testFirstOneFourthBlock() {
		  CPPUNIT_ASSERT_EQUAL((short) 11, FirstOne(0x0000000000000800ULL));
	  }

	  void testLastOne() {
		  BITBOARD i = 0x1000000000000000ULL;
		  short firstOne = FirstOne(i);
		  CPPUNIT_ASSERT_EQUAL((short) 60, firstOne);
	  }


//	void testFirstOne() {
//		BITBOARD i =   0x1000000000000001ULL;
//		for(int j = 0; j < 64; ++j) {
//			if (j != 0)
//				i = i << 1;
//			short firstOneOld = FirstOne(i);
//			short firstOneNew = FirstOneNew(i);
//			std::string message = "different FirstOne for ";
//			message += BITBOARD_to_string(i);
//			message += "\n" + bitboard_to_string(i);
//			CPPUNIT_ASSERT_EQUAL_MESSAGE(message, firstOneOld, firstOneNew);
//		}
//
//		cerr << "finished testing first ones" << endl;
//	}
	
};

#endif /*BITTEST_H_*/
