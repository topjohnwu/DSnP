/****************************************************************************
  FileName     [ dbTable.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Table member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <string>
#include <cctype>
#include <cassert>
#include <set>
#include <algorithm>
#include <sstream>
#include "dbTable.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBRow& r)
{
   for(int i = 0; i < r.size(); i++) {
      if(r[i] != INT_MAX) os << r[i];
      else os << ".";
      if(i != r.size() - 1) cout << " ";
   }
   return os;
}

ostream& operator << (ostream& os, const DBTable& t)
{
   for(int row = 0; row < t.nRows(); row++) {
      for(int col = 0; col < t.nCols(); col++) {
        if(t[row][col] == INT_MAX) os << setw(6) << right << ".";
        else os << setw(6) << right << t[row][col];
      }
      os << endl;
   }
   return os;
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{
   string s;
   getline(ifs, s);
   stringstream ss(s);
   bool done = false;
   while(1) {
      if(done) break;
      bool flag = true;
      DBRow* newRow = new DBRow();
      while(1) {
        if(ss.peek() == ',') {
          if(flag) newRow->addData(INT_MAX);
          flag = true;
          ss.ignore();
        }
        else if(ss.peek() == '\r') {
          if(flag) newRow->addData(INT_MAX);
          ss.ignore();
          if(ss.peek() == '\r' || ss.peek() == EOF) done = true;
          break;
        }
        else {
          int readin;
          ss >> readin;
          newRow->addData(readin);
          flag = false;
        }
      }
      t.addRow(*newRow);
      delete newRow;
   }
   return ifs;
}

/*****************************************/
/*   Member Functions for class DBRow    */
/*****************************************/
void
DBRow::removeCell(size_t c)
{
   _data.erase(_data.begin() + c);
}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
   for(int i = 0; i < _sortOrder.size(); i++) {
      if(r1[_sortOrder[i]] < r2[_sortOrder[i]]) return true;
      else if(r1[_sortOrder[i]] == r2[_sortOrder[i]]) continue;
      else return false;
   }
   return false;
}

/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void
DBTable::reset()
{
   for(int i = 0; i < nRows(); i++) _table[i].reset();
   vector<DBRow> temp;
   temp.swap(_table); 
}

void
DBTable::addCol(const vector<int>& d)
{
   for(int i = 0; i < nRows(); i++) 
      _table[i].addData(d[i]);
}

void
DBTable::delRow(int c)
{
   _table.erase(_table.begin() + c);
}

void
DBTable::delCol(int c)
{
   for (size_t i = 0, n = nRows(); i < n; ++i)
      _table[i].removeCell(c);
}

float
DBTable::getMax(size_t c) const
{
   vector<int> col;
   for(int i = 0; i < nRows(); i++) {
      if(_table[i][c] != INT_MAX) {
        col.push_back(_table[i][c]);
      }
   }
   if(col.size()) return *max_element(col.begin(), col.end());
   else return NAN;
}

float
DBTable::getMin(size_t c) const
{
   vector<int> col;
   for(int i = 0; i < nRows(); i++) {
      if(_table[i][c] != INT_MAX) {
        col.push_back(_table[i][c]);
      }
   }
   if(col.size()) return *min_element(col.begin(), col.end());
   else return NAN;
}

float 
DBTable::getSum(size_t c) const
{
   int sum = 0;
   int num = 0;
   for(int i = 0; i < nRows(); i++) {
      if(_table[i][c] != INT_MAX) {
        num++;
        sum += _table[i][c];
      }
   }
   if(num) return sum;
   else return NAN;
}

int
DBTable::getCount(size_t c) const
{
   set<int> uniq_col;
   for(int i = 0; i < nRows(); i++) {
      if(_table[i][c] != INT_MAX) {
        uniq_col.insert(_table[i][c]);
      }
   }
   return uniq_col.size();
}

float
DBTable::getAve(size_t c) const
{
   int sum = 0;
   int num = 0;
   for(int i = 0; i < nRows(); i++) {
      if(_table[i][c] != INT_MAX) {
        num++;
        sum += _table[i][c];
      }
   }
   if(num) return ((float)sum)/num;
   else return NAN;
}

void
DBTable::sort(const struct DBSort& s)
{
   ::sort(_table.begin(), _table.end(), s);
}

void
DBTable::printCol(size_t c) const
{
   for(int i = 0; i < nRows(); i++) {
      if(_table[i][c] == INT_MAX) cout << ".";
      else cout << _table[i][c];
      if(i != nRows() - 1) cout << " ";
   }
   cout << endl;
}

void
DBTable::printSummary() const
{
   size_t nr = nRows(), nc = nCols(), nv = 0;
   for (size_t i = 0; i < nr; ++i)
      for (size_t j = 0; j < nc; ++j)
         if (_table[i][j] != INT_MAX) ++nv;
   cout << "(#rows, #cols, #data) = (" << nr << ", " << nc << ", "
        << nv << ")" << endl;
}

