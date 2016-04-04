/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

using namespace std;

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   const Data& min() const { return _data.front(); }
   void insert(const Data& d) {
      int idx = _data.size();
      _data.push_back(d);
      while(idx > 0) {
         int p = (idx - 1) / 2;
         if(d < _data[p]) {
            _data[idx] = _data[p];
            idx = p; 
         }
         else break;
      }
      _data[idx] = d;
   }
   void delMin() {
      delData(0);
   }
   void delData(size_t i) {
      Data back = _data.back();
      _data.pop_back();
      // Sink
      int a = i, b = a * 2 + 1;
      if(a == _data.size()) return;
      while(b < _data.size()) {
         if(b + 1 < _data.size()) 
            if(_data[b + 1] < _data[b]) ++b;
         if(back < _data[b]) break;
         _data[a] = _data[b];
         a = b;
         b = a * 2 + 1;
      }
      // Float
      b = (a - 1) / 2;
      while(b > 0) {
         if(back < _data[b]) {
            _data[a] = _data[b];
            a = b;
            b = (a - 1) / 2;
         }
         else break;
      }
      
      _data[a] = back;
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
