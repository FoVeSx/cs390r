Your goal in this challenge is to spawn a shell using the house of force technique we discussed in 
class. Source-code and makefile are provided, so feel free to recompile the binary if you wish, your
final exploit has to work on the handed out binary though.  

Do NOT use a one_gadget for this challenge. Spawn your shell by calling system('/bin/sh')

Grading Criteria:  
    20% - Get a libc & heap leak   
    20% - Overwrite top-chunk sizefield
    20% - Overwrite a hook with the address of system
    40% - Spawm a shell
