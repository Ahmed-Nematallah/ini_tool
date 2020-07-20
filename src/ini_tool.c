
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "ini.h"

typedef enum {
	MODE_NONE, 
	MODE_MODIFY,
	MODE_READ, 
	MODE_DELETE, 
	MODE_ADD, 
	MODE_SECTION_RENAME, 
	MODE_SECTION_ADD, 
	MODE_SECTION_DELETE, 
	MODE_SECTION_READ
} MODE;

void print_usage(char * file_name);

int main (int argc, char ** argv) {

	MODE mode = MODE_NONE;

	char * fn;
	char * section;
	char * key;
	char * value;

	if (argc < 4) {
		print_usage(argv[0]);
		return 1;
	}

	fn = argv[2];

	if (strncmp(argv[1], "MODIFY", strlen("MODIFY")) == 0) {
		mode = MODE_MODIFY;
		if (argc == 5) {
			section = NULL;
			key = argv[3];
			value = argv[4];
		} else if (argc == 6) {
			section = argv[3];
			key = argv[4];
			value = argv[5];
		} else {
			print_usage(argv[0]);
			return 1;
		}
	} else if (strncmp(argv[1], "READ", strlen("READ")) == 0) {
		mode = MODE_READ;
		if (argc == 4) {
			section = NULL;
			key = argv[3];
		} else if (argc == 5) {
			section = argv[3];
			key = argv[4];
		} else {
			print_usage(argv[0]);
			return 1;
		}
	} else if (strncmp(argv[1], "DELETE", strlen("DELETE")) == 0) {
		mode = MODE_DELETE;
		if (argc == 4) {
			section = NULL;
			key = argv[3];
		} else if (argc == 5) {
			section = argv[3];
			key = argv[4];
		} else {
			print_usage(argv[0]);
			return 1;
		}
	} else if (strncmp(argv[1], "ADD", strlen("ADD")) == 0) {
		mode = MODE_ADD;
		if (argc == 5) {
			section = NULL;
			key = argv[3];
			value = argv[4];
		} else if (argc == 6) {
			section = argv[3];
			key = argv[4];
			value = argv[5];
		} else {
			print_usage(argv[0]);
			return 1;
		}
	} else if (strncmp(argv[1], "SECTION_RENAME", strlen("SECTION_RENAME")) == 0) {
		mode = MODE_SECTION_RENAME;
		if (argc == 5) {
			section = argv[3];
			value = argv[4];
		} else {
			print_usage(argv[0]);
			return 1;
		}
	} else if (strncmp(argv[1], "SECTION_ADD", strlen("SECTION_ADD")) == 0) {
		mode = MODE_SECTION_ADD;
		if (argc == 4) {
			section = argv[3];
		} else {
			print_usage(argv[0]);
			return 1;
		}
	} else if (strncmp(argv[1], "SECTION_DELETE", strlen("SECTION_DELETE")) == 0) {
		mode = MODE_SECTION_DELETE;
		if (argc == 4) {
			section = argv[3];
		} else {
			print_usage(argv[0]);
			return 1;
		}
	} else if (strncmp(argv[1], "SECTION_READ", strlen("SECTION_READ")) == 0) {
		mode = MODE_SECTION_READ;
		if (argc == 4) {
			section = argv[3];
		} else {
			print_usage(argv[0]);
			return 1;
		}
	} else {
		print_usage(argv[0]);
		return 1;
	}

	int ret;

	if (mode == MODE_MODIFY) {
		ret = ini_modify(section, key, value, fn);
	} else if (mode == MODE_DELETE) {
		ret = ini_delete(section, key, fn);
	} else if (mode == MODE_READ) {
		char returned_value[MAX_LINE_LEN + 1];
		ret = ini_read(section, key, returned_value, fn);

		if (ret == 0) {
			printf("%s\n", returned_value);
		}
	} else if (mode == MODE_ADD) {
		ret = ini_add(section, key, value, fn);
	} else if (mode == MODE_SECTION_RENAME) {
		ret = ini_rename_section(section, value, fn);
	} else if (mode == MODE_SECTION_ADD) {
		ret = ini_add_section(section, fn);
	} else if (mode == MODE_SECTION_DELETE) {
		ret = ini_delete_section(section, fn);
	} else if (mode == MODE_SECTION_READ) {
		FILE * fp = fopen(fn, "r");
		fseek(fp, 0L, SEEK_END);
		char returned_value[ftell(fp) + 1];
		fclose(fp);

		ret = ini_read_section(section, returned_value, fn);

		if (ret == 0) {
			printf("%s\n", returned_value);
		}
	} else {
		print_usage(argv[0]);
		return 1;
	}

	switch (ret) {
		case INI_SUCCESS:
			printf("Success\n");
			break;
		case INI_NOT_FOUND:
			printf("Selected key/section not found\n");
			break;
		case INI_NO_FILE:
			printf("Could not open INI file\n");
			break;
		case INI_NO_WRITE:
			printf("Could not open the file as writable\n");
			break;
		case INI_MAX_LINE_REACHED:
			printf("Maximum line length of %d reached, you can modify the value from the header file\n", MAX_LINE_LEN);
			break;
		case INI_NOWRITTEN:
			printf("Selected key/section not found\n");
			break;
		case INI_SECTION_EXISTS:
			printf("Section already exists\n");
			break;
		case INI_ALREADY_EXISTS:
			printf("Key already exists\n");
			break;
		
		default:
			printf("Failure!\n");
			break;
	}

	return ret;
}

void print_usage(char * file_name) {
	printf("INI Tool\n");
	printf("Tool for editing ini files using command line.\n");
	printf("Usage: \n");
	printf("\t%s MODIFY file_name.ini [section] key value\n", file_name);
	printf("\t\tModify an existing key in a section (optional) to a value\n");
	printf("\t%s ADD file_name.ini [section] key value\n", file_name);
	printf("\t\tAdd a new key in a section (optional) with a value (Do not use if key already exists)\n");
	printf("\t%s READ file_name.ini [section] key\n", file_name);
	printf("\t\tRead the value of a key in a section (optional)\n");
	printf("\t%s DELETE file_name.ini [section] key\n", file_name);
	printf("\t\tDelete a key in a section (optional)\n");
	printf("\t%s SECTION_RENAME file_name.ini section new_name\n", file_name);
	printf("\t\tRename a section\n");
	printf("\t%s SECTION_ADD file_name.ini section\n", file_name);
	printf("\t\tAdd a new section\n");
	printf("\t%s SECTION_DELETE file_name.ini section\n", file_name);
	printf("\t\tDelete a section\n");
	printf("\t%s SECTION_READ file_name.ini section\n", file_name);
	printf("\t\tRead keys in a section\n");
}
