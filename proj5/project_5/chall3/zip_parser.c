#include "zip_parser.h"

unsigned int file_length;
int is_init = 0;

Head *header = NULL;
Centdir *entrylist = NULL;
Dataentry *data = NULL;

int parse_data(char *buffer, Head *header, Centdir *entrylist, Dataentry *data) {
	int offset, len, i;

    if (header == NULL || entrylist == NULL || data == NULL || is_init == 0) {
        puts("Error detected");
        return -1;
    }

	for (i = 0; i < header->num_files; i++) {
        if (entrylist[i].filerecord_offset + 18 > file_length) {
            puts("Error detected");
            return -1;
        }

		memcpy(&data[i].extrafield_len, &buffer[entrylist[i].filerecord_offset + 28], 
                sizeof(short));
		memcpy(&data[i].comp_size, &buffer[entrylist[i].filerecord_offset + 18], sizeof(int));
		offset = 30 + entrylist[i].filerecord_offset + entrylist[i].filename_len + 
            data[i].extrafield_len;

		len = data[i].comp_size;
        len = MIN(len, 512);

		data[i].comp_data = malloc(len);

        if ((offset + len) >= file_length) {
            puts("Error detected");
            return -1;
        }

		memcpy(data[i].comp_data, &buffer[offset], len);

        printf("[FILE-%d Data]\n", i);
        printf("\tExtrafield Length: %u\n", data[i].extrafield_len);
        printf("Compressed Data:\n");
        for (int j = 0; j < len; j++) {
            printf("\\x%02X", data[i].comp_data[j]);
        }
	}
    return 0;
}

int parse_centdir(char *buffer, Head *header, Centdir *entrylist) {
	int offset = 0;

    if (header == NULL || entrylist == NULL || is_init == 0) {
        puts("Error detected");
        return -1;
    }

	for (int i = 0; i < header->num_files; i++) {
        if ((offset + header->centdir_offset + 20) > file_length) {
            puts("Error detected");
            return -1;
        }

		memcpy(&entrylist[i].comp_size, &buffer[offset + header->centdir_offset + 20], sizeof(int));
		memcpy(&entrylist[i].filerecord_offset, &buffer[offset + header->centdir_offset + 42], 
                sizeof(int));

		memcpy(&entrylist[i].filename_len, &buffer[offset + header->centdir_offset + 28], 
                sizeof(short));
		memcpy(&entrylist[i].extrafield_len, &buffer[offset + header->centdir_offset + 30], 
                sizeof(short));

		offset += 46 + entrylist[i].extrafield_len + entrylist[i].filename_len;

        printf("[FILE-%d Metadata]\n", i);
        printf("\tCompressed Size: %u\n", entrylist[i].comp_size);
        printf("\tFilerecord Offset: %u\n", entrylist[i].filerecord_offset);
        printf("\tfilename length: %hu\n", entrylist[i].filename_len);
        printf("\tExtrafield length: %hu\n\n", entrylist[i].extrafield_len);
	}

    return 0;
}

int parse_head(char *buffer, Head *header) {
	char magic[] = {0x50, 0x4B, 0x05, 0x06};
	int header_offset;

    if (header == NULL || is_init == 0 || file_length < 8) {
        puts("Error detected");
        return -1;
    }

	for (int i = 0; i < file_length-4; i++) {
		if (memcmp(&buffer[i], magic, 4) == 0) {
			header_offset = i;
			memcpy(&header->num_files, &buffer[header_offset+10], sizeof(short));
			memcpy(&header->centdir_size, &buffer[header_offset+12], sizeof(int));
			memcpy(&header->centdir_offset, &buffer[header_offset+16], sizeof(int));
		}
	}
    printf("num_files: %hu\n", header->num_files);
    printf("centdir_size: %u\n", header->centdir_size);
    printf("centdir_offset: %u\n\n", header->centdir_offset);

	return 0;
}

int init(unsigned int len) {
    file_length = len;
    is_init = 1;
    return 0;
}
