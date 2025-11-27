// gameboard.h
//
// GameBoard class starter code for course project
// University of Illinois at Chicago
// CS 211 - Programming Practicum
// Original Author: Professor Scott Reckinger
//

#ifndef _GAMEBOARD_H
#define _GAMEBOARD_H

#include <cstdlib>
#include <ctime>
#include <exception>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>

#include "boardcell.h"
#include "grid.h"

using namespace std;

class OutOfBoundsError : public exception {
private:
  string errorMsg;
  bool invalidRow;
  bool invalidCol;

public:
  OutOfBoundsError(const string &eMsg, bool eRow, bool eCol)
      : errorMsg(eMsg), invalidRow(eRow), invalidCol(eCol) {}
  const char *what() const noexcept override { return errorMsg.c_str(); }
  bool isInvalidRow() { return invalidRow; }
  bool isInvalidCol() { return invalidCol; }
};

class BarrierCollisionError : public exception {
private:
  string errorMsg;

public:
  BarrierCollisionError(const string &msg) : errorMsg(msg) {}
  const char *what() const noexcept override { return errorMsg.c_str(); }
};

class GameBoard {
private:
  Grid<BoardCell *> board;
  size_t numRows;
  size_t numCols;
  size_t HeroRow; // Hero's position row
  size_t HeroCol; // Hero's position column
  int numMonsters;
  int numSuperMonsters;
  int numAbysses;
  int numBats;
  bool wonGame; // false, unless the Hero reached the exit successfully

public:
  /* default constructor */
  GameBoard() {
    numMonsters = 4;
    numSuperMonsters = 2;
    numAbysses = 50;
    numBats = 2;
    wonGame = false;

    this->numRows = 15;
    this->numCols = 40;

    Grid<BoardCell *> boardnew(numRows, numCols);
    board = boardnew;

    blankBoard();
  }

  /* param constructor */
  GameBoard(size_t numRows, size_t numCols) {
    numMonsters = 4;
    numSuperMonsters = 2;
    numAbysses = 20;
    numBats = 3;
    wonGame = false;

    this->numRows = numRows;
    this->numCols = numCols;

    Grid<BoardCell *> boardnew(numRows, numCols);
    board = boardnew;

    blankBoard();
  }

  /* destructor */
  virtual ~GameBoard() {
    for (size_t row = 0; row < board.numrows(); row++) {
      for (size_t col = 0; col < board.numcols(row); col++) {
        delete board(row, col);
      }
    }
  }

  void blankBoard() {
    for (size_t row = 0; row < board.numrows(); row++) {
      for (size_t col = 0; col < board.numcols(row); col++) {
        board(row, col) = new Nothing(row, col);
      }
    }
  }

  char getCellDisplay(size_t r, size_t c) { return board(r, c)->display(); }

  void setCell(BoardCell *myCell, size_t r, size_t c) { board(r, c) = myCell; }

  void freeCell(size_t r, size_t c) { delete board(r, c); }

