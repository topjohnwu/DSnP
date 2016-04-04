/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <cmath>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

class FecKey
{
public:
   FecKey(CirGate* n) : value(n->getValue()) {}

   size_t operator() () const {
   	if(~value < value) return ~value;
   	return value;
   }

   bool operator == (const FecKey& k) const { return (*this)() == k(); }

   bool invert(const FecKey& k) const { return value == ~k.value; }

private:
   unsigned value;
};

class SimKey
{
public:
   SimKey(CirGate* n, bool inv) {
      if(inv) value = ~(n->getValue());
      else value = n->getValue();
   }
   size_t operator() () const { return value; }
   bool operator == (const SimKey& k) const { return k.value == value; }
private:
   unsigned value;
};

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
   unsigned max = log(_dfsList.size()) / log(2.5) + 0.5, fail = 0, pre = 0, num = 0;;
   UnsignedList randpattern(_header[1]);
   cout << "MAX_FAILS = " << max << endl;
   while(fail < max) {
      for(unsigned n = 0; n < _header[1]; ++n) randpattern[n] = rand();
      num += 32;
      sim(randpattern);
      if(_fecList.size() == pre) ++fail;
      else fail = 0;
      pre = _fecList.size();
   }
   cout << num << " patterns simulated." << endl;
}

void
CirMgr::fileSim(ifstream& patternFile)
{
	unsigned num = 0;
	UnsignedList patterns(_header[1], 0);
	string line;
	while(!patternFile.eof()) {
      patternFile >> line;
      while(patternFile.peek() == '\n' || patternFile.peek() == ' ') patternFile.get();
		++num;
		if(line.size() != _header[1]) {
			cout << "Error: Pattern(" << line <<") length(" << line.size()
				  << ") does not match the number of inputs(" << _header[1] << ") in a circuit!!" << endl;
		}
		for(unsigned n = 0; n < _header[1]; ++n) 
			patterns[n] = (patterns[n] << 1) + line[n] - '0';
		if(!(num % 32)) {
			sim(patterns);
         simLog();
			UnsignedList temp(_header[1], 0);
			patterns.swap(temp);
		}
	}
	if(num % 32) { sim(patterns); simLog(num % 32); }
	cout << num << " patterns simulated." << endl;
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
void
CirMgr::sim(UnsignedList& patterns) 
{
	// Input pattern
   FORPI(n)
      static_cast<PIGate*>(INFOGATE(n))->setValue(patterns[n]);

   // First FEC
	if(_fecList.empty()) {
      UnsignedList aigDfs;
		aigDfs.push_back(0);
      // First time all gate simulate
		for(unsigned n = 0; n < _dfsList.size(); ++n) {
			if(DFSGATE(n)->getType() == AIG_GATE) {
				static_cast<AIGGate*>(DFSGATE(n))->initValue();
				aigDfs.push_back(_dfsList[n]);
			}
		}
		sort(aigDfs.begin(), aigDfs.end());
      HashMap<FecKey, UnsignedList*> fecHash(getHashSize(aigDfs.size()));
      for(unsigned n = 0; n < aigDfs.size(); ++n) {
         FecKey key(_idGlist[aigDfs[n]]);
         UnsignedList* u;
         // Find new group
         if(!fecHash.check(key, u)) {
            u = new UnsignedList(); 
            _fecList.push_back(u);
            // Update fecList and gate fec info
            _idGlist[aigDfs[n]]->setFec(u, 0);
            u->push_back(aigDfs[n] * 2);
            fecHash.insert(key, u);
         }
         // Insert to existing group
         else {
            // Update fecList and gate fec info
            _idGlist[aigDfs[n]]->setFec(u, u->size());
            u->push_back(aigDfs[n] * 2 + fecHash.checkInv(key));
         }
      }
	}
   else {
      // Simulate (event-driven)
      CirGate::resetmark();
      FORPO(n)
         INFOGATE(n)->simulate();

   	// Find FEC
      vector<UnsignedList*> newList;
   	for(unsigned n = 0, s = _fecList.size(); n < s; ++n) {
         HashMap<SimKey, UnsignedList*> fecHash( getHashSize(_fecList[n]->size() ) );
         for(unsigned m = 0; m < _fecList[n]->size(); ++m) {
            unsigned cur = (*_fecList[n])[m];
            SimKey key(_idGlist[cur / 2], cur % 2);
            UnsignedList* u;
            // Find new group
            if(!fecHash.check(key, u)) {
               u = new UnsignedList(); 
               newList.push_back(u);
               u->push_back(cur);
               fecHash.insert(key, u);
               _idGlist[cur / 2]->setFec(u, 0);
            }
            // Insert to existing group
            else {
               _idGlist[cur / 2]->setFec(u, u->size());
               u->push_back(cur);
            }
         }
   	}
      for(unsigned n = 0; n < _fecList.size(); ++n)
         delete _fecList[n];
      _fecList.swap(newList);
   }
   // Cleanup
   for(int n = _fecList.size() - 1; n >= 0 ; --n) {
      if(_fecList[n]->size() == 1) {
         _idGlist[(*_fecList[n])[0] / 2]->setFec(0, 0);
         delete _fecList[n];
         _fecList.erase(_fecList.begin() + n);
      }
   }
   cout << "Total #FEC Group = " << _fecList.size() << char(13) << flush;
}

void
CirMgr::simLog(unsigned mask)
{
   if(!_simLog) return;
   if(mask == 0) mask = (1 << 31);
   else mask = (1 << (mask - 1));
   FORPI(n)
      *_simLog << (bool)(INFOGATE(n)->_value & mask);
   *_simLog << " ";
   FORPO(n)
      *_simLog << (bool)(INFOGATE(n)->_value & mask);
   *_simLog << endl;
   mask >>= 1;
}

void
CirMgr::updateFec()
{
   for(int n = _fecList.size() - 1; n >= 0 ; --n) {
      for(int m = _fecList[n]->size() - 1; m >= 0; --m) {
         if(!_idGlist[(*_fecList[n])[m] / 2])
            (*_fecList[n]).erase((*_fecList[n]).begin() + m);
      }
      if(_fecList[n]->size() <= 1) {
         if(_fecList[n]->size() == 1)
            _idGlist[(*_fecList[n])[0] / 2]->setFec(0, 0);
         delete _fecList[n];
         _fecList.erase(_fecList.begin() + n);
      }
   }
}
