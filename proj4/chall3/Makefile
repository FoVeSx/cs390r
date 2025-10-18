CC1=gcc

all:
	$(CC1) -o vuln vuln.c -Wall -Werror -g
	patchelf --set-interpreter ./ld-2.27.so ./vuln
	patchelf --set-rpath ./ ./vuln

clean:
	rm -f vuln
