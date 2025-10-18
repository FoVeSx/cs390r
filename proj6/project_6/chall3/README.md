### Project 6 - Challenge 3

You are working as an embedded security researcher at company xyz. Your team wrote an exploit for a
bug they found, but they have had trouble bypassing a small check that is stopping the exploit from
successfully completing.

Your job is to use symbolic execution to find an input that bypasses this check and allows your team
to continue. To do this you will take a snapshot of the target right at the checksum function and
symbolically analyze it from there.

It has the following signature, so at the snapshot location, `rdi` points to the buffer and `rsi` 
points to the size. From our testing we were able to determine that the size is always 16.

```c
int checksum(char * input, int size);
```

##### Provided Files:
- script.py         - Use this to script out your solution
- snapshot.py       - GDB-script used to dump memory. Feel free to reuse this if you want to do this
                      type of symbolic execution or snapshot fuzzing for your final presentation
- target            - Target binary used to generate memory dump, not necessary for script to run,
                      but can be used to test solution or to find addresses of instructions you 
                      want to manually handle
- dump              - Memory/register layout of target at time of snapshot (TODO-0)
    - regs          - Json dictionary describing register->value mappings at time of snapshot
    - memory_maps   - Json dictionary describing the memory layout at time of snapshot
    - raw_memory    - Directory that contains the raw memory backing data described in `memory_maps`
    - files/raw_files - Describe file-dumps, Ignore for this project

#### Installation 
```sh
# Install capstone library (Need version 4.0.2)
wget https://github.com/capstone-engine/capstone/archive/refs/tags/4.0.2.zip
unzip 4.0.2.zip && mv capstone-4.0.2 capstone && cd capstone
./make.sh
sudo make install

# Build Triton
git clone https://github.com/JonathanSalwan/Triton && cd Triton
mkdir build && cd build
cmake ..
make
sudo make install
```

#### Instructions

Take a few minutes to familiarize yourself with the initial code provided in `script.py`. It
initializes an empty context, parses/loads the memory/register dump and then calls some functions
that you now need to complete. Also make sure to get a decent idea of the purposes of the global
variables.

> TODO-0

We want to focus on the `checksum` function without having to worry about relocation/emulating
library functions like fgets(). To accomplish this we'll be working with a snapshot of the target.
Generate the snapshot using the below commands

```sh
gdb target
b *checksum
run (After this it will be waiting for some input, so just provide it some)
source snapshot.py
fulldump
```

This should generate a dump directory that will be loaded as input to your script. This directory
contains the entire memory/register state at the start of `checksum`

> TODO-1

First you are going to have to start symbolizing our inputs. Since we had a concrete memory/register
dump, we were able to give everything thus far a concrete value. Now complete the `symbolize_inputs`
function. It should accomplish the following tasks:

1. Retrieve memory address of input array (rdi)
1. Retrieve size of input array (rsi)
3. Iterate through the array, and mark the input memory region as symbolic so the engine can set up 
    constraints for it during execution

> TODO-2

Now it is time to start emulating the instructions from the memory dump. 

There are multiple approaches to solving this and I would encourage you to check out the Triton ctf
writeups for some ideas. The general idea is that since our input is looped over, and the loop early
exits once a character is wrong, we just need constraints to make sure we don't early exit the loop
and keep iterating until we have the entire solution.

One valid approach would be to instrument the compare instructions similar as to what we did in
demo_2 during class. You can then add constraints to it to do what you want.

Another valid approach would be to check if an instruction is a symbolized branch, and if so, setup
the relevant constraints so the branch takes the path you expect (this is approach is demonstrated
in the ekoparty writeup)

You can confirm that your password is correct by providing it as the input to the `target` binary.

#### Rubric
For full points, complete all of the parts listed for this assignment below.
```
25%  Complete the `symbolize_inputs` function
45%  Complete the emulation to print the correct password
30%  Short writeup describing how exactly the password is computed (This should not only 
     include your own code but also how the engine does things in the background behind the apis)
```

#### Additional Resources that may be helpful
- https://github.com/JonathanSalwan/Triton/blob/master/src/examples/python/ctf-writeups/ - example scripts
- https://triton-library.github.io/documentation/doxygen/     - documentation
- https://raw.githubusercontent.com/JonathanSalwan/Triton/master/publications/SSTIC2015_English_slide_detailed_version_Triton_Concolic_Execution_FrameWork_FSaudel_JSalwan.pdf
