#include <iostream>
#include <fstream>

#include "pin.H"

using std::cerr;
using std::endl;
using std::ios;
using std::ofstream;
using std::string;

ofstream OutFile;
UINT64 main_img_start = 0;
UINT64 main_img_end = 0;

/// Struct represents a call instruction using its src and dst
struct callInstr {
    UINT64 addr_pre;
    UINT64 addr_post;
};

// Array of function calls to be logged
std::vector<callInstr> instr_trace;

/// Log a call instruction with its source and destination
/// addr_pre  - Address of the call instruction
/// addr_post - Address that the call instruction is transferring execution too
VOID log_instr(UINT64 addr_pre, UINT64 addr_post) {
    // TODO
}

/// Pin calls this function every time a new instruction is encountered
/// ins - Current instruction
/// v   - Ignore
VOID Instruction(INS ins, VOID* v) {
    // TODO
}

/// Executed on imageload, used to find the base address of executable section
/// img - Current image
/// v   - Ignore
VOID ImageLoad(IMG img, VOID* v) { 
    // TODO
}

KNOB< string > KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "results.out", 
        "specify output file name");

/// This function is called when the application exits
VOID Fini(INT32 code, VOID* v) {
    // Write to a file since cout and cerr maybe closed by the application
    OutFile.setf(ios::showbase);
    for (auto call: instr_trace) {
        OutFile << std::hex << std::showbase << call.addr_pre << " -> " << call.addr_post << endl;
    }
    OutFile.close();
}

/// Print out usage instructions
INT32 Usage() {
    cerr << "This tool logs all instructions that have indirect calls and their dynamic \
        destination address" << endl;
    cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

int main(int argc, char* argv[]) {
    // Initialize Pin
    if (PIN_Init(argc, argv)) return Usage();

    OutFile.open(KnobOutputFile.Value().c_str());

    // Register ImageLoad to be called to instrument image loading
    IMG_AddInstrumentFunction(ImageLoad, 0);

    // Register Instruction to be called to instrument instructions
    INS_AddInstrumentFunction(Instruction, 0);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);

    // Start the target program
    PIN_StartProgram();

    return 0;
}
