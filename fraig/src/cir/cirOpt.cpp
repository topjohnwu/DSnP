/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
	if(_unused.size() == 0) return;
	for(unsigned i = 1; i < _idGlist.size(); ++i) {
		if(_idGlist[i]) {
			if(!_idGlist[i]->inDfs()) {
				if(_idGlist[i]->getType() == AIG_GATE || _idGlist[i]->getType() == UNDEF_GATE) {
					cout << "Sweeping: " << _idGlist[i]->getTypeStr()
						  << "(" << _idGlist[i]->_id << ") removed..." << endl;
					replace(_idGlist[i]);
				}
			}
		}
	}
	findUndef();
	findFloating();
	findUnused();
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
	for(unsigned n = 0; n < _dfsList.size(); ++n) {
		CirGate *&cur = _idGlist[_dfsList[n]], *pre = 0;
		bool inv = false;
		if(cur->getType() != AIG_GATE) continue;

		// Fanin const 1
		if((cur->_in[0]->getType() == CONST_GATE && cur->_inInv[0]))
			{ pre = cur->_in[1]; inv = cur->_inInv[1]; }
		else if((cur->_in[1]->getType() == CONST_GATE && cur->_inInv[1]))
			{ pre = cur->_in[0]; inv = cur->_inInv[0]; }
		// Fanin const 0
		else if((cur->_in[0]->getType() == CONST_GATE && !(cur->_inInv[0])) ||
				  (cur->_in[1]->getType() == CONST_GATE && !(cur->_inInv[1])) )
			{ pre = _idGlist[0]; inv = false; }
		// Identical gate
		else if(cur->_in[0] == cur->_in[1]) {
			// Identical
			if(cur->_inInv[0] == cur->_inInv[1])
				{ pre = cur->_in[0]; inv = cur->_inInv[0]; }
			// Inverted
			else { pre = _idGlist[0]; inv = false; }
		}

		if(pre) {
			cout << "Simplifying: " << pre->_id << " merging " << (inv ? "!" : "") << cur->_id << "..." << endl;
			replace(cur, pre, inv);
		}
	}
	findUndef();
	findDfs();
	findFloating();
}

void
CirMgr::replace(CirGate*& a, CirGate* b, bool inv)
{
	// Detach a's inputs
	for(unsigned i = 0; i < a->_in.size(); ++i) {
		for(unsigned j = 0; j < a->_in[i]->_out.size(); ++j) {
			if(a == a->_in[i]->_out[j]) {
				a->_in[i]->_out.erase(a->_in[i]->_out.begin() + j);
				a->_in[i]->_outInv.erase(a->_in[i]->_outInv.begin() + j);
				break;
			}
		}
	}

	// Attach a's output to b
	if(b) {
		for(unsigned n = 0; n < a->_out.size(); ++n) {
			b->_out.push_back(a->_out[n]);
			b->_outInv.push_back(inv != a->_outInv[n]);
		}
	}

	// Reconnect a's output
	for(unsigned i = 0; i < a->_out.size(); ++i) {
		for(unsigned j = 0; j < a->_out[i]->_in.size(); ++j) {
			if(a == a->_out[i]->_in[j]) {
				if(b) {
					a->_out[i]->_in[j] = b;
					a->_out[i]->_inInv[j] = (inv != a->_outInv[i]);
				}
				else {
					a->_out[i]->_in.erase(a->_out[i]->_in.begin() + j);
					a->_out[i]->_inInv.erase(a->_out[i]->_inInv.begin() + j);
				}
				break;
			}
		}
	}

	// Delete a, also remove from _idGlist
	delete a;
	a = 0;
	--_header[4];
}
	
/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
