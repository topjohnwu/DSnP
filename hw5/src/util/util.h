/****************************************************************************
  FileName     [ util.h ]
  PackageName  [ util ]
  Synopsis     [ Define function interfaces and global variables for util ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef UTIL_H
#define UTIL_H

#include <istream>
#include "rnGen.h"
#include "myUsage.h"

using namespace std;

// Extern global variable defined in util.cpp
extern RandomNumGen  rnGen;
extern MyUsage       myUsage;

// In myString.cpp
extern int myStrNCmp(const string& s1, const string& s2, unsigned n);
extern size_t myStrGetTok(const string& str, string& tok, size_t pos = 0,
                          const char del = ' ');
extern bool myStr2Int(const string& str, int& num);
extern bool isValidVarName(const string& str);

// In myGetChar.cpp
extern char myGetChar(istream&);
extern char myGetChar();


#endif // UTIL_H
