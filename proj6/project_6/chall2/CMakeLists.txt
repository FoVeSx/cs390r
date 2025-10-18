cmake_minimum_required(VERSION 3.13.4)
project(chall2_analysis)

# Verify LLVM installation directory
set(LT_LLVM_INSTALL_DIR "" CACHE PATH "/lib/llvm-14/")

# Set CMake Standard. 14, 17 & 20 are all valid for llvm-14
set(CMAKE_CXX_STANDARD 14)

# Make sure LLVM_HOME env variable was previously set
# Downloading llvm-source and setting this might help with debugging
#if(NOT DEFINED ENV{LLVM_HOME})
#    message(FATAL_ERROR "$LLVM_HOME is not defined")
#else ()
#    set(ENV{LLVM_DIR} $ENV{LLVM_HOME}/lib/cmake/llvm)
#endif()

# Find llvm-14
find_package(LLVM 14 REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

# Set LLVM headers and library paths
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

add_subdirectory(chall2_analysis)
