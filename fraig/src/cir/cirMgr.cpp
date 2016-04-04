/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
/*static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;*/

/*static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}*/

/******************************/
/*   Constructor/Destructor   */
/******************************/

CirMgr::CirMgr()
{
   _header.resize(5);
   _idGlist.push_back(new ConstGate());
   srand(time(0));
}

CirMgr::~CirMgr()
{
   for(unsigned n = 0 ; n < _idGlist.size(); ++n)
      if(_idGlist[n])
         delete _idGlist[n];
}

/************************/
/*   Helper Functions   */
/************************/

CirGate*
CirMgr::getGate(unsigned gid) const 
{
   if(gid >= _idGlist.size()) return 0;
   return _idGlist[gid];
}

void
CirMgr::linkPO(unsigned n) 
{
   CirGate* input = _idGlist[ info[n][1] / 2 ];
   if(input == 0) {
      input = new UndefGate(info[n][1] / 2);
      _undefList.push_back(info[n][1] / 2);
      _idGlist[info[n][1] / 2] = input;
   }
   _idGlist[info[n][0]]->setIn(input, info[n][1] % 2);
}

void
CirMgr::linkAIG(unsigned n) 
{
   CirGate* in1 = _idGlist[ info[n][1] / 2 ];
   CirGate* in2 = _idGlist[ info[n][2] / 2 ];
   if(in1 == 0) {
      in1 = new UndefGate(info[n][1] / 2);
      _undefList.push_back(info[n][1] / 2);
      _idGlist[info[n][1] / 2] = in1;
   }
   if(in2 == 0) {
      in2 = new UndefGate(info[n][2] / 2);
      _undefList.push_back(info[n][2] / 2);
      _idGlist[info[n][2] / 2] = in2;
   }
   _idGlist[info[n][0]]->setIn(in1, info[n][1] % 2);
   _idGlist[info[n][0]]->setIn(in2, info[n][2] % 2);
}

void
CirMgr::findUnused()
{
   _unused.clear();
   for(unsigned n = 1; n <= _header[0]; ++n)
      if(_idGlist[n])
         if(_idGlist[n]->unused()) _unused.push_back(_idGlist[n]->_id);
}

void
CirMgr::findFloating()
{
   _floating.clear();
   for(unsigned n = 0; n < _undefList.size(); ++n) {
      UnsignedList v = static_cast<UndefGate*>(_idGlist[_undefList[n]])->getOutList();
      _floating.insert(_floating.end(), v.begin(), v.end());
   }
   sort(_floating.begin(), _floating.end());
}

void
CirMgr::findDfs()
{
   CirGate::resetmark();
   _dfsList.clear();
   FORPO(n)
      INFOGATE(n)->dfs(_dfsList);
}

void
CirMgr::findUndef()
{
   UnsignedList newundef;
   for(unsigned n = 0; n < _undefList.size(); ++n) {
      if(_idGlist[_undefList[n]]) {
         if(_idGlist[_undefList[n]]->unused()) {
            delete _idGlist[_undefList[n]];
            _idGlist[_undefList[n]] = 0;
         }
      }
   }
   for(unsigned n = 0; n < _undefList.size(); ++n)
      if(_idGlist[_undefList[n]]) newundef.push_back(_undefList[n]);
   _undefList.swap(newundef);
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
   ifstream file(fileName.c_str(), ios::in);
   if(!file.is_open()) {
      cerr << "Cannot open design \"" << fileName << "\"!!" << endl;
      return false;
   }
   string line, head;

   file >> head;
   if(head == "aag")
      for(int i = 0; i < 5; ++i)
         file >> _header[i];
   else { return false; }
   ++lineNo;

   // Read in info

   for(unsigned n = 0; n < _header[1]; ++n, ++lineNo) {
      unsigned x;
      file >> x;
      vector<unsigned> v;
      v.push_back(x / 2);
      info.push_back(v);
   }
   
   for(unsigned n = 0; n < _header[2]; ++n, ++lineNo) { } // Latch placeholder

   for(unsigned n = 0; n < _header[3]; ++n, ++lineNo) {
      unsigned x;
      file >> x;
      UnsignedList v;
      v.push_back(_header[0] + n + 1); v.push_back(x);
      info.push_back(v);
   }

   for(unsigned n = 0; n < _header[4]; ++n, ++lineNo) {
      unsigned x, y, z;
      file >> x >> y >> z;
      UnsignedList v;
      v.push_back(x / 2); v.push_back(y); v.push_back(z);
      info.push_back(v);
   }

   // Construct Gates

   _idGlist.resize(_header[0] + _header[3] + 1, 0);

   // PIGate
   FORPI(n)
      _idGlist[info[n][0]] = new PIGate(info[n][0], n + 2);

   // POGate
   FORPO(n)
      _idGlist[info[n][0]] = new POGate(info[n][0], n + 2);

   // AIGGate
   FORAIG(n)
      _idGlist[info[n][0]] = new AIGGate(info[n][0], n + 2);

   // LinkAIGs
   for(unsigned n = 1; n < _header[0] + 1; ++n) {
      if(_idGlist[n]->getType() == AIG_GATE)
         linkAIG(_idGlist[n]->getLineNo() - 2);
   }

   // LinkPOs
   for(unsigned n = _header[0] + 1; n < _idGlist.size(); ++n)
      linkPO(_idGlist[n]->getLineNo() - 2);

   // Set symbols

   file.ignore();
   while(file.peek() != 'c' && file.peek() != EOF) {
      getline(file, line);
      if(line.empty()) break;
      string tok, symbol;
      size_t e = myStrGetTok(line, tok, 0);
      if(e == string::npos) return false;
      else symbol = line.substr(e + 1);
      char c = tok.front();
      tok.erase(tok.begin());
      int pos;
      if(tok.size()) myStr2Int(tok, pos);
      if(c == 'i') {
         _idGlist[info[pos][0]]->setSymbol(symbol);
      }
      else if(c == 'o') {
         _idGlist[info[_header[1] + _header[2] + pos][0]]->setSymbol(symbol);
      }
   }

   findUnused();
   findFloating();
   findDfs();

   return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
   cout << endl
   << "Circuit Statistics" << endl 
   << "==================" << endl
   << "  " << setw(7) << left << "PI" << setw(7) << right << _header[1] << endl
   << "  " << setw(7) << left << "PO" << setw(7) << right << _header[3] << endl
   << "  " << setw(7) << left << "AIG" << setw(7) << right << _header[4] << endl
   << "------------------" << endl
   << "  " << setw(7) << left << "Total" << setw(7) << right << _header[1] + _header[3] + _header[4] << endl;
}

