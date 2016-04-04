/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
unsigned CirGate::_gstat = 1;

void
CirGate::reportGate() const
{
   stringstream ss;
   string s;
   cout << "==================================================" << endl;
   ss << "= " << type << "(" << _id << ")";
   if(symbol.size()) ss << "\"" << symbol << "\"";
   ss << ", line " << _line;
   s = ss.str();
   s.resize(49, ' ');
   s += "=";
   cout << s << endl << "==================================================" << endl;
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   resetmark();
   dfs(true, level);
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   resetmark();
   dfs(false, level);
}

void
CirGate::setIn(CirGate* input, bool inv) 
{
   assert(input != 0);
   _in.push_back(input);
   _inInv.push_back(inv);
   input->_out.push_back(this);
   input->_outInv.push_back(inv);
}

void
CirGate::dfs(bool in, int level, int cur) const 
{
   if(level != -1) if(cur > level) return;
   const GateList* list;
   const vector<bool>* inv;
   if(in) {
      list = &_in;
      inv = &_inInv;
   }
   else {
      list = &_out;
      inv = &_outInv;
   }
   if((*list).size()) mark();
   if(cur == 0) {
      cout << type << " " << _id << endl;
      mark();
      ++cur;
   }
   for(int n = 0; n < (*list).size(); ++n) {
      for(int m = 0; m < cur; ++m) cout << "  ";
      if((*inv)[n]) cout << "!";
      cout << (*list)[n]->type << " " << (*list)[n]->_id;
      if((*list)[n]->marked()) { cout << " (*)" << endl; }
      else {
         cout << endl;
         (*list)[n]->dfs(in, level, cur + 1);
      }
   }
}

void
CirGate::dfs(int& num) const 
{
   const UndefGate* undef = dynamic_cast<const UndefGate*>(this);
   if(undef) return;
   for(int n = 0; n < _in.size(); ++n)
      if(!_in[n]->marked()) { _in[n]->dfs(num); }
   if(num == 0) cout << endl;
   cout << "[" << num << "] " << setw(4) << left << type << _id;
   ++num;
   mark();
   for(int n = 0; n < _in.size(); ++n) {
      cout << " ";
      UndefGate* g = dynamic_cast<UndefGate*>(_in[n]);
      if(g) cout << "*";
      if(_inInv[n]) cout << "!";
      cout << _in[n]->_id;     
   }
   if(symbol.size()) cout << " (" << symbol << ")";
   cout << endl;
}

void
CirGate::dfs(IdList& dfslist) const
{
   for(int n = 0; n < _in.size(); ++n)
      if(!_in[n]->marked()) { _in[n]->dfs(dfslist); }
   mark();
   const AIGGate* aig = dynamic_cast<const AIGGate*>(this);
   if(aig) dfslist.push_back(aig->_line - 2);
}

/**************************************/
/*  Derived CirGate member functions  */
/**************************************/

ConstGate::ConstGate() 
{
   _id = 0;
   _line = 0;
   type = "CONST";
}

UndefGate::UndefGate(unsigned i) 
{
   _id = i;
   _line = 0;
   type = "UNDEF";
}

IdList
UndefGate::getOutList() const 
{
   IdList outList;
   for(int i = 0; i < _out.size(); ++i)
      outList.push_back(_out[i]->getId());
   return outList;
}

PIGate::PIGate(unsigned i, unsigned l) 
{
   _id = i;
   _line = l;
   type = "PI";
}

POGate::POGate(unsigned i, unsigned l) 
{
   _id = i;
   _line = l;
   type = "PO";
}

AIGGate::AIGGate(unsigned i, unsigned l) 
{
   _id = i;
   _line = l;
   type = "AIG";
}