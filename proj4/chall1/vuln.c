#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_ITEMS 10

//gcc vuln.c -no-pie -o vuln

/*
File:
vuln: ELF 64-bit LSB executable, x86-64, version 1 (SYSV), 
dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, 
BuildID[sha1]=2f0128553dd4bda3dc03271589ae5df6471ba9a7, for GNU/Linux 3.2.0, 
not stripped

Mitigations:
	Arch:       amd64-64-little
    RELRO:      Partial RELRO
    Stack:      Canary found
    NX:         NX enabled
    PIE:        No PIE (0x400000)
    SHSTK:      Enabled
    IBT:        Enabled
    Stripped:   No

Strategy:
	leak canary
	integer underflow/overflow to get access to call use_special_item
	buffer overflow gives pc control 
	call main again
	leak libc and rebase
	then rop
	spawn shell
*/
__attribute__((constructor)) void ignore_me(){
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
}

int wallet = 0x1337;
uint32_t item_count = 1;
bool owns_special = false;

struct item {
	uint32_t id;
	char name[0x8];
	uint32_t price;
};

struct item items[10];

void init_shop() {
	srand(time(NULL));
	
	items[0].id = 0;
	items[0].price = 999999;
	strcpy(items[0].name,"special");

	while (item_count < 5) {

		items[item_count].id = item_count;
		items[item_count].price = rand() % 999;

		for (int j = 0; j < 6; j++) {
			items[item_count].name[j] = 'A' + (rand() % 26);
		}
		item_count ++;
	}
}

int get_num() {
	char buf[0x20];

	if (fgets(buf, sizeof(buf), stdin) == NULL) { 
		exit(-1); }  
    return atoi(buf);
}

void view_items() {
	for (int i = 0; i < item_count; i++) {
		printf("%d: %s\t\t%d\n",items[i].id, items[i].name, items[i].price);
	}
}

void buy_items() {
	int choice;
	int num;

	view_items();

	puts("Which item do you want to buy?");
	choice = get_num();

	puts("How many of this item do you want to buy?");
	num = get_num();

	if (wallet < items[choice].price) {
		puts("You dont have enough money to make this purchase.");
		return;
	} 
	if (choice == 0) {
		owns_special = true;
	}

	wallet -= num * items[choice].price; // vuln here, make num negative
	printf("Congratulations you bought %d %s's.\n",num,items[choice].name);
}

void add_item() {

	if (item_count >= 10) {
		puts("You have reached the max amount of items to add.");
		return;
	}

	items[item_count].id = item_count;
	
	puts("What is your item called?");
	if (fgets(items[item_count].name, 0x8, stdin) == NULL) { exit(-1); }  

	puts("How much do you want to charge for your item?");
	items[item_count].price = get_num();

	while(items[item_count].price > 1000) {
		puts("We dont support such expensive sales, lower your prices!");
		items[item_count].price = get_num();
	}
}

void use_special_item() {
	char buf[0x28];

	if (owns_special == false) {
		puts("You can't use what you don't own");
		return;
	}
	puts("The special item gives you a stack buffer overflow!");
	gets(buf);
}

void print_shop_menu() {
	printf("Your current available funds: %d\n\n", wallet);
	puts("+=========:[ Menu ]:========+");
	puts("| [1] View items for sale   |");
	puts("| [2] Buy items	     	    |");
	puts("| [3] Put up items for sale |");
	puts("| [4] Use special item	    |");
	puts("| [5] Exit Shop	    	    |");
	puts("+===========================+");
	printf("\n > ");
}

void main() {

	uint32_t choice;
	char name[0x5];
	
	init_shop();

	puts("Please enter your name to enter our shop:");
	
	if (fgets(name, sizeof(name), stdin) == NULL) { 
		exit(-1);
	}  
	
	printf("We hope you enjoy our shop ");
	printf(name); // this is only good for leak, not useful for arb write


	while(1) {
		puts("\n\nWelcome to my shop!\nYou can see my offers below.\n");
		print_shop_menu(); // print shop

		choice = get_num();

		switch(choice) {

			case(1):
				view_items(); // prints items
				break;
			case(2):
				buy_items();
				break;
			case(3):
				add_item();
				break;
			case(4):
				use_special_item();
				break;
			case(5):
				exit(1);
			default:
				puts("Invalid Option.");
		}
	}
}
