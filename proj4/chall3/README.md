Your goal in this challenge is to spawn a shell using the tcache-dup technique we discussed in 
class. Source-code and makefile are provided, so feel free to recompile the binary if you wish, your
final exploit has to work on the handed out binary though.  

You should use one of the one_gadgets listed below to spawn your shell in this challenge.

Grading Criteria:  
    20% - Get a libc leak   
    20% - Overwrite one of the hooks   
    20% - Get a shell   
    40% - Writeup detailing your approach to the challenge   

one_gadget ./libc.so.6 output:
```
0x4f2c5 execve("/bin/sh", rsp+0x40, environ)
constraints:
  rsp & 0xf == 0
  rcx == NULL

0x4f322 execve("/bin/sh", rsp+0x40, environ)
constraints:
  [rsp+0x40] == NULL

0x10a38c execve("/bin/sh", rsp+0x70, environ)
constraints:
  [rsp+0x70] == NULL
```
