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

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;
unsigned CirGate::_gstat = 1;
unsigned CirGate::_gfraigstat = 1;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::printGate() const
{
	cout << setw(4) << left << getTypeStr() << _id;
   for(int n = 0; n < _in.size(); ++n) {
      cout << " ";
      if(_in[n]->getType() == UNDEF_GATE) cout << "*";
      if(_inInv[n]) cout << "!";
      cout << _in[n]->_id;
   }
   if(_symbol.size()) cout << " (" << _symbol << ")";
   cout << endl;
}

void
CirGate::reportGate() const
{
	stringstream ss;
   string s;
   cout << "==================================================" << endl;
   ss << "= " << getTypeStr() << "(" << _id << ")";
   if(_symbol.size()) ss << "\"" << _symbol << "\"";
   ss << ", line " << _line;
   s = ss.str();
   s.resize(49, ' ');
   s += "=";
   cout << s << endl;
   ss.str("");
   ss << "= FECs:";
   if(_fec) {
	   for(unsigned n = 0; n < _fec->size(); ++n)
	   	if(((*_fec)[n] / 2) != _id)
	   		ss << (((*_fec)[n] % 2) ? " !" : " " ) << ((*_fec)[n] / 2);
   }
   s = ss.str();
   if(s.size() < 49) s.resize(49, ' ');
   s += "=";
   cout << s << endl << "= Value: ";
   unsigned mask = 1;
   for(unsigned n = 0; n < 32; ++n) {
   	if(!(n % 4) && n) cout << "_";
   	cout << (bool)(_value & mask);
   	mask <<= 1;
   }
   cout << " =" << endl << "==================================================" << endl;
}

void
CirGate::dfs(UnsignedList& dfs) const
{
	//printGate();
	for(int n = 0; n < _in.size(); ++n)
      if(!_in[n]->marked()) { _in[n]->dfs(dfs); }
   mark(true);
   dfs.push_back(_id);
}

void
CirGate::reportFanin(int level, int cur) const
{
   assert (level >= 0);
   if(cur > level) return;
   if(cur == 0) {
   	resetmark();
      cout << getTypeStr() << " " << _id << endl;
      ++cur;
   }
   if(_in.size()) mark();
   for(int n = 0; n < _in.size(); ++n) {
      for(int m = 0; m < cur; ++m) cout << "  ";
      if(_inInv[n]) cout << "!";
      cout << _in[n]->getTypeStr() << " " << _in[n]->_id;
      if(_in[n]->marked()) { cout << " (*)" << endl; }
      else {
         cout << endl;
         _in[n]->reportFanin(level, cur + 1);
      }
   }
}

void
CirGate::reportFanout(int level, int cur) const
{
   assert (level >= 0);
   if(cur > level) return;
   if(cur == 0) {
   	resetmark();
      cout << getTypeStr() << " " << _id << endl;
      ++cur;
   }
   if(_out.size()) mark();
   for(int n = 0; n < _out.size(); ++n) {
      for(int m = 0; m < cur; ++m) cout << "  ";
      if(_outInv[n]) cout << "!";
      cout << _out[n]->getTypeStr() << " " << _out[n]->_id;
      if(_out[n]->marked()) { cout << " (*)" << endl; }
      else {
         cout << endl;
         _out[n]->reportFanout(level, cur + 1);
      }
   }
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

/**************************************/
/*  Derived CirGate member functions  */
/**************************************/

UnsignedList
UndefGate::getOutList() const 
{
   UnsignedList outList;
   for(int i = 0; i < _out.size(); ++i)
      outList.push_back(_out[i]->getId());
   return outList;
}

bool
POGate::simulate()
{
	if((_sim = _in[0]->simulate()))
		_value = _in[0]->getValue() ^ (0 - _inInv[0]);
	return _sim;
}

bool
AIGGate::simulate()
{
	if(marked()) return _sim;
	mark();
	if( (_sim = _in[0]->simulate() + _in[1]->simulate()) ) {
		unsigned newvalue;
		newvalue = (_in[0]->getValue() ^ (0 - _inInv[0]) ) & (_in[1]->getValue() ^ (0 - _inInv[1]) );
		if(_value == newvalue) _sim = false;
		_value = newvalue;
	}
	return _sim;
}