  // fills board with by randomly placing...
  //  - Hero (H) in the first three columns
  //  - EscapeLadder (*) in last three columns
  //  - 3 vertical Walls (+), each 1/2 of board height, in middle segment
  //  - Abyss cells (#), quantity set by numAbysses, in middle segment
  //  - Baddies [Monsters (m), Super Monsters (M), & Bats (~)] in middle
  //  segment;
  //    number of Baddies set by numMonsters, numSuperMonsters, & numBats
  void setupBoard(int seed) {
    srand(seed);
    size_t r, c;

    r = rand() % numRows;
    c = rand() % 3;
    delete board(r, c);
    board(r, c) = new Hero(r, c);
    HeroRow = r;
    HeroCol = c;

    r = rand() % numRows;
    c = numCols - 1 - (rand() % 3);
    delete board(r, c);
    board(r, c) = new EscapeLadder(r, c);

    int sizeMid = numCols - 6;

    c = 3 + (rand() % sizeMid);
    for (r = 0; r < numRows / 2; ++r) {
      delete board(r, c);
      board(r, c) = new Wall(r, c);
    }
    size_t topc = c;

    while (c == topc || c == topc - 1 || c == topc + 1) {
      c = 3 + (rand() % sizeMid);
    }
    for (r = numRows - 1; r > numRows / 2; --r) {
      delete board(r, c);
      board(r, c) = new Wall(r, c);
    }
    size_t botc = c;

    while (c == topc || c == topc - 1 || c == topc + 1 || c == botc ||
           c == botc - 1 || c == botc + 1) {
      c = 3 + (rand() % sizeMid);
    }
    for (r = numRows / 4; r < 3 * numRows / 4; ++r) {
      delete board(r, c);
      board(r, c) = new Wall(r, c);
    }

    for (int i = 0; i < numMonsters; ++i) {
      r = rand() % numRows;
      c = 3 + (rand() % sizeMid);
      while (board(r, c)->display() != ' ') {
        r = rand() % numRows;
        c = 3 + (rand() % sizeMid);
      }
      delete board(r, c);
      board(r, c) = new Monster(r, c);
      board(r, c)->setPower(1);
    }

    for (int i = 0; i < numSuperMonsters; ++i) {
      r = rand() % numRows;
      c = 3 + (rand() % sizeMid);
      while (board(r, c)->display() != ' ') {
        r = rand() % numRows;
        c = 3 + (rand() % sizeMid);
      }
      delete board(r, c);
      board(r, c) = new Monster(r, c);
      board(r, c)->setPower(2);
    }

    for (int i = 0; i < numBats; ++i) {
      r = rand() % numRows;
      c = 3 + (rand() % sizeMid);
      while (board(r, c)->display() != ' ') {
        r = rand() % numRows;
        c = 3 + (rand() % sizeMid);
      }
      delete board(r, c);
      board(r, c) = new Bat(r, c);
    }

    for (int i = 0; i < numAbysses; ++i) {
      r = rand() % numRows;
      c = 3 + (rand() % sizeMid);
      while (board(r, c)->display() != ' ') {
        r = rand() % numRows;
        c = 3 + (rand() % sizeMid);
      }
      delete board(r, c);
      board(r, c) = new Abyss(r, c);
    }
  }

  // neatly displaying the game board
  void display() {
    cout << '-';
    for (size_t col = 0; col < board.numcols(0); col++) {
      cout << '-';
    }
    cout << '-';
    cout << endl;
    for (size_t row = 0; row < board.numrows(); row++) {
      cout << '|';
      for (size_t col = 0; col < board.numcols(row); col++) {
        cout << board(row, col)->display();
      }
      cout << '|';
      cout << endl;
    }
    cout << '-';
    for (size_t col = 0; col < board.numcols(0); col++) {
      cout << '-';
    }
    cout << '-';
    cout << endl;
  }

  bool getWonGame() { return wonGame; }

  // distributing total number of monsters so that
  //  ~1/3 of num are Super Monsters (M), and
  //  ~2/3 of num are Regular Monsters (m)
  void setNumMonsters(int num) {
    numSuperMonsters = num / 3;
    numMonsters = num - numSuperMonsters;
  }

  void setNumAbysses(int num) { numAbysses = num; }

  void setNumBats(int num) { numBats = num; }

  size_t getNumRows() { return board.numrows(); }

  size_t getNumCols() { return board.numcols(0); }

  //---------------------------------------------------------------------------------
  // void getHeroPosition(size_t& row, size_t& col)
  //
  // getter for Hero's position, which are private data members
  //      int HeroRow;
  //      int HeroCol;
  // note: row and col are passed-by-reference
  //---------------------------------------------------------------------------------
  void getHeroPosition(size_t &row, size_t &col) {
    row = HeroRow;
    col = HeroCol;
  }

  //---------------------------------------------------------------------------------
  // void setHeroPosition(size_t row, size_t col)
  //
  // setter for Hero's position, which are private data members
  //      int HeroRow;
  //      int HeroCol;
  //---------------------------------------------------------------------------------
  void setHeroPosition(size_t row, size_t col) {
    HeroRow = row;
    HeroCol = col;
  }

  //---------------------------------------------------------------------------------
  // findHero()
  //
  // updater for Hero's position, which are private data members
  //      int HeroRow;
  //      int HeroCol;
  // this function should find Hero in board and update
  //      HeroRow and HeroCol with the Hero's updated position;
  // if Hero cannot be found in board, then set Hero's position to (-1,-1)
  //---------------------------------------------------------------------------------
  void findHero() {
    if (HeroRow < board.numrows() && HeroCol < board.numcols(0)) {
      for (size_t r = 0; r < board.numrows(); ++r) {
        for (size_t c = 0; c < board.numcols(r); ++c) {
          if (board(r, c)->isHero()) {
            HeroRow = r;
            HeroCol = c;
            return;
          }
        }
      }
    }

    HeroRow = -1;
    HeroCol = -1;
  }

