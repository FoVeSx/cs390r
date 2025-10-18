import os
import sys

from triton import *
from pwn import *

# Directory that contains the memory/register dumps
# Generated using snapshot.py
DUMP_DIR = "./dump/"

# Hitting this address indicates that we found the correct answer
GOAL_ADDR = 0x000055555555520c

# Hitting this address indicates that we exit the check-sum function on a failure condition
# We could like to avoid hitting this
FAIL_ADDR = 0x00005555555551f9

# The target function verifies our symbolic data in a loop, 1 byte at a time
# If we fail a byte, the function exits, so taking the backward edge to execute the loop again for 
# the next input-byte is a win-condition 
#
# Hitting this address means that we are re-executing the loop
NEXT_LOOP_ITER = 0x555555555200

# Symbolic variables with random inputs at the first iteration.
FOUND_VARS = [0x0 for x in range(0x10)]

# Emulate the binary code.
def emulate(ctx, pc):
    counter = 0

    while pc:
        # Fetch opcodes for this instruction
        opcodes = ctx.getConcreteMemoryAreaValue(pc, 16)

        # Parse the Triton instruction at `pc`
        instruction = Instruction(pc, opcodes)

        # Execute instruction
        ctx.processing(instruction)

        # End of function with the incorrect answer
        if pc == FAIL_ADDR:
            return 1

        # End of function with the correct answer
        # Hitting the GOAL_ADDR means that we found the correct checksum
        if pc == GOAL_ADDR:
            return 0

        # TODO-2

        # Advance pc
        pc = ctx.getConcreteRegisterValue(ctx.registers.rip)

# Loads a memory/register dump from disk into ctx
def loadDump(ctx):
    reg_file = DUMP_DIR + "regs"
    mem_file = DUMP_DIR + "memory_maps"

    # Load registers from dump into ctx
    with open(reg_file) as f:
        regs = eval(f.read())

        ctx.setConcreteRegisterValue(ctx.registers.rax,    regs['rax'])
        ctx.setConcreteRegisterValue(ctx.registers.rbx,    regs['rbx'])
        ctx.setConcreteRegisterValue(ctx.registers.rcx,    regs['rcx'])
        ctx.setConcreteRegisterValue(ctx.registers.rdx,    regs['rdx'])
        ctx.setConcreteRegisterValue(ctx.registers.rdi,    regs['rdi'])
        ctx.setConcreteRegisterValue(ctx.registers.rsi,    regs['rsi'])
        ctx.setConcreteRegisterValue(ctx.registers.rbp,    regs['rbp'])
        ctx.setConcreteRegisterValue(ctx.registers.rsp,    regs['rsp'])
        ctx.setConcreteRegisterValue(ctx.registers.rip,    regs['rip'])
        ctx.setConcreteRegisterValue(ctx.registers.r8,     regs['r8'])
        ctx.setConcreteRegisterValue(ctx.registers.r9,     regs['r9'])
        ctx.setConcreteRegisterValue(ctx.registers.r10,    regs['r10'])
        ctx.setConcreteRegisterValue(ctx.registers.r11,    regs['r11'])
        ctx.setConcreteRegisterValue(ctx.registers.r12,    regs['r12'])
        ctx.setConcreteRegisterValue(ctx.registers.r13,    regs['r13'])
        ctx.setConcreteRegisterValue(ctx.registers.r14,    regs['r14'])
        ctx.setConcreteRegisterValue(ctx.registers.eflags, regs['eflags'])
        ctx.setConcreteRegisterValue(ctx.registers.cs,     regs['cs'])
        ctx.setConcreteRegisterValue(ctx.registers.ss,     regs['ss'])
        ctx.setConcreteRegisterValue(ctx.registers.ds,     regs['ds'])
        ctx.setConcreteRegisterValue(ctx.registers.es,     regs['es'])
        ctx.setConcreteRegisterValue(ctx.registers.fs,     regs['fs'])
        ctx.setConcreteRegisterValue(ctx.registers.gs,     regs['gs'])

    # Load memory from dump into ctx
    with open(mem_file) as f:
        mem_mappings = eval(f.read())

        for i in range(0, len(mem_mappings)):
            data_file = DUMP_DIR + "raw_memory/" + f"raw_{i}"
            start = mem_mappings[i]['start']
            size  = mem_mappings[i]['size']

            # Get the raw byte backing
            with open(data_file, 'rb') as df:
                data = df.read()
                assert(len(data) == size)
                ctx.setConcreteMemoryAreaValue(start, data)

# This sets up the input to the function consisting of a concrete size and a symbolized array
def symbolize_inputs(ctx):
    # TODO-1
    pass

# Initialize state
def initialize():
    # Set architecture to x86
    ctx = TritonContext(ARCH.X86_64)

    # Optimizations to make the engine run a little faster
    ctx.setMode(MODE.ALIGNED_MEMORY, True)
    ctx.setMode(MODE.CONSTANT_FOLDING, True)
    ctx.setMode(MODE.AST_OPTIMIZATIONS, True)
    ctx.setMode(MODE.ONLY_ON_SYMBOLIZED, True)

    # Load the meory dump
    loadDump(ctx)

    # Symbolize inputs to the target
    symbolize_inputs(ctx)

    return ctx

def main():
    # Initialize our context from the memory dump and symbolize inputs
    ctx = initialize()

    print(f"[+] RIP set to: {hex(ctx.getConcreteRegisterValue(ctx.registers.rip))}")

    # Starting emulation
    print("Starting emulation")
    res = emulate(ctx, ctx.getConcreteRegisterValue(ctx.registers.rip))
    print("Finished emulation")

    if res == 0:
        print("Found answer")
        print(f"Answer: {''.join(chr(x) for x in FOUND_VARS)}")
    else:
        print("Failed to find solve-value")
        print(f"Final state of FOUND_VARS: {FOUND_VARS}")

if __name__ == '__main__':
    retValue = main()
    sys.exit(retValue)
