/****************************************************************************
  FileName     [ util.cpp ]
  PackageName  [ util ]
  Synopsis     [ Global variables for util package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include "rnGen.h"
#include "myUsage.h"

//----------------------------------------------------------------------
//    Global variables in util
//----------------------------------------------------------------------

RandomNumGen  rnGen(0);  // use random seed = 0
MyUsage       myUsage;

size_t getHashSize(size_t s) {
   if (s < 8) return 7;
   if (s < 16) return 13;
   if (s < 32) return 31;
   if (s < 64) return 61;
   if (s < 128) return 127;
   if (s < 512) return 509;
   if (s < 2048) return 1499;
   if (s < 8192) return 4999;
   if (s < 32768) return 13999;
   if (s < 131073) return 59999;
   return 71993;
}

