//-------------------------------------------------
// TODO:  Write file header.
//-------------------------------------------------
//-------------------------------------------------
// TODO:  Update the member function comments in 
//        your own style.
//-------------------------------------------------

#pragma once

#include <cstddef>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <algorithm>

using namespace std;

template <typename T>
struct ROW {
  T *Cols;
  size_t NumCols;

  ROW(): NumCols(4) {
    Cols = new T[4];
    for (size_t i = 0; i < 4; ++i) {
      Cols[i] = T();
    }
  }
  ROW(size_t C): NumCols(C) {
    Cols = new T[C];
    for (size_t i = 0; i < C; ++i) {
      Cols[i] = T();
    }
  }
  ~ROW() { delete[] Cols; }
  ROW(const ROW &source) {
    NumCols = source.NumCols;
    copy(source.Cols, source.Cols + source.NumCols, Cols);
  }
  ROW& operator=(const ROW &source) {
    if (this != &source) {
      delete[] Cols;
      Cols = new T[source.NumCols];
      NumCols = source.NumCols;
      copy(source.Cols, source.Cols + source.NumCols, Cols);
    }

    return *this;
  }
};

template <typename T>
class Grid {
private:

  ROW<T>* Rows;     // array of ROWs
  size_t  NumRows;  // total # of rows (0..NumRows-1)
    
public:
  //
  // default constructor:
  //
  // Called automatically by to construct a 4x4 Grid. 
  // All elements initialized to default value of T.
  //
  Grid(): Rows(new ROW<T>[4]), NumRows(4) {}

  Grid(size_t R, size_t C) : NumRows(R) {
    Rows = new ROW<T>[R];

    for (size_t r = 0; r < R; ++r) {
      Rows[r] = ROW<T>(C);
    }
  }
    
  //
  // destructor:
  //
  // Called automatically to free memory for this Grid.
  //
  virtual ~Grid() {
    delete[] Rows;
  }
  //
  Grid(const Grid<T>& source) {
    NumRows = source.NumRows; // copies NumRows
    Rows = new ROW<T>[NumRows]; // creates empty ROW array

    copy(source.Rows, source.Rows + source.NumRows, Rows);
  }
    
  //
  // copy operator=
  //
  // Called when one Grid is assigned into another, i.e. this = other;
  //
  Grid &operator=(const Grid &source) {
    // TODO consider using uninitialized_copy from "<memory>" to remove redundant code
    if (this != &source) {
      delete[] Rows; // deallocate memory for ROW array (calls destructor for each ROW object)
      NumRows = source.NumRows; // set new NumRows
      Rows = new ROW<T>[NumRows]; // create new ROW array
      copy(source.Rows, source.Rows + source.NumRows, Rows); // copy row data from source to this (calls copy operator for each ROW object)
    }

    return *this;
      
  }

  //
  // numrows
  //
  // Returns the # of rows in the Grid.  
  // The indices for these rows are 0..numrows-1.
  //
  size_t numrows() const {return NumRows;}
  
  //
  // numcols
  //
  // Returns the # of columns in row r.  
  // The indices for these columns are 0..numcols-1.  
  // For now, each row has the same number of columns.
  //
  size_t numcols(size_t r) const {
    // if no data, return 0 else return NumCols of first row since grid shape is rectangular
    return (NumRows == 0) ? 0 : Rows[0].NumCols;
  }


  //
  // size
  //
  // Returns the total # of elements in the Grid.
  //
  size_t size() const {
    return (NumRows == 0) ? 0 : NumRows * Rows[0].NumCols;
  }


  //
  // ()
  //
  // Returns a reference to the element at location (r, c);
  // this allows you to access or assign the element:
  //
  //    grid(r, c) = ...
  //    cout << grid(r, c) << endl;
  //
  T& operator()(size_t r, size_t c) {

      if (r >= NumRows || c >= Rows[0].NumCols) {
        throw invalid_argument("Index out of bounds");
      }
      return Rows[r].Cols[c];
  }

  //
  // _output
  //
  // Outputs the contents of the grid; for debugging purposes.  
  // This is not tested.
  //
  void _output() {

    if (NumRows == 0) {
      return;
    }

    size_t nCols = Rows[0].NumCols;

    for (size_t r = 0; r < NumRows; ++r) {
      cout << "|    ";
      for (size_t c = 0; c < nCols; ++c) {
        cout << "|  " << Rows[r].Cols[c] << "  |  ";
      }
      cout << "  |" << endl;
    }
      
      
  }

};
