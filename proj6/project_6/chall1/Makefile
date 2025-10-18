TARGET=./chall1

all:
	cd pintool && make
clean:
	rm -f *.out *.log
	cd pintool && make clean
run:
	cd pintool && make && cd ..
	$(PIN_ROOT)/pin -t ./pintool/obj-intel64/dump_indirect_addrs.so -- $(TARGET)