  //---------------------------------------------------------------------------------
  // bool makeMoves(char HeroNextMove)
  //
  // This is the primary gameplay operation for a single round of the game.
  // A LOT happens in this function...
  // General steps:
  //  - Allow user to input their next move
  //  - Get the attempted move position for the Hero
  //  - Move the hero, catching/resolving any potential collision exceptions...
  //      - attempted move out-of-bounds: change row &/or col to stay on board
  //      - attempted move into a barrier: change row &/or col to stay off
  //      barrier
  //      - attempted move to the exit: remove hero from board, hero escaped!
  //      - attempted move into a hole: remove hero from board, hero did not
  //      escape
  //      - attempted move to a baddie: remove hero from board, hero did not
  //      escape
  //      - attempted move to an empty space: actual move is the attempted move
  //  - For each baddie (regular Monster, super Monster, or Bat) on the board...
  //      - check that this baddies hasn't already moved this round
  //      - get its attempted move position
  //      - move the baddie, catching/resolving any potential collision
  //      exceptions...
  //          - attempted move out-of-bounds: change row &/or col to stay on
  //          board
  //          - attempted move into a barrier: change row &/or col to stay off
  //          barrier
  //          - attempted move to the exit: change row &/or col to stay off exit
  //          - attempted move into a hole: remove baddie from board
  //          - attempted move to hero: remove hero from board, hero did not
  //          escape
  //          - attempted move to a baddie: ignore attempted move, no position
  //          change
  //          - attempted move to an empty space: actual move is the attempted
  //          move
  //
  // Note: all baddies (and the hero) fall into holes if their attempted
  //       move ENDs on a hole (i.e. even Bats are drawn into the
  //       Abyss if their attempted move takes them to an Abyss cell);
  //       BUT, baddies can travel over holes, as long as their attempted
  //       move does NOT END on a hole; this only applies, in practice,
  //       to super monsters and bats, since their step sizes can be more
  //       than 1 (monsters and the hero are limited to steps of size 1)
  //
  // Note: also, whereas baddies (and the hero) can never move onto a
  //       barrier (i.e. a wall), they can step over barriers as long
  //       as the attempted move does NOT END on a barrier; this only
  //       applies, in practice, to super monsters and bats, since their
  //       step sizes can be more than 1 (monsters and the hero are limited
  //       to steps of size 1)
  //
  // Note: to prevent a single baddie from making multiple moves
  //       during a single round of the game, whenever a baddie
  //       has moved, it should be marked as "already moved" in
  //       some way; this can be done using the [moved] data member
  //       of the BoardCell base class, which has setter/getter
  //       functions provided. The [moved] data members must be
  //       reset for all BoardCells at the beginning of each round.
  //
  // Note: the [myRow] and [myCol] data members for BoardCell derived
  //       class objects must be updated whenever a move is made;
  //       additionally, [HeroRow] and [HeroCol] data members for the
  //       GameBoard must be updated whenever the Hero has moved,
  //       which can be done easily with a call to findHero()
  //
  // Note: many actual moves made by non-static board cell objects
  //       (i.e. Heros, Monsters, Bats) involve constructing and/or
  //       destructing objects; be careful with memory management;
  //       specifically, make sure to free (delete) the memory for
  //       BoardCell derived class objects when you are done with it
  //
  // return true if Hero is still on board at the end of the round
  // return false if Hero is NOT on board at the end of the round
  //---------------------------------------------------------------------------------
  bool makeMoves(char HeroNextMove) {
    //-----------------------------------
    // TODO: write this gameplay function
    //-----------------------------------
    //
    // this function should use some try/catch statements for handling collision
    // exceptions; some sample code is provided to get you started...
    // get user move

    // determine where hero proposes to move to
    size_t newR, newC;
    bool rowChanged, colChanged = false;
    board(HeroRow, HeroCol)->setNextMove(HeroNextMove);
    board(HeroRow, HeroCol)
        ->attemptMoveTo(newR, newC, rowChanged, colChanged, HeroRow, HeroCol);

    if (!rowChanged && !colChanged) {
      cout << "Hero remains in place\n";
      return true;
    }

    try {
      if (tryMove(board(HeroRow, HeroCol), newR, newC)) {
        return true;
      }
      if (board(newR, newC)->isExit()) {
        cout << "Hero escaped\n";
        delete board(HeroRow, HeroCol);
        return false;
      }
      cout << "Hero destroyed. Game Over.\n";
      delete board(HeroRow, HeroCol);
      return false;
    } catch (OutOfBoundsError &e) {
      newR = (newR < 0 || newR >= numRows) ? HeroRow : newR;
      newC = (newC < 0 || newC >= numCols) ? HeroCol : newC;
      // try move to (newR, newC)
    } catch (BarrierCollisionError &e) {
      if (rowChanged && colChanged) {
        if (board(newR, HeroCol)->)
      }
    }
    try {
      // hero attempts to move out-of-bounds in rows
      if (newR < 0 || newR >= numRows || newC < 0 || newC >= numCols) {
        throw OutOfBoundsError("Hero's attempted move is out of bounds",
                               newR < 0 || newR >= numRows,
                               newC < 0 || newC >= numCols);
      } else if (board(newR, newC)->isBarrier()) {
        throw BarrierCollisionError("Hero's attempted move blocked by barrier");
      } else if (board(newR, newC)->isExit()) {
        delete board(HeroRow, HeroCol);
        cout << "Hero escaped!" << endl;
        return false;

      } else if (board(newR, newC)->isHole()) {
        delete board(HeroRow, HeroCol);
        cout << "Hero fell into the abyss! Game over" << endl;
        return false;
      } else if (board(newR, newC)->isBaddie()) {
        delete board(HeroRow, HeroCol);
        cout << "Hero was defeated by a monster! Game over" << endl;
        return false;
      } else if (board(newR, newC)->isSpace()) {
        board(HeroRow, HeroCol)->setMoved(true);
        board(HeroRow, HeroCol)->setPos(newR, newC);

        delete board(newR, newC); // destroy Nothing cell
        board(newR, newC) = board(
            HeroRow, HeroCol); // set cell pointer to hold hero cell pointer
        board(HeroRow, HeroCol) = new Nothing(
            HeroRow, HeroCol); // set old hero cell pointer to new Nothing cell

        HeroRow = newR;
        HeroCol = newC;

        return true;
      }
    } catch (OutOfBoundsError &e) {
      cout << e.what() << endl;
      if (e.isInvalidRow()) {
        if (newR >= numRows) {
          newR = numRows - 1;
        } else {
          newR = 0;
        }
      }
      if (e.isInvalidCol()) {
        if (newC >= numCols) {
          newC = numCols - 1;
        } else {
          newC = 0;
        }
      }
      cout << "Changing row for Hero position to stay in-bounds" << endl;

    } catch (BarrierCollisionError &e) {
      cout << e.what() << endl;
      if (newR == HeroRow || newC == HeroCol) {
        cout << "Barrier collision, Hero unmoved" << endl;
        return true;
      } else {
        newC = HeroCol;
        // check attempt move for board(newR, newC);
      }
    }
    // etc.

    for (size_t r = 0; r < numRows; ++r) {
      for (size_t c = 0; c < numCols; ++c) {
        if (board(r, c)->isBaddie()) {
          board(r, c)->attemptMoveTo(newR, newC, HeroRow, HeroCol);
          try {
            if (newR >= numRows || newR < 0) {
              throw OutOfBoundsError("write error message");
            }
          }
        }
      }
    }

    return false;
  }

  bool tryMove(BoardCell *&cell, size_t tryRow, size_t tryCol) {
    if (cell->isStatic()) {
      throw runtime_error("Cannot call tryMove on a static cell");
    }
    if (tryRow < 0 || tryRow >= numRows || tryCol < 0 || tryCol >= numCols) {
      throw OutOfBoundsError("attempted move is out of bounds",
                             tryRow < 0 || tryRow >= numRows,
                             tryCol < 0 || tryCol >= numCols);
    }
    if (board(tryRow, tryCol)->isBarrier()) {
      throw BarrierCollisionError("attempted to move into wall");
    }

    if (board(tryRow, tryCol)->isSpace()) {
      delete board(tryRow, tryCol);
      board(tryRow, tryCol) = cell;
      board(tryRow, tryCol)->setMoved(true);
      cell = new Nothing(cell->getRow(), cell->getCol());
      return true;
    }
    return false;
  }
};

#endif //_GAMEBOARD_H