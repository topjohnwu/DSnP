/****************************************************************************
  FileName     [ dbTable.h ]
  PackageName  [ db ]
  Synopsis     [ Define database Table classes ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef DB_TABLE_H
#define DB_TABLE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <climits>

using namespace std;

class DBRow
{
public:
   DBRow() {}
   DBRow(const vector<int>& d) : _data(d) {}

   void reset() { vector<int> tmp; tmp.swap(_data); }
   int& operator [] (size_t i) { return _data[i]; }
   const int& operator [] (size_t i) const { return _data[i]; }
   void addData(int i) { _data.push_back(i); }
   void removeCell(size_t c);
   size_t size() const { return _data.size(); }
   bool empty() const { return _data.empty(); }

   friend ostream& operator << (ostream& os, const DBRow& r);

private:
   vector<int>   _data;
};

struct DBSort
{
   bool operator() (const DBRow& r1, const DBRow& r2) const;
   void pushOrder(size_t i) { _sortOrder.push_back(i); }
   vector<size_t> _sortOrder;
};

class DBTable
{
public:
   void reset();
   // access functions for row/column
   const DBRow& operator [] (size_t i) const { return _table[i]; }
   void addRow(const DBRow& r) { _table.push_back(r); }
   void addCol(const vector<int>& d);
   void delRow(int c);
   void delCol(int c);
   // check whether the table is empty or not-empty
   bool operator !() { return _table.empty(); }
   operator void* () const { return _table.empty()? NULL: (void*)this; }
   //number of rows/columns
   size_t nRows() const { return _table.size(); }
   size_t nCols() const { if(nRows()) return _table[0].size(); return 0; }
   // "getXXX" functions
   int getData(size_t r, size_t c) const { return _table[r][c]; }
   float getMax(size_t c) const;
   float getMin(size_t c) const;
   float getSum(size_t c) const;
   int getCount(size_t c) const;
   float getAve(size_t c) const;

   void sort(const struct DBSort& s);

   // printing functions
   void printCol(size_t c) const;
   void printSummary() const;

   static void printData(ostream& os, int i, bool printNull = false) {
      if (i == INT_MAX) os << (printNull? "null":".");
      else os << i;
   }

   friend ifstream& operator >> (ifstream& ifs, DBTable& t);
   friend ostream& operator << (ostream& os, const DBTable& t);

private:
   vector<DBRow>    _table;

   // Helper functions; You are welcome to define on your own
};

#endif // DB_TABLE_H