void
CirMgr::printNetlist() const
{
   cout << endl;
   for(int i = 0, num = 0; i < _dfsList.size(); ++i) {
      if(_idGlist[_dfsList[i]]->getType() != UNDEF_GATE) {
         cout << "[" << num << "] ";
         _idGlist[_dfsList[i]]->printGate();
         ++num;
      }
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   FORPI(n)
      cout << " " << _idGlist[info[n][0]]->_id;
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   FORPO(n)
      cout << " " << _idGlist[info[n][0]]->_id;
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   if(_floating.size()) {
      cout << "Gates with floating fanin(s):";
      for(unsigned n = 0; n < _floating.size(); ++n)
         cout << " " << _floating[n];
      cout << endl;
   }
   if(_unused.size()) {
      cout << "Gates defined but not used  :";
      for(unsigned n = 0; n < _unused.size(); ++n)
         cout << " " << _unused[n];
      cout << endl;
   }
}

void
CirMgr::printFECPairs() const
{
   for(unsigned n = 0; n < _fecList.size(); ++n) {
      cout << "[" << n << "]";
      for(unsigned m = 0; m < _fecList[n]->size(); ++m)
         cout << " " << ((*_fecList[n])[m] % 2 ? "!" : "") << (*_fecList[n])[m] / 2;
      cout << endl;
   }
}

void
CirMgr::writeAag(ostream& outfile) const
{
   UnsignedList _aigDfsList;
   for(unsigned n = 0; n < _dfsList.size(); ++n)
      if(_idGlist[_dfsList[n]]->getType() == AIG_GATE)
         _aigDfsList.push_back(_dfsList[n]);
   outfile << "aag" << " " << _header[0] << " " << _header[1] << " " << _header[2] << " " << _header[3] << " " << _aigDfsList.size() << endl;
   FORPI(n)
      outfile << info[n][0] * 2 << endl;
   FORPO(n)
      outfile << INFOGATE(n)->_in[0]->_id * 2 + INFOGATE(n)->_inInv[0] << endl;
   for(unsigned n = 0; n < _aigDfsList.size(); ++n) {
      outfile 
      << _idGlist[_aigDfsList[n]]->_id * 2 << " "
      << _idGlist[_aigDfsList[n]]->_in[0]->_id * 2 + _idGlist[_aigDfsList[n]]->_inInv[0] << " "
      << _idGlist[_aigDfsList[n]]->_in[1]->_id * 2 + _idGlist[_aigDfsList[n]]->_inInv[1]
      << endl;
   }
   for(unsigned n = 0; n < _header[1]; ++n) {
      string symbol = _idGlist[info[n][0]]->getSymbol();
      if(symbol.size()) outfile << "i" << n << " " << symbol << endl;
   }
   for(unsigned n = _header[1] + _header[2]; n < _header[1] + _header[2] + _header[3]; ++n) {
      string symbol = _idGlist[info[n][0]]->getSymbol();
      if(symbol.size()) outfile << "o" << n - _header[1] - _header[2] << " " << symbol << endl;
   }
}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
   UnsignedList _coneList, _aigList, _piList;
   CirGate::resetmark();
   g->dfs(_coneList);
   for(unsigned n = 0; n < _coneList.size(); ++n) {
      if(_idGlist[_coneList[n]]->getType() == AIG_GATE) {
         _aigList.push_back(_coneList[n]);
      }
      else if(_idGlist[_coneList[n]]->getType() == PI_GATE)
         _piList.push_back(_coneList[n]);
   }
   sort(_piList.begin(), _piList.end());
   outfile << "aag" << " " << _header[0] << " " << _piList.size() << " " << _header[2] << " " << 1 << " " << _aigList.size() << endl;
   for(unsigned n = 0; n < _piList.size(); ++n)
      outfile << _piList[n] * 2 << endl;
   outfile << g->_id * 2 << endl;
   for(unsigned n = 0; n < _aigList.size(); ++n) {
      outfile 
      << _idGlist[_aigList[n]]->_id * 2 << " "
      << _idGlist[_aigList[n]]->_in[0]->_id * 2 + _idGlist[_aigList[n]]->_inInv[0] << " "
      << _idGlist[_aigList[n]]->_in[1]->_id * 2 + _idGlist[_aigList[n]]->_inInv[1]
      << endl;
   }
   for(unsigned n = 0; n < _piList.size(); ++n) {
      string symbol = _idGlist[_piList[n]]->getSymbol();
      if(symbol.size()) outfile << "i" << n << " " << symbol << endl;
   }
   string symbol = g->getSymbol();
   if(symbol.size()) outfile << "o" << 0 << " " << symbol << endl;
}
