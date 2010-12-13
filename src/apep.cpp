//============================================================================
// Name        : apep.cpp
// Author      : Dave King
// Version     :
// Copyright   : GNU Lesser General Public License
// Description : Main function for apep.  XBoard/UCI interface
//============================================================================

#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <time.h>
#ifndef WIN32
#include <sys/time.h>
#endif
#include "commonboards.h"
#include "board.h"
#include "movepriorityqueue.h"
#include "movegen.h"
#include "moveprocess.h"
#include "move.h"
#include "search.h"
#include "xboard.h"
#include "openbook.h"


using namespace std;

int randomSeed = -1;

int main(int argc, char** argv) {
	if (argc >= 2 && std::string(argv[1]) == "-random") {
		randomSeed = atoi(argv[2]);
	}
	initialize_common_boards();
	loadOpeningBook("bookl.dat");

	xboardMainLoop();

	return 0;
}
