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
#include <cstdio>

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
      for (size_t r = 0; r < getNumRows(); ++r) {
        for (size_t c = 0; c < getNumCols(); ++c) {
          if (board(r, c)->isHero()) {
            HeroRow = r;
            HeroCol = c;
            return;
          }
        }
      }

    HeroRow = -1;
    HeroCol = -1;
  }

  //---------------------------------------------------------------------------------
  // bool  Moves(char HeroNextMove)
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
  void swapCells(BoardCell *&lhs, BoardCell *&rhs) {
    BoardCell *tempLh = lhs;
    lhs = rhs;
    rhs = tempLh;
  }

  void adjustMove(BoardCell *thisCell, size_t &row, size_t &col) {
    /* adjustMove handles any invalid coordinates by adjusting them until
    they are valid. Does not make any moves, only changes coordinates if
    invalid*/
    // cout << "Original target position: (" << row << ", " << col << ")\n";
    if (row < 0 || row >= numRows) {
      // if attempted row is invalid, reset row to current row
      row = thisCell->getRow();
    }

    if (col < 0 || col >= numCols) {
      // if attempted column is invalid, reset column to current column
      col = thisCell->getCol();
    }

    if (board(row, col)->isBarrier() ||
        (board(row, col)->isExit() && thisCell->isBaddie())) {
      // if adjusted target cell is a wall cell, OR target cell is an exit and
      // the mover is a baddie, reset column to current
      // column (ignore horizontal direction)
      col = thisCell->getCol();
      if (board(row, col)->isBarrier() ||
          (board(row, col)->isExit() && thisCell->isBaddie())) {
        // if secondary adjustment is still invalid, reset row to
        // current row
        row = thisCell->getRow();
      }
    }
    // cout << "New target position: (" << row << ", " << col << ")\n";
  }
  void resetMoved() {
    for (size_t r = 0; r < numRows; ++r) {
      for (size_t c = 0; c < numCols; ++c) {
        if (board(r,c)->isBaddie()) {
          board(r,c)->setMoved(false);
        }
      }
    }
  }
  bool makeMoves(char HeroNextMove) {
    //-----------------------------------
    // TODO: write this gameplay function
    //-----------------------------------
    //
    // this function should use some try/catch statements for handling collision
    // exceptions; some sample code is provided to get you started...
    // get user move

    // START HERO MOVEMENT
    size_t newR, newC;
    size_t oldHRow = HeroRow, oldHCol = HeroCol;
    board(HeroRow, HeroCol)->setNextMove(HeroNextMove);
    board(HeroRow, HeroCol)->attemptMoveTo(newR, newC, HeroRow, HeroCol);
    printf("Attempting Hero Move (%zu, %zu) --> (%zu, %zu)\n", HeroRow, HeroCol, newR, newC);
    adjustMove(board(HeroRow, HeroCol), newR, newC); // validate coordinates
    printf("Updated target, new Hero Move: (%zu, %zu) --> (%zu, %zu)\n", HeroRow, HeroCol, newR, newC);

    if (board(HeroRow,HeroCol) != board(newR,newC)) {
      // if Hero needs moved (target cell is not the same as current cell)
      try {
        if (board(newR, newC)->isSpace()) {
          // target cell is empty, move is made
          swap(board(HeroRow, HeroCol),
              board(newR, newC)); // swap hero and space cell
          HeroRow = newR;
          HeroCol = newC;
          board(HeroRow, HeroCol)->setPos(HeroRow, HeroCol);
        } else if (board(newR, newC)->isExit()) {
          // target cell is exit, game over hero escaped
          cout << "Hero escaped\n";
          delete board(HeroRow, HeroCol);
          board(HeroRow, HeroCol) = new Nothing(HeroRow, HeroCol);
          wonGame = true;
          return false;
        } else if (board(newR, newC)->isBaddie() || board(newR, newC)->isHole()) {
          // target cell is baddie or hole, game over hero failed
          cout << "Hero destroyed. Game Over.\n";
          delete board(HeroRow, HeroCol);
          board(HeroRow, HeroCol) = new Nothing(HeroRow, HeroCol);
          return false;
        }
      } catch (runtime_error &e) {
        // return false on runtime error
        cout << e.what() << endl;
        return false;
      }
    }
    board(HeroRow, HeroCol)->setMoved(true);
    
    // END HERO MOVEMENT

    // START BADDIE MOVEMENT
    size_t baddiesCount = 0; // count how many baddies have been iterated over
    for (size_t r = 0; r < numRows; ++r) {
      if (baddiesCount >= numMonsters + numSuperMonsters + numBats) {
        // if all baddies have been iterated over, do not continue checking for
        // baddies
        break;
      }
      for (size_t c = 0; c < numCols; ++c) {
        if (baddiesCount >= numMonsters + numSuperMonsters + numBats) {
          // same check as above but for inner loop (no need to continue
          // iteration if all baddies found)
          break;
        }

        if (board(r, c)->isBaddie() && !board(r,c)->getMoved()) {
          baddiesCount++;
          board(r, c)->attemptMoveTo(newR, newC, HeroRow, HeroCol);
          if (r == oldHRow) {
            newC = c;
          }
          if (c == oldHCol) {
            newR = r;
          }
          printf("Attempting Baddie Move (%zu, %zu) --> (%zu, %zu)\n", r, c, newR, newC);
          adjustMove(board(r, c), newR, newC);
          printf("Updated target, new Baddie Move: (%zu, %zu) --> (%zu, %zu)\n", r, c, newR, newC);

          if (board(r, c) == board(newR, newC)) {
            // if target position is same as currnet position, set moved to true
            // and continue loop
            board(r, c)->setMoved(true);
            continue;
          }

          try {
            if (board(newR, newC)->isSpace()) {
              // target cell is empty, move baddie to target cell
              swap(board(r, c),
                   board(newR, newC)); // swap baddie and empty cell
              board(newR, newC)->setPos(newR, newC);
              board(newR, newC)->setMoved(true);
            } else if (board(newR, newC)->isHero()) {
              // target cell is Hero, Hero destroyed game over
              delete board(HeroRow, HeroCol);
              board(HeroRow, HeroCol) = new Nothing(HeroRow, HeroCol);
              swap(board(r, c), board(HeroRow, HeroCol));
              cout << "Hero destroyed, game over\n";
              return false;
            } else if (board(newR, newC)->isHole()) {
              // target cell is hole, badddie removed from board
              delete board(r, c);
              board(r, c) =
                  new Nothing(r, c); // cell where baddie was is now empty cell
            } else if (board(newR, newC)->isBaddie()) {
              // target cell is another baddie, do not move
              cout << "baddie attempted to move to spot occupied by another "
                      "baddie, remaining in place\n";
              board(r, c)->setMoved(true);
            }
          } catch (runtime_error &e) {
            // return false on runtime error
            cout << e.what() << endl;
            return false;
          }
        }
      }
    }

    resetMoved();
    return true;
  }
};

#endif //_GAMEBOARD_H