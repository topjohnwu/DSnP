/****************************************************************************
  FileName     [ cirDef.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic data or var for cir package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_DEF_H
#define CIR_DEF_H

#include <vector>
#include "myHashMap.h"

#define COUT if(DEBUG) cout
#define DEBUG 0

using namespace std;

// TODO: define your own typedef or enum

#define FORPI(n) for(unsigned n = 0; n < _header[1]; ++n)
#define FORPO(n) for(unsigned n = _header[1] + _header[2]; n < _header[1] + _header[2] + _header[3]; ++n)
#define FORIDPO(n) for(unsigned n = _header[0] + 1; n < _header[0] + _header[3] + 1)
#define FORAIG(n) for(unsigned n = _header[1] + _header[2] + _header[3]; n < _header[1] + _header[2] + _header[3] + _header[4]; ++n)
#define INFOGATE(n) _idGlist[info[n][0]]
#define DFSGATE(n) _idGlist[_dfsList[n]]


class CirGate;
class CirMgr;
class SatSolver;

typedef vector<CirGate*>           GateList;
typedef vector<unsigned>			  UnsignedList;

enum GateType
{
   UNDEF_GATE = 0,
   PI_GATE    = 1,
   PO_GATE    = 2,
   AIG_GATE   = 3,
   CONST_GATE = 4,

   TOT_GATE
};

#endif // CIR_DEF_H
