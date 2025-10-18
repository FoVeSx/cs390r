#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAXSIZE 512

__attribute__((constructor)) void ignore_me(){
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
}

char *file;

int error_exit(char *err) {
    printf("%s", err);
    exit(1);
}

int get_int() {
    char buf[8];
    if (fgets(buf, sizeof(buf), stdin) == NULL) { error_exit("Failed to read in integer"); }
    return atoi(buf);
}

int verify_user(char *user) {
    return strncmp(user, "USER ", 5);
}

int verify_pass(char *pass) {
    return strncmp(pass, "PASS ", 5);
}

int serve_retr(char *filename) {
    int fd; 
    int sz;

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        puts("550 Error opening requested file."); 
        return 1;
    }

    puts("150 File opened successfully.");

    read(fd, file, MAXSIZE);

    return 0;
}

int serve_allo(char *cmd) {
    char buf[0x20];
    long long size;

    memcpy(buf, cmd, 0x20);
    size = strtoll(buf, NULL, 0);

    file = malloc(size);
    return 0;
}

int handle_cmd(char *cmd) {
    if (strncmp(cmd, "LIST", 4) == 0) {
        puts("150 Displaying directory listing.");
        system("/bin/ls");
        puts("200 The requested action has been successfully completed");
    } else if (strncmp(cmd, "RETR ", 5) == 0) {
        serve_retr(cmd+5);
    } else if (strncmp(cmd, "ALLO ", 5) == 0) {
        serve_allo(cmd+5);
    } else if (strncmp(cmd, "QUIT", 4) == 0) {
        puts("221 Service closing control connection.");
        return 1;
    }

    return 0;
}

int serve_request() {
    char *ptr = malloc(0);
    file = malloc(MAXSIZE);
    char cmd[0x30];
    char user[MAXSIZE];
    char pass[MAXSIZE];

    bzero(cmd, 0x20);
    bzero(user, MAXSIZE);
    bzero(pass, MAXSIZE);

    puts("220 cs390r FTP server ready.");
    fgets(user, MAXSIZE, stdin);

    while (verify_user(user) != 0) {
        puts("530 Please login with USER and PASS.");
        fgets(user, MAXSIZE, stdin);
    }
    printf(user+5);

    puts("331 Please Please specify the password.");
    fgets(pass, MAXSIZE, stdin);

    while (verify_pass(pass) != 0) {
        puts("530 Please login with USER and PASS.");
        fgets(pass, MAXSIZE, stdin);
    }
    printf(pass+5);

    // Just gonna let anyone login. Sharing is caring.
    puts("230 Guest login ok, access restrictions apply.");

    while (1) {
        fgets(cmd, sizeof(cmd), stdin);
        if (handle_cmd(cmd) == 1) { break; }
    }
    return 0;
}

int main() {
    return serve_request();
}
