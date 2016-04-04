/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"

extern CirMgr *cirMgr;

class CirMgr
{
public:
   CirMgr();
   ~CirMgr();

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const;

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;

private:
   ofstream                *_simLog;
   UnsignedList            _header, _unused, _floating, _undefList, _dfsList;
   GateList                _idGlist;
   vector<UnsignedList>    info;
   vector<UnsignedList*>   _fecList;

   // Link/Relink functions
   void linkPO(unsigned);
   void linkAIG(unsigned);
   void replace(CirGate*& a, CirGate* b = 0, bool inv = false);
   
   // Sim/fraig functions
   void sim(UnsignedList&);
   void simLog(unsigned = 0);
   bool prove(unsigned, unsigned, SatSolver&);

   // Maintaining functions
   void findUnused();
   void findFloating();
   void findDfs();
   void findUndef();
   void updateFec();

   // Debugging
   void test();

};

#endif // CIR_MGR_H
