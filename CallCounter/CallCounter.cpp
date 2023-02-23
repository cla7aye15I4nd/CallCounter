#include "llvm-c/Core.h"
#include "llvm/Pass.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

using namespace llvm;

const char kCallCounterName[] = "__callcounter";
const char KCallCounterReport[] = "__callcounter_report";

namespace
{
  struct CallCounterPass : public FunctionPass
  {
    static char ID;
    CallCounterPass() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override
    {
      Module *M = F.getParent();
      LLVMContext &C = M->getContext();
      M->getOrInsertFunction(kCallCounterName, Type::getVoidTy(C), Type::getInt8PtrTy(C));
      M->getOrInsertFunction(KCallCounterReport, Type::getVoidTy(C));

      Instruction *InsertPt = &*F.getEntryBlock().getFirstInsertionPt();
      IRBuilder<> IRB(InsertPt);

      Value *FuncName = IRB.CreateGlobalStringPtr(F.getName());
      IRB.CreateCall(M->getFunction(kCallCounterName), FuncName);

      for (auto &BB : F)
      {
        if (F.getName() == "main")
        {
          if (auto *Ret = dyn_cast<ReturnInst>(BB.getTerminator()))
          {
            IRB.SetInsertPoint(Ret);
            IRB.CreateCall(M->getFunction(KCallCounterReport));
          }
        }

        for (auto &I : BB)
        {
          if (auto *Call = dyn_cast<CallInst>(&I))
          {
            if (Call->getCalledFunction()->getName() == "exit")
            {
              IRB.SetInsertPoint(Call);
              IRB.CreateCall(M->getFunction(KCallCounterReport));
            }
          }
        }
      }

      return true;
    }
  };
} // anonymous namespace

char CallCounterPass::ID = 0;

static void registerCallCounterPass(const PassManagerBuilder &,
                                    legacy::PassManagerBase &PM)
{
  PM.add(new CallCounterPass());
}

static RegisterStandardPasses
    RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                   registerCallCounterPass);
