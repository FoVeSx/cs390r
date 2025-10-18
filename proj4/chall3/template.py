#!/usr/bin/python3

from pwn import *

elf = context.binary = ELF('./vuln')
libc = elf.libc
p = elf.process()

if args.GDB:
    gdb.attach(p, gdbscript=f'''
        continue
    ''')

def add_service(name, url, auto, pw):
    p.sendline(str(1))
    p.sendline(name)
    p.sendline(url)
    p.sendline(str(auto))
    if pw != 0:
        p.sendline(str(pw))

def view_service(name):
    p.sendline(str(2))
    p.sendline(name)

def free(index):
    p.sendline(str(3))
    p.sendline(str(index))

p.interactive()
