/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include <algorithm>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

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
static unsigned colNo  = 0;  // in printing, colNo needs to ++
//static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
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
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/

CirMgr::CirMgr() { _idGlist.push_back(new ConstGate());}

CirMgr::~CirMgr() { 
   delete _idGlist[0];
   for(unsigned n = 0 ; n < _glist.size(); ++n) delete _glist[n];
}

CirGate*
CirMgr::getGate(unsigned gid) const {
   if(gid > m + o) return 0;
   return _idGlist[gid];
}

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
   if(head == "aag") file >> m >> i  >> l >> o >> a;
   else { return false; }
   ++lineNo;

   for(unsigned n = 0; n < i; ++n, ++lineNo) {
      unsigned x;
      file >> x;
      vector<unsigned> v;
      v.push_back(x);
      info.push_back(v);
   }
   
   for(unsigned n = 0; n < l; ++n, ++lineNo) { } // Latch placeholder

   for(unsigned n = 0, b = 1; n < o; ++n, ++lineNo, ++b) {
      unsigned x;
      file >> x;
      vector<unsigned> v;
      v.push_back(m + b); v.push_back(x);
      info.push_back(v);
   }

   for(size_t n = 0; n < a; ++n, ++lineNo) {
      unsigned x, y, z;
      file >> x >> y >> z;
      vector<unsigned> v;
      v.push_back(x); v.push_back(y); v.push_back(z);
      info.push_back(v);
   }

   //_glist.resize(info.size(), 0);
   _idGlist.resize(m + o + 1, 0);

   for(unsigned n = 0; n < i; ++n) {
      _glist.push_back(new PIGate(info[n][0] / 2, n + 2));
      addToIdGlist(_glist.back());
   }

   for(unsigned n = i; n < i + l; ++n) {}

   for(unsigned n = i + l; n < i + l + o; ++n) {
      _glist.push_back(new POGate(info[n][0], n + 2));
      addToIdGlist(_glist.back());
   }

   for(unsigned n = i + l + o; n < i + l + o + a; ++n) {
      _glist.push_back(new AIGGate(info[n][0] / 2, n + 2));
      addToIdGlist(_glist.back());
   }

   for(unsigned n = 0; n < m + 1; ++n) {
      CirGate* g = dynamic_cast<AIGGate*>(_idGlist[n]);
      if(g == 0) continue;
      string type = _idGlist[n]->getTypeStr();
      linkAIG(_idGlist[n]->getLineNo() - 2);
   }

   for(unsigned n = m + 1; n < m + o + 1; ++n)
      linkPO(_idGlist[n]->getLineNo() - 2);

   file.ignore();
   // Set symbols
   while(file.peek() != 'c' && file.peek() != EOF) {
      //cout << "Sym!" << endl;
      getline(file, line);
      //cout << line << endl;
      if(line.empty()) break;
      string tok, symbol;
      size_t e = myStrGetTok(line, tok, 0);
      if(e == string::npos) return false;
      else symbol = line.substr(e + 1);
      char c = tok.front();
      tok.erase(tok.begin());
      int pos;
      if(tok.size()) myStr2Int(tok, pos);
      //cout << pos << " " << symbol << endl;
      if(c == 'i') {
         _glist[pos]->setSymbol(symbol);
      }
      else if(c == 'o') {
         _glist[i + l + pos]->setSymbol(symbol);
      }
   }
   return true;
}

void
CirMgr::addToIdGlist(CirGate* n) { _idGlist[n->getId()] = n;}

void
CirMgr::linkPO(unsigned n) {
   CirGate* input = getGate( info[n][1] / 2 );
   if(input == 0) {
      input = new UndefGate(info[n][1] / 2);
      _glist.push_back(input);
      addToIdGlist(input);
   }
   _glist[n]->setIn(input, info[n][1] % 2);
}

void
CirMgr::linkAIG(unsigned n) {
   CirGate* in1 = getGate( info[n][1] / 2 );
   CirGate* in2 = getGate( info[n][2] / 2 );
   if(in1 == 0) {
      in1 = new UndefGate(info[n][1] / 2);
      _glist.push_back(in1);
      addToIdGlist(in1);
   }
   if(in2 == 0) {
      in2 = new UndefGate(info[n][2] / 2);
      _glist.push_back(in2);
      addToIdGlist(in2);
   }
   _glist[n]->setIn(in1, info[n][1] % 2);
   _glist[n]->setIn(in2, info[n][2] % 2);
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
        << "  " << setw(7) << left << "PI" << setw(7) << right << i << endl
        << "  " << setw(7) << left << "PO" << setw(7) << right << o << endl
        << "  " << setw(7) << left << "AIG" << setw(7) << right << a << endl
        << "------------------" << endl
        << "  " << setw(7) << left << "Total" << setw(7) << right << i+o+a << endl;
}

void
CirMgr::printNetlist() const
{
   CirGate::resetmark();
   int num = 0;
   for(unsigned n = i; n < i + o; ++n)
      _glist[n]->dfs(num);
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(unsigned n = 0; n < i; ++n)
      cout << " " << _glist[n]->getId();
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(unsigned n = i; n < i + o; ++n)
      cout << " " << _glist[n]->getId();
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   IdList floating, unused, v;
   for(unsigned n = i + o + l + a; n < _glist.size(); ++n) {
      v = static_cast<UndefGate*>(_glist[n])->getOutList();
      floating.insert(floating.end(), v.begin(), v.end());
   }
   for(unsigned n = 0; n < i + o + l + a; ++n) {
      if(n >= i && n < i + o) continue;
      if(_glist[n]->unused()) unused.push_back(_glist[n]->getId());
   }
   sort(floating.begin(), floating.end());
   sort(unused.begin(), unused.end());
   if(floating.size()) {
      cout << "Gates with floating fanin(s):";
      for(unsigned n = 0; n < floating.size(); ++n)
         cout << " " << floating[n];
      cout << endl;
   }
   if(unused.size()) {
      cout << "Gates defined but not used  :";
      for(unsigned n = 0; n < unused.size(); ++n)
         cout << " " << unused[n];
      cout << endl;
   }
}

void
CirMgr::writeAag(ostream& outfile) const
{
   CirGate::resetmark();
   IdList aigDfsList;
   for(unsigned n = i; n < i + o; ++n)
      _glist[n]->dfs(aigDfsList);
   outfile << "aag" << " " << m << " " << i << " " << l << " " << o << " " << aigDfsList.size() << endl;
   for(unsigned n = 0; n < i; ++n)
      cout << info[n][0] << endl;
   for(unsigned n = i + l; n < i + o + l; ++n)
      cout << info[n][1] << endl;
   for(unsigned n = 0; n < aigDfsList.size(); ++n) {
      cout << info[aigDfsList[n]][0] << " " 
           << info[aigDfsList[n]][1] << " " 
           << info[aigDfsList[n]][2] << endl;
   }
   for(unsigned n = 0; n < i; ++n) {
      string symbol = _glist[n]->getSymbol();
      if(symbol.size()) outfile << "i" << n << " " << symbol << endl;
   }
   for(unsigned n = i + l; n < i + l + o; ++n) {
      string symbol = _glist[n]->getSymbol();
      if(symbol.size()) outfile << "o" << n - i - l << " " << symbol << endl;
   }
}
