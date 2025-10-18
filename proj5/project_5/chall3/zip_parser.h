#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h> 
#include <stdint.h>
#include <unistd.h>
#include <assert.h>

typedef struct head {
	uint16_t num_files;
	uint32_t centdir_size;
	uint32_t centdir_offset;
} Head;

typedef struct centdir {
	uint32_t comp_size;
	uint32_t filerecord_offset;
	uint16_t filename_len;
	uint16_t extrafield_len;
} Centdir;

typedef struct dataentry {
	unsigned char *comp_data;
	uint32_t comp_size;
	uint16_t extrafield_len;
} Dataentry;

extern int parse_data(char *buffer, Head *header, Centdir *entrylist, Dataentry *data);
extern int parse_centdir(char *buffer, Head *header, Centdir *entrylist);
extern int parse_head(char *buffer, Head *header);
extern int init(unsigned int len);
extern unsigned int file_length;

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
