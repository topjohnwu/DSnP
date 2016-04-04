/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n = 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { 
         _node = _node->_next; 
         return *(this);
      }
      iterator operator ++ (int) {
         iterator temp = *(this);
         _node = _node->_next;
         return temp;
      }
      iterator& operator -- () {
         _node = _node->_prev;
         return *(this);
      }
      iterator operator -- (int) {
         iterator temp = *(this);
         _node = _node->_prev;
         return temp;
      }

      iterator& operator = (const iterator& i) {
         _node = i._node;
         return *(this);
      }

      bool operator != (const iterator& i) const {
         if(_node == i._node) return false;
         else return true;
      }
      bool operator == (const iterator& i) const { return !(*this != i); }

   private:
      DListNode<T>* _node;
   };

   iterator begin() const { return iterator(_head->_next); }
   iterator end() const { return iterator(_head); }
   bool empty() const { return _head == _head->_next; }
   size_t size() const {
      size_t size = 0;
      for (iterator i = begin(); i != end(); ++i, ++size);
      return size;
   }

   void push_back(const T& x) { insert(x, end()); }
   void pop_front() { erase(begin()); }
   void pop_back() { erase(--end()); }

   // return false if nothing to erase
   bool erase(iterator pos) {
      if(pos == end()) return false;
      else {
         pop(pos);
         delete pos._node;
         return true;
      }
   }
   bool erase(const T& x) { 
      for (iterator h = begin(); h != end(); ++h) {
         if(*h == x) {
            erase(h);
            return true;
         }
      }
      return false;
   }
   void clear() {
      if(!empty()) {
         DListNode<T>* d = _head->_next->_next;
         while(1) {
            delete d->_prev;
            if(d == _head) break;
            else d = d->_next;
         }
         _head->_prev = _head->_next = _head;
      }
   }  // delete all nodes except for the dummy node

   void sort() const {
      //insertion_sort();
      quick_sort(begin(), --end());
   }

private:
   DListNode<T>*  _head;  // = dummy node if list is empty

   void insert(const T& x, const iterator pos) const {
      DListNode<T> *p = pos._node, *n = new DListNode<T>(x, p->_prev, p);
      n->_next->_prev = n;
      n->_prev->_next = n;
   }

   void insert(const iterator obj, const iterator pos) const {
      if(obj == pos) return;
      DListNode<T> *i = obj._node, *p = pos._node;
      i->_prev = p->_prev;
      i->_next = p;
      i->_prev->_next = i;
      i->_next->_prev = i;
   }

   void pop(iterator pos) const {
      DListNode<T>* d = pos._node;
      d->_prev->_next = d->_next;
      d->_next->_prev = d->_prev;
   }

   void insertion_sort() const {
      for(iterator i = begin(); i != end(); ) {
         for(iterator j = i; ; --j) {
            if(*j < *i || j == end()) {
               pop(i);
               insert(i++, ++j);
               break;
            }
         }
      }
   }

   void quick_sort(iterator s, iterator e) const {
      if(s == e) return;
      if(s._node->_next == e._node) {
         if(*s < *e) return;
         else {
            T temp = *s;
            *s = *e;
            *e = temp;
            return;
         }
      }
      iterator m = s, p, t = e;
      for(iterator i = e; m != i && m._node->_prev != i._node; ++m, --i);
      if(*s < *m) {
         if(*m < *e) p = m;
         else if(*s < *e) { p = e; --e; }
         else { p = s; ++s; }
      }
      else {
         if(*e < *m) p = m;
         else if(*e < *s) { p = e; --e; }
         else { p = s; ++s; }
      }
      pop(p);
      m = s;
      bool l = false, r = false;
      while(m != t && m._node->_prev != t._node) {
         if(!l) {
            if(*m < *p) ++m;
            else l = true;
         }
         if(!r) {
            if(*t < *p) r = true;
            else --t;
         }
         if(l && r) {
            l = r = false;
            T temp = *m;
            *m = *t;
            *t = temp;
         }
      }
      if(*m < *p) insert(p, ++m);
      else insert(p, m);
      if(p != s._node->_prev) quick_sort(s, iterator(p._node->_prev));
      if(p != e._node->_next) quick_sort(iterator(p._node->_next), e);
   }
};

#endif // DLIST_H
