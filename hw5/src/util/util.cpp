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

