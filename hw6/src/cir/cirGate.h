/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
public:
   CirGate() : _stat(0) {}
   virtual ~CirGate() {}

   // Basic access methods
   string getTypeStr() const { return type; }
   unsigned getId() const { return _id; }
   unsigned getLineNo() const { return _line; }
   string getSymbol() const { return symbol; }
   bool unused() const { return !(_out.size()); }

   // Depth first search functions
   void dfs(bool in, int level, int cur = 0) const;
   void dfs(int& num) const;
   void dfs(IdList& dfslist) const;

   // Printing functions
   // virtual void printGate() const = 0;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;

   // Setting functions
   void setIn(CirGate*, bool);
   static void resetmark() { ++_gstat; }
   void setSymbol(string& s) { symbol = s; }

private:

protected:
   
   unsigned _id, _line;
   static unsigned _gstat;
   mutable unsigned _stat;
   GateList _out, _in;
   vector<bool> _outInv, _inInv;
   string type, symbol;
   void mark() const { _stat = _gstat; }
   bool marked() const { return _stat == _gstat; }

};

class ConstGate : public CirGate
{
public:
   ConstGate();
   ~ConstGate() {}
};

class UndefGate : public CirGate
{
public:
   UndefGate(unsigned);
   ~UndefGate() {}
   IdList getOutList() const;
};

class PIGate : public CirGate
{
public:
   PIGate(unsigned, unsigned);
   ~PIGate() {}
};

class POGate : public CirGate
{
public:
   POGate(unsigned, unsigned);
   ~POGate() {}
};

class LAGate; // Latch placeholder

class AIGGate : public CirGate
{
public:
   AIGGate(unsigned, unsigned);
   ~AIGGate() {}
};


#endif // CIR_GATE_H
