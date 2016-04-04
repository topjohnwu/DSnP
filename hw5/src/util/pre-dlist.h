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

   iterator begin() const { return iterator(_head); }
   iterator end() const { return iterator(_head->_prev); }
   bool empty() const { return _head == _head->_prev; }
   size_t size() const {
      size_t size = 0;
      for (DListNode<T>* h = _head; h != _head->_prev; h = h->_next) ++size;
      return size;
   }

   void push_back(const T& x) { insert(x, _head->_prev); }
   void pop_front() { pop(_head); }
   void pop_back() { pop(_head->_prev->_prev); }

   bool erase(iterator pos) { return pop(pos._node); }
   bool erase(const T& x) { 
      for (DListNode<T>* h = _head; h != _head->_prev; h = h->_next) {
         if(h->_data == x) {
            pop(h);
            return true;
         }
      }
      return false;
   }

   void clear() { for(int i = 0, s = size(); i < s; ++i) pop_back(); }

   // Insertion Sort O(n^2)
   /*void sort() const {
      for(DListNode<T>* i = _head->_next; i != _head->_prev; ) {
         for(DListNode<T>* j = i->_prev; j != _head->_prev->_prev; j = j->_prev) {
            if((j->_data) < (i->_data) || j == _head->_prev) {
               insert(i->_data, j->_next);
               pop(i);
               break;
            }
         }
      }
   }*/

   void sort() const { quick_sort(_head, _head->_prev->_prev); }

   

private:
   DListNode<T>*  _head;  // = dummy node if list is empty

   void insert(const T& x, DListNode<T>* i, bool reserve = false) const {
      DListNode<T>* n;
      if(i == _head || reserve) {
         n = new DListNode<T>(i->_data, i, i->_next);
         i->_data = x;
      }
      else n = new DListNode<T>(x, i->_prev, i);
      n->_next->_prev = n;
      n->_prev->_next = n;
   }

   bool pop(DListNode<T>* d, int mode = 0) const {
      if(d == _head->_prev) return false;
      if(mode == 2) {
         d->_data = d->_prev->_data;
         d = d->_prev;
      }
      else if(mode == 1) {
         d->_data = d->_next->_data;
         d = d->_next;
      }
      d->_prev->_next = d->_next;
      d->_next->_prev = d->_prev;
      delete d;
      return true;
   }

   void printout(iterator h = iterator(), iterator t = iterator(), iterator p = iterator()) const {
      int len = 6;
      iterator li = begin();
      for (; li != end(); ++li) {
         cout << *li << " ";
      }
      cout << endl;
      if(1) {
      for(li = begin(); li != end(); ++li) {
         if(li == p) cout << setw(len) << "*";
         else if(li == h || li == t) cout << setw(len) << "^";
         else cout << setw(len) << " ";
         cout << " ";
      }
      cout << endl;
      }
   }

   void quick_sort(DListNode<T>* s, DListNode<T>* e) const {
      if(s == _head->_prev || e == _head->_prev) {
         cout << "Error!!" << endl;
         return;
      }
      if(s == e) return;
      if(s->_next == e) {
         if(s->_data < e->_data) return;
         else {
            T temp = s->_data;
            s->_data = e->_data;
            e->_data = temp;
            return;
         }
      }
      bool l = false, r = false;
      DListNode<T> *h = s, *t = e, *p = s;
      for(; h != t && h->_prev != t; h = h->_next, t = t->_prev);
      if(s->_data < h->_data) {
         if(h->_data < e->_data) p = h;
         else if(s->_data < e->_data) p = e;
         else p = s;
      }
      else {
         if(e->_data < h->_data) p = h;
         else if(e->_data < s->_data) p = e;
         else p = s;
      }
      T pivot = p->_data;
      printout(iterator(s), iterator(e), iterator(p));
      if(p == s) pop(p, 1);
      else if(p == e) pop(p, 2);
      else pop(p);
      h = s, t = e;
      //DListNode<T> *h = s->_next, *t = e;
      while(h != t && h->_prev != t) {
         if(!l) {
            if(h->_data < pivot) h = h->_next;
            else l = true;
         }
         if(!r) {
            if(t->_data < pivot) r = true;
            else t = t->_prev;
         }
         if(l && r) {
            l = r = false;
            T temp = h->_data;
            h->_data = t->_data;
            t->_data = temp;
         }
      }
      /*DListNode<T>* ss = s->_next;
      if(h->_data < s->_data) move(s, h->_next);
      else move(s, h);*/
      if(h->_data < pivot) { insert(pivot, h->_next); p = h->_next;}
      else { insert(pivot, h); p = h->_prev;}
      //if(p == s->_prev) p = s;
      //if(p == e->_next) e = p;
      //pop(s, true);
      printout(iterator(s), iterator(e), iterator(p));
      //fgetc(stdin); 
      //if(h->_prev == e) {cout << "Largest!!" << endl;}
      //if(e->_next != s) quick_sort(s->_next, e);
      //if(ss->_prev != s) quick_sort(ss, s->_prev);
      //if(h->_prev != e) quick_sort(h->_next, e);
      if(p != s) quick_sort(s, p->_prev);
      if(p != e) quick_sort(p->_next, e);
      /*quick_sort(s, h);
      if(h->_prev == e) return;
      else quick_sort(h->_next, e);*/
   }
};

#endif // DLIST_H
