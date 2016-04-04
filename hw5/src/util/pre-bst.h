/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

#define REPLACE(A,B) if(B) B->_parent = A->_parent;\
  							if(A->_parent)\
  							(A->_parent->_right == A) ? (A->_parent->_right = B) : (A->_parent->_left = B);\
  							else _root = B

#define TOMAX(A)     while(A->_right) A = A->_right
#define TOMIN(A)     while(A->_left) A = A->_left

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
	friend class BSTree<T>;
   friend class BSTree<T>::iterator;

   BSTreeNode(const T& d, BSTreeNode<T>* p = 0, BSTreeNode<T>* r = 0, BSTreeNode<T>* l = 0):
   	_data(d), _parent(p), _right(r), _left(l) {}

   T 					_data;
   BSTreeNode<T>*	_parent;
   BSTreeNode<T>*	_right;
   BSTreeNode<T>*	_left;
};


template <class T>
class BSTree
{
public:
	BSTree() : _size(0) {
		_dummy = new BSTreeNode<T>(T());
		_root = _dummy;
	}
	~BSTree() {}
   class iterator {
   	friend class BSTree;
   public:
   	iterator(BSTreeNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {}

      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { 
         if(_node->_right) {
            _node = _node->_right;
            TOMIN(_node);
         }
         else {
            while(1) {
               if(_node->_parent == 0) break;
               if(_node ==_node->_parent->_left) break;
               _node = _node->_parent;
            }
            _node = _node->_parent;
         }
      	return *(this);
      }
      iterator operator ++ (int) { 
      	iterator temp = *(this);
      	++(*this);
      	return temp;
      }
      iterator& operator -- () {
      	if(_node->_left) {
            _node = _node->_left;
            TOMAX(_node);
         }
         else {
            while(1) {
               if(_node->_parent == 0) break;
               if(_node == _node->_parent->_right) break;
               _node = _node->_parent;
            }
            _node = _node->_parent;
         }
         return *(this);
   	}
      iterator operator -- (int) { 
      	iterator temp = (*this);
      	--(*this);
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
      BSTreeNode<T>* _node;
   };
   iterator begin() {
   	BSTreeNode<T>* temp = _root;
      TOMIN(temp);
   	return iterator(temp);
   }
   iterator end() { return iterator(_dummy); }
   bool empty() { return !_size; }
   size_t size() { return _size; }
   void insert(const T& x) {
   	BSTreeNode<T>* temp = _root;
   	while(1) {
   		if(temp == _dummy) {
   			BSTreeNode<T>* n = new BSTreeNode<T>(x, _dummy->_parent, _dummy);
   			if(temp == _root) _root = n;
   			else _dummy->_parent->_right = n;
   			_dummy->_parent = n;
   			break;
   		}
   		else if(x < temp->_data) {
   			if(temp->_left) temp = temp->_left;
   			else {
   				BSTreeNode<T>* n = new BSTreeNode<T>(x, temp);
   				temp->_left = n;
   				break;
   			}
   		}
   		else {
   			if(temp->_right) temp = temp->_right;
   			else {
   				BSTreeNode<T>* n = new BSTreeNode<T>(x, temp);
   				temp->_right = n;
   				break;
   			}
   		}
   	}
   	++_size;
   }
   void pop_front() { erase(begin()); }
   void pop_back() { erase(--end()); }
   bool erase(iterator pos) {
   	if(pos == end()) return false;
   	BSTreeNode<T>* d = pos._node;
   	if(d->_right && d->_right != _dummy) {
   		if(d->_left) {
   			T s = (++pos)._node->_data;
   			erase(pos);
   			d->_data = s;
   			++_size;
   		}
   		else {
   			REPLACE(d, d->_right);
   			delete d;
   		}
   	}
   	else if(d->_left) {
   		REPLACE(d, d->_left);
   		if(d->_right == _dummy) {
   			(--pos)._node->_right = _dummy;
   			_dummy->_parent = pos._node;
   		}
   		delete d;
   	}
   	else {
   		REPLACE(d, d->_left);
   		if(_root == 0) _root = _dummy;
   		delete d;
   	}
   	--_size;
   	return true;
   }
   bool erase(const T& x) {
   	iterator h = begin(), t = end();
      for (; h != t; ++h) {
         if(h._node->_data == x) {
            erase(h);
            return true;
         }
      }
      return false;
   }
   void clear() {
   	iterator h = begin(), t = end(), i = h;
      while(1) {
         if(i == t) break;
         ++i;
         erase(h);
         h = i;
      }
   }
   void sort() {}
   void print() {}
private:
	BSTreeNode<T>* _root;
	BSTreeNode<T>* _dummy;  //Points to dummy
	size_t _size;
};

#endif // BST_H
