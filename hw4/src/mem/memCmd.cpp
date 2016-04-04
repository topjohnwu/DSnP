/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include <sstream>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(sizeof(MemTestObj))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   vector<string> options;
   if (!CmdExec::lexOptions(option, options)) return CMD_EXEC_ERROR;
   if (options.empty()) return CmdExec::errorOption(CMD_OPT_MISSING, "");
   int numObjects = 0, arraySize = 0;
   for(int i = 0; i < options.size(); ++i) {
      if(myStrNCmp("-Array", options[i], 2) == 0) {
         if(arraySize > 0) return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
         else if(++i == options.size()) return CmdExec::errorOption(CMD_OPT_MISSING, options[i - 1]);
         else if(!myStr2Int(options[i], arraySize) || arraySize < 1) return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
      }
      else if(numObjects < 1) {
         if(!myStr2Int(options[i], numObjects) || numObjects < 1) return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
      }
      else return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
   }
   if(numObjects < 1) return CmdExec::errorOption(CMD_OPT_MISSING, "");
   else if(arraySize > 0) mtest.newArrs(numObjects, arraySize);
   else mtest.newObjs(numObjects);
   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   vector<string> options;
   if (!CmdExec::lexOptions(option, options)) return CMD_EXEC_ERROR;
   if (options.empty()) return CmdExec::errorOption(CMD_OPT_MISSING, "");
   int objId, numRandId, randIdx;
   bool arr = false, read = false, random = false;
   for(int i = 0; i < options.size(); ++i) {
      if(myStrNCmp("-Array", options[i], 2) == 0) {
         if(arr) return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
         else arr = true;
      }
      else if(myStrNCmp("-Index", options[i], 2) == 0) {
         if(read) return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
         else if(++i == options.size()) return CmdExec::errorOption(CMD_OPT_MISSING, options[i - 1]);
         else if(!myStr2Int(options[i], objId) || objId < 0) return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
         else read = true;
      }
      else if(myStrNCmp("-Random", options[i], 2) == 0) {
         if(read) return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
         else if(++i == options.size()) return CmdExec::errorOption(CMD_OPT_MISSING, options[i - 1]);
         else if(!myStr2Int(options[i], numRandId) || numRandId < 1) return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
         else {read = true; random = true; randIdx = i - 1;}
      }
      else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
   }
   if(!read) return CmdExec::errorOption(CMD_OPT_MISSING, "");
   else if(!random) {
      if(arr) {
         if(objId >= mtest.getArrListSize()) {
            cerr << "Size of array list (" << mtest.getArrListSize() << ") is <= " << objId << "!!" << endl;
            stringstream ss;
            ss << objId;
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, ss.str());
         }
         else mtest.deleteArr(objId);
      }
      else {
         if(objId >= mtest.getObjListSize()) {
            cerr << "Size of object list (" << mtest.getObjListSize() << ") is <= " << objId << "!!" << endl;
            stringstream ss;
            ss << objId;
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, ss.str());
         }
         else mtest.deleteObj(objId);
      }
   }
   else {
      if(arr) {
         if(mtest.getArrListSize() == 0) {
            cerr << "Size of array list is 0!!" << endl;
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[randIdx]);
         }
         for (int i = 0; i < numRandId; ++i)
            mtest.deleteArr(rnGen(mtest.getArrListSize()));
      }
      else {
         if(mtest.getObjListSize() == 0) {
            cerr << "Size of object list is 0!!" << endl;
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[randIdx]);
         }
         for (int i = 0; i < numRandId; ++i)
            mtest.deleteObj(rnGen(mtest.getObjListSize()));
      }
   }
   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


