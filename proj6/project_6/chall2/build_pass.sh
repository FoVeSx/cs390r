#!/bin/sh

PASS_PATH=./build/chall2_analysis/libchall2_analysis.so
PASS_NAME=chall2_analysis
TARGET=chall2

# Clean up build directory if it already exists
if [ -d build ]; then rm -rf build; fi
if [ instrumented.ll ]; then rm -rf instrumented.ll; fi

# Compile the pass into library 
mkdir build && cd build
cmake ..
make
cd ..

# Compile helper code into object file
cc -c helper.c

# Emit llvm bit-code for target source-code
clang-14 -S -emit-llvm $TARGET.c -o $TARGET.ll

# Apply pass to target and emit transformed llvm-bitcode
/lib/llvm-14/bin/opt -load-pass-plugin $PASS_PATH -passes=$PASS_NAME $TARGET.ll -o instrumented.bc

# Compile llvm-bitcode into elf-executable
/lib/llvm-14/bin/llc -filetype=obj instrumented.bc
cc instrumented.o helper.o -o instrumented

# Emit human-readable llvm-bitcode to instrumented.ll so we can manually inspect it
llvm-dis-14 instrumented.bc

# Clean up files that were generated during build
rm -rf build helper.o $TARGET.ll instrumented.bc instrumented.o
