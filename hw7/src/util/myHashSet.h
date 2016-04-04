/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ()" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // Implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
      //iterator() {}
      iterator(bool begin, vector<Data>* b, size_t n)
               : _numBuckets(n), _index(0), _pos(0), _buckets(b) {
         if(begin) {
            for(; _index < _numBuckets; ++_index) {
               if(_buckets[_index].size()) {
                  _node = &_buckets[_index][_pos];
                  break;
               }
            }
         }
         else _index = _numBuckets;
      }
      ~iterator() {}
      const Data& operator * () const { return *_node; }
      Data& operator * () { return *_node; }
      iterator& operator ++ () {
         if(_index == _numBuckets) return (*this);
         else if(_pos < _buckets[_index].size() - 1) ++_pos;
         else {
            while(++_index < _numBuckets) {
               if(_buckets[_index].size()) {
                  break;
               }
            }
            _pos = 0;
         }
         if(_index != _numBuckets) _node = &_buckets[_index][_pos];
         return (*this);
      }
      iterator operator ++ (int) {
         iterator ret = (*this);
         ++(*this);
         return ret;
      }
      iterator& operator -- () {
         if(_pos > 0) --_pos;
         else {
            size_t n = _index;
            while(n > 0) {
               --n;
               if(_buckets[n].size()) {
                  _pos = _buckets[n].size() - 1;
                  _index = n;
                  break;
               }
            }
         }
         _node = &_buckets[_index][_pos];
         return (*this);
      }
      iterator operator -- (int) {
         iterator ret = (*this);
         --(*this);
         return ret;
      }
      iterator& operator = (const iterator& i) {
         _node = i._node;
         _numBuckets = i._numBuckets;
         _index = i._index;
         _pos = i._pos;
         _buckets = i._buckets;
      }
      bool operator == (const iterator& i) const {
         return ((i._pos == _pos) && (i._index == _index));
      }
      bool operator != (const iterator& i) const {
         return !(i == (*this));
      }
   private:
      Data*             _node;
      size_t            _numBuckets, _index, _pos;
      vector<Data>*     _buckets;
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // Implement these functions
   //
   // Point to the first valid data
   iterator begin() const { return iterator(true, _buckets, _numBuckets); }
   // Pass the end
   iterator end() const { return iterator(false, _buckets, _numBuckets); }
   // return true if no valid data
   bool empty() const { return (begin() == end()); }
   // number of valid data
   size_t size() const {
      size_t s = 0;
      iterator b = begin(), e = end();
      for(; b != e; ++b, ++s);
      return s;
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const {
      size_t num = bucketNum(d), pos;
      for(pos = 0; pos < _buckets[num].size(); ++pos)
         if(_buckets[num][pos] == d) return true;
      return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const {
      size_t num = bucketNum(d), pos;
      for(pos = 0; pos < _buckets[num].size(); ++pos)
         if(_buckets[num][pos] == d) break;
      if(pos == _buckets[num].size()) return false;
      else {
         d = _buckets[num][pos];
         return true;
      }
   }

   // update the entry in hash that is equal to d
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) {
      size_t num = bucketNum(d), pos;
      for(pos = 0; pos < _buckets[num].size(); ++pos)
         if(_buckets[num][pos] == d) break;
      if(pos == _buckets[num].size()) {
         _buckets[num].push_back(d);
         return false;
      }
      else {
         _buckets[num][pos] = d;
         return true;
      }
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) {
      size_t num = bucketNum(d), pos;
      for(pos = 0; pos < _buckets[num].size(); ++pos)
         if(_buckets[num][pos] == d) break;
      if(pos == _buckets[num].size()) {
         _buckets[num].push_back(d);
         return true;
      }
      else return false;
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) {
      size_t num = bucketNum(d), pos;
      for(pos = 0; pos < _buckets[num].size(); ++pos)
         if(_buckets[num][pos] == d) break;
      if(pos == _buckets[num].size()) return false;
      else {
         _buckets[num].erase(_buckets[num].begin() + pos);
         return true;
      }
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
