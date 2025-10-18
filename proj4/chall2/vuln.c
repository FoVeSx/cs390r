#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

// gcc vuln.c -o vuln

__attribute__((constructor)) void ignore_me(){
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
}

typedef struct {
	uintptr_t (*eat)();
	uintptr_t (*look)();
	char *username;
} Meal;

Meal *meal;

int win() {
	char flag[32] = {0};
	FILE *fd = fopen("flag.txt","r");

	if (fd == NULL) {
		puts("Something went wrong while opening the flag file.");
		exit(1);
	}
	fgets(flag, 0x30, fd);
	printf("The flag is: %s\n", flag);
	exit(1);
}

void f1() {
 	printf("Closely inspects broccoli... %p\n", win);
}

void f2() {
	puts("Nom Nom Nom ... this food is disgusting");
}

void init() {
	meal->look = (void*)f1;
	meal->eat  = (void*)f2;
}

void add_desert() {
	puts("What type of dessert do you want");
	char* buf = (char*)malloc(8);
	read(0, buf, 8);
}

void trash() {
	char var;
  	puts("You could have at least fed it to the cat...");
  	puts("Are you sure that you would not rather do that?");
	scanf("%c", &var);
	getchar();
	if (var == 'y'){
		puts("Good choice!");
	} else {
		puts("Fine, have it your way");
		free(meal);
	}
}

void print_menu() {
	puts("+=========:[ Menu ]:========+");
	puts("| [1] Eat Meal              |");
	puts("| [2] Add a Desert	    |");
	puts("| [3] look at meal          |");
	puts("| [4] Throw meal away       |");
	puts("| [5] Log out		    |");
	puts("+===========================+");
	printf("\n > ");
}

void cook() {
	int choice;
	char buf[0x8];

	if (fgets(buf, sizeof(buf), stdin) == NULL) { exit(1); }
	choice = atoi(buf);

	switch(choice) {
		case 1:
			(*meal->eat)();
			break;
		case 2:
			add_desert();
			break;
		case 3:
			(*meal->look)();
			break;
		case 4:
			trash();
			break;
		case 5:
			exit(0);
		default:
			puts("Invalid option!");
			exit(1);
	}
}

int main() {
	meal = (Meal*)malloc(sizeof(meal));
	init();

 	puts("Are you ready to eat?");
	while(1){
		print_menu();
		cook();
	}
	return 0;
}
