### Project 5 - Challenge 3

In this assignment you will be fuzzing a custom-made binary. This binary is only around ~100 lines
big, and has plenty of bugs that you should be able to find with light fuzzing. The goals remain the
exact same as in challenge 2, except that you will be required to use address sanitizer for your
second bug.

Submit a writeup that contains relevant screenshots and code examples to showcase that you
successfully achieved the goals listed below. (including your full harness code)
```
40% Setup the target and harness it so you can fuzz it with afl++
30% Find a bug and triage it to find and fix the first bug
30% Recompile with the previous fix, fuzz to find a second bug, and triage again to find and 
    fix this bug. This time compile the binary using address-sanitizer to look for harder bugs.
```

The added difficulty in this challenge comes mostly from the fact that you can't just run afl
against the target since it does not expose a trivially fuzzable input like the previous objdump
target. You will need to write a harness to interact with the binary, and then fuzz the harness to
indirectly interact with the provided code. 

Your bugs have to be crashes in the actual target and not in your harness.
