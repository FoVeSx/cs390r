#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Constants.h"

/*
   This sample pass simply inserts code at the beginning of each function so that when it is called,
   it calls the `hello` function from helper.c to emit some simple debug-prints. Modify the code in
   the visitor function for your assignment. You shouldn't have to modify anything else in this 
   file.
*/

using namespace llvm;

/// Anonymous namespace to separate this module from rest of llvm
namespace {

/// This method does the actual work of this pass.
/// It is called on every function that is hit during compilation
void visitor(Function &func) {

    // Register an empty function `hello(char *func_name)` so that we can reference 
    // it from our pass. You can look at how this is implemented in the emited llvm-bitcode
    LLVMContext& Ctx = func.getContext();
    FunctionCallee hello = func.getParent()->getOrInsertFunction("hello", Type::getVoidTy(Ctx),
            Type::getInt8PtrTy(Ctx));

    // Get the first block of this function
    BasicBlock &first_block = *func.begin();

    // Get the first instruction from this block and create an IRBuilder using it
    IRBuilder<> builder(first_block.getFirstNonPHI());

    // Construct the call to the hello-function, providing the necessary arguments
    Value *func_name = builder.CreateGlobalStringPtr(func.getName());
    Value *args[] = {func_name};
    builder.CreateCall(hello, args);
}

struct chall2_analysis : PassInfoMixin<chall2_analysis> {
    /// Main entry point
    /// This defines that this pass should be run on every function (instead of eg. every
    /// individual instruction or every block). It also indicates if this pass invalidated any other
    /// passes or modified the code using the return value. In this case, 
    /// `PreservedAnalysis::none()` indicates that llvm should assume that we changed this function
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
        visitor(F);
        return PreservedAnalyses::none();
    }

    /// Call this plugin on every function, otherwise it will be skipped for functions with the
    /// `optnone` attribute
    static bool isRequired() { return true; }
};
} // End anonymous namespace

/// Register this plugin
llvm::PassPluginLibraryInfo get_chall2_analysis_plugin_info() {
    return {LLVM_PLUGIN_API_VERSION, "chall2_analysis", LLVM_VERSION_STRING, [](PassBuilder &PB) {
        PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "chall2_analysis") {
                        FPM.addPass(chall2_analysis());
                        return true;
                    }
                    return false;
                }
            );
        }
    };
}

/// This lets `opt` find and call this plugin
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return get_chall2_analysis_plugin_info();
}

