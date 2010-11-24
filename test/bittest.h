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
	    						"testLastOneFirstBlock",
	    						&BitTest::testLastOneFirstBlock ) );
	    suiteOfTests->addTest( new CppUnit::TestCaller<BitTest>(
	    						"testLastOneSecondBlock",
	    						&BitTest::testLastOneSecondBlock ) );
	    suiteOfTests->addTest( new CppUnit::TestCaller<BitTest>(
	    						"testLastOneThirdBlock",
	    						&BitTest::testLastOneThirdBlock ) );
	    suiteOfTests->addTest( new CppUnit::TestCaller<BitTest>(
	    						"testLastOneFourthBlock",
	    						&BitTest::testLastOneFourthBlock ) );
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

	  void testLastOneFirstBlock() {
		  BITBOARD i = 0x1000000000000000ULL;
		  short firstOne = FirstOne(i);
		  CPPUNIT_ASSERT_EQUAL((short) 60, firstOne);
	  }

	  void testLastOneSecondBlock() {
		  BITBOARD i = 0x0000800000000000ULL;
		  short firstOne = FirstOne(i);
		  CPPUNIT_ASSERT_EQUAL((short) 47, firstOne);
	  }

	  void testLastOneThirdBlock() {
		  BITBOARD i = 0x0000000004000000ULL;
		  short firstOne = FirstOne(i);
		  CPPUNIT_ASSERT_EQUAL((short) 26, firstOne);
	  }

	  void testLastOneFourthBlock() {
		  BITBOARD i = 0x1000000000000300ULL;
		  short firstOne = FirstOne(i);
		  CPPUNIT_ASSERT_EQUAL((short) 8, firstOne);
	  }
};

#endif /*BITTEST_H_*/
