[![CircleCI](https://circleci.com/gh/tildedave/apep-chess-engine.svg?style=svg)](https://circleci.com/gh/tildedave/apep-chess-engine)

This is a chess engine that I programmed in grad school (C++).  I'm sure a lot of the code is 
bad/could have been done better.  Looking at the code I couldn't find the tests, which is a good 
sign that I messed something up.

I did most of the search stuff and stopped active development when the engine needed to 
understand the "goodness" of a board.

Major points:

* In order to add new functionality at this point I need to do some serious refactoring.

* Main refactoring targets are the search code and the xboard code.  The bitboard/move generation code is a little large, but seems to have been written in an okay fashion.

* _Very_ bad at improving its position/advantages in the middlegame, basically it will wait for you to mess up.

* Fails to castle in "obvious" situations

* Bad at enemy/own king safety (there is code, it doesn't seem to work)
 - I think I fixed this during a 2010 refactor

* Need to do a fairly deep search (4M nodes) to work out Lucena endgame

* Something wrong with timing code: even on checkmate next move will search for 5 seconds (might be a bug in how we handle xboard messages)
 - I think I fixed this during a 2010 refactor

Minor points:

* Code is pretty bad.

* I got a lot out of looking at crafty source code (hopefully the resemblence is not too obvious).

* There are some epd chess board files that I used to test this engine that I have not included in this repo as they are not open source.

* apep supports parts of the Winboard Protocol v1 http://home.hccnet.nl/h.g.muller/engine-intf.html.  If you load it in xboard/winboard it should play against you without error.  It is a little wonkier with analysis.  Work is in progress to fix these bugs.
 - Work is not in progress, I'm not updating this repo
