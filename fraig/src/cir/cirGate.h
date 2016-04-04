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
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate
{
   friend class CirMgr;
   friend HashKey getKey(CirGate* cur);
public:
   CirGate() : _value(0), _stat(0), _fraigstat(0), _dfs(0), _sim(0), _fec(0) {}
   virtual ~CirGate() {}

   // Basic access methods
   virtual string getTypeStr() const = 0;
   virtual GateType getType() const = 0;
   unsigned getId() const { return _id; }
   unsigned getLineNo() const { return _line; }
   string getSymbol() const { return _symbol; }
   bool unused() const { return !(_out.size()); }
   unsigned getValue() const { return _value; }
   Var getVar() const { return _var; }

   // Special functions
   void dfs(UnsignedList&) const;
   virtual bool simulate() { return _sim; };

   // Printing functions
   void printGate() const;
   void reportGate() const;
   void reportFanin(int level, int cur = 0) const;
   void reportFanout(int level, int cur = 0) const;

   // Setting functions
   void setIn(CirGate*, bool);
   static void resetmark() { ++_gstat; }
   static void resetfraig() { ++_gfraigstat; }
   void setSymbol(const string& s) { _symbol = s; }
   void setVar(const Var& v) { _var = v; }
   void setFec(UnsignedList* l, unsigned p) { _fec = l; _fecpos = p; }

   // Checking functions
   bool inDfs() { return _dfs; }
   virtual bool isAig() const { return false; }

   // Marking
   void mark(bool dfs = false) const { _stat = _gstat; _dfs += dfs; }
   bool marked() const { return _stat == _gstat; }
   void fraig() { _fraigstat = _gfraigstat; }
   bool fraiged() { return _fraigstat == _gfraigstat; }
   
private:

protected:
   unsigned _id, _line, _value, _fecpos;
   static unsigned _gstat, _gfraigstat;
   mutable unsigned _stat, _fraigstat;
   GateList _out, _in;
   vector<bool> _outInv, _inInv;
   mutable bool _dfs, _sim;
   UnsignedList* _fec;
   string _symbol;
   Var _var;
};


class ConstGate : public CirGate
{
public:
   ConstGate() { _id = 0; _line = 0; _sim = true; }
   ~ConstGate() {}
   virtual string getTypeStr() const { return "CONST"; }
   virtual GateType getType() const { return CONST_GATE; }
};

class UndefGate : public CirGate
{
public:
   UndefGate(unsigned i) { _id = i; _line = 0; }
   ~UndefGate() {}
   virtual string getTypeStr() const { return "UNDEF"; }
   virtual GateType getType() const { return UNDEF_GATE; }
   UnsignedList getOutList() const;
};

class PIGate : public CirGate
{
public:
   PIGate(unsigned i, unsigned l) { _id  = i; _line  = l; };
   ~PIGate() {}
   virtual string getTypeStr() const { return "PI"; }
   virtual GateType getType() const { return PI_GATE; }
   void setValue(unsigned n) { _value = n; _sim = true; mark(); }
};

class POGate : public CirGate
{
public:
   POGate(unsigned i, unsigned l) { _id = i; _line = l; }
   ~POGate() {}
   virtual string getTypeStr() const { return "PO"; }
   virtual GateType getType() const { return PO_GATE; }
   virtual bool simulate();
};

class LAGate; // Latch placeholder

class AIGGate : public CirGate
{
public:
   AIGGate(unsigned i, unsigned l) { _id = i; _line = l; }
   ~AIGGate() {}
   virtual string getTypeStr() const { return "AIG"; }
   virtual GateType getType() const { return AIG_GATE; }
   virtual bool isAig() const { return true; }
   virtual bool simulate();
   void initValue() 
   { _value = (_in[0]->getValue() ^ (0 - _inInv[0]) ) & (_in[1]->getValue() ^ (0 - _inInv[1]) ); }
};

#endif // CIR_GATE_H
