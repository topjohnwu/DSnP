/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   Array() : _data(0), _size(0), _capacity(0) {}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      const T& operator * () const { return (*_node); }
      T& operator * () { return (*_node); }
      iterator& operator ++ () { 
         ++_node;
         return (*this);
      }
      iterator operator ++ (int) { 
         iterator temp = *this;
         ++_node;
         return temp;
      }
      iterator& operator -- () { 
         --_node;
         return (*this);
      }
      iterator operator -- (int) {
         iterator temp = *this;
         --_node;
         return temp;
      }

      iterator operator + (int i) const { 
         iterator temp = *this;
         temp += i;
         return temp;
      }
      iterator& operator += (int i) { 
         _node += i;
         return (*this);
      }

      iterator& operator = (const iterator& i) { 
         _node = i._node;
         return (*this);
      }

      bool operator != (const iterator& i) const { 
         if(_node == i._node) return false;
         else return true;
      }
      bool operator == (const iterator& i) const { return !(*this != i); }

   private:
      T*    _node;
   };

   iterator begin() const { return iterator(_data); }
   iterator end() const { return iterator(_data + _size); }
   bool empty() const { return !_size; }
   size_t size() const { return _size; }

   T& operator [] (size_t i) { return _data[i]; }
   const T& operator [] (size_t i) const { return _data[i]; }

   void push_back(const T& x) { 
      if(_size == _capacity) expand();
      _data[_size] = x;
      ++_size;
   }
   void pop_front() { erase(begin()); }
   void pop_back() { if(_size) --_size; }

   bool erase(iterator pos) { 
      T* d = pos._node;
      if(_size == 0 || d < _data || d >= _data + _size) return false;
      for(; d < _data + _size - 1; ++d) *d = *(d + 1);
      --_size;
      return true;

   }
   bool erase(const T& x) { 
      for(T* i = _data; i < _data + _size; ++i) {
         if(*i == x) {
            erase(iterator(i));
            return true;
         }
      }
      return false;
   }

   void clear() { _size = 0; }

   // This is done. DO NOT change this one.
   void sort() const { if (!empty()) ::sort(_data, _data+_size); }

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }

private:
   T*           _data;
   size_t       _size;       // number of valid elements
   size_t       _capacity;   // max number of elements

   void expand() {
      if(_capacity == 0) _capacity = 1;
      else _capacity *= 2;
      T* temp = _data;
      _data = new T[_capacity];
      for(int i = 0; i < _size; ++i) _data[i] = temp[i];
      delete [] temp;
   }
};

#endif // ARRAY_H
