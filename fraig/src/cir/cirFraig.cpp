/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
HashKey
getKey(CirGate* cur)
{
	unsigned a = 2 * cur->_in[0]->_id + cur->_inInv[0], b = 2 * cur->_in[1]->_id + cur->_inInv[1];
	if(a < b) return HashKey(a, b);
	else return HashKey(b, a);
}

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
	HashMap<HashKey, CirGate*> hash(getHashSize(_header[0]));
	for(unsigned n = 0; n < _dfsList.size(); ++n) {
		CirGate*& cur = _idGlist[_dfsList[n]];
		if(cur->getType() == AIG_GATE) {
			HashKey k = getKey(cur);
			if(!hash.insert(k, cur)) {
				CirGate* pre;
				hash.check(k, pre);
				cout << "Strashing: " << pre->_id << " merging " << cur->_id << "..." << endl;
				replace(cur, pre);
			}
		}
	}
	findFloating();
	findDfs();
}

void
CirMgr::fraig()
{  
   vector<UnsignedList> mergelist;
   SatSolver solver;
   while(_fecList.size()) {
      mergelist.clear();
      solver.reset();
      solver.initialize();

   	// Set var
      for(unsigned n = 0; n < _dfsList.size(); ++n)
         if(DFSGATE(n))
            DFSGATE(n)->setVar(solver.newVar());
      // Construct proof model
   	for(unsigned n = 0; n < _dfsList.size(); ++n) {
   		if(DFSGATE(n)) {
   			if(DFSGATE(n)->getType() == AIG_GATE) {
   				solver.addAigCNF
   				(DFSGATE(n)->getVar(),
   				 DFSGATE(n)->_in[0]->getVar(), DFSGATE(n)->_inInv[0],
   				 DFSGATE(n)->_in[1]->getVar(), DFSGATE(n)->_inInv[1] );
   			}
   		}
   	}

   	// Prove all pairs
   	UnsignedList patterns(_header[1], 0);
      unsigned num = 0;
      CirGate::resetfraig();
      for(unsigned n = 0; n < _dfsList.size(); ++n) {
         if(DFSGATE(n)->getType() == AIG_GATE) {
            // In an FEC group
            if(DFSGATE(n)->_fec) {
               // Unfraiged only
               if(!DFSGATE(n)->fraiged()) {
                  DFSGATE(n)->fraig();
                  unsigned self = DFSGATE(n)->_fecpos;
                  UnsignedList& list = *DFSGATE(n)->_fec;
                  // Const mode
                  if(list[0] == 0) {
                     // UnSAT
                     if(!prove(0, list[self], solver)) {
                        mergelist.push_back(UnsignedList());
                        mergelist.back().push_back(0);
                        mergelist.back().push_back(list[self]);
                     }
                     // SAT
                     else {
                        FORPI(x)
                           patterns[x] = (patterns[x] << 1) + solver.getValue(INFOGATE(x)->getVar());
                        ++num;
                        // Update by SAT results
                        if(!(num % 32)) {
                           sim(patterns);
                           cout << "Updating by SAT... Total #FEC Group = " << _fecList.size() << endl;
                           UnsignedList temp(_header[1], 0);
                           patterns.swap(temp);
                        }
                        // break;
                     }
                  }
                  // Normal mode
                  else {
                     for(unsigned m = 0; m < DFSGATE(n)->_fec->size(); ++m) {
                        if(m != self) {
                           if(!_idGlist[list[m] / 2]->fraiged()) {
                              // UnSAT
                              if(!prove(list[self], list[m], solver)) {
                                 mergelist.push_back(UnsignedList());
                                 mergelist.back().push_back(list[self]);
                                 mergelist.back().push_back(list[m]);
                              }
                              // SAT
                              else {
                                 FORPI(x)
                                    patterns[x] = (patterns[x] << 1) + solver.getValue(INFOGATE(x)->getVar());
                                 ++num;
                                 // Update by SAT results
                                 if(!(num % 32)) {
                                    sim(patterns);
                                    cout << "Updating by SAT... Total #FEC Group = " << _fecList.size() << endl;
                                    UnsignedList temp(_header[1], 0);
                                    patterns.swap(temp);
                                    break;
                                 }
                                 //break;
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }

   	// Merge pairs
      for(unsigned n = 0; n < mergelist.size(); ++n) {
         cout << "Fraig: " << mergelist[n][0] / 2 << " merging "
         << (((mergelist[n][0] % 2) != (mergelist[n][1] % 2)) ? "!" : "") 
         << mergelist[n][1] / 2 << "..." << endl;
         replace(_idGlist[mergelist[n][1] / 2],
                 _idGlist[mergelist[n][0] / 2],
                 (mergelist[n][0] % 2) != (mergelist[n][1] % 2));
      }

      updateFec();
      findDfs();
      cout << "Updating by UNSAT... Total #FEC Group = " << _fecList.size() << endl;

      if(num % 32) {
         sim(patterns);
         cout << "Updating by SAT... Total #FEC Group = " << _fecList.size() << endl;
         UnsignedList temp(_header[1], 0);
         patterns.swap(temp);
      }
   }
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/

bool
CirMgr::prove(unsigned a, unsigned b, SatSolver& solver) 
{
   bool result;
   if(a) {
      Var v = solver.newVar();
      solver.addXorCNF(v, _idGlist[a / 2]->getVar(), a % 2, _idGlist[b / 2]->getVar(), b % 2);
      solver.assumeRelease();
      solver.assumeProperty(v, true);
      result = solver.assumpSolve();
      cout << char(13) <<flush
         << "Proving (" << (a % 2 ? "!" : "") << a / 2 << ", " 
         << (b % 2 ? "!" : "") << b / 2
         << ")..." << (result ? "SAT" : "UNSAT") << "!!" << char(13) <<flush;
   }
   else {
      solver.assumeRelease();
      solver.assumeProperty(_idGlist[b / 2]->getVar(), !(b % 2));
      result = solver.assumpSolve();
      cout << char(13) <<flush
         << "Proving " << b / 2 << " = " << !(b % 2) << "..." 
         << (result ? "SAT" : "UNSAT") << "!!" << char(13) <<flush;
   }
   if(!result) _idGlist[b / 2]->fraig();
   return result;
}
