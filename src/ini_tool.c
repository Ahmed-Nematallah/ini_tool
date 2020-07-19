
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
	MODE_ADD
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
	} else {
		print_usage(argv[0]);
		return 1;
	}

	FILE * fp = fopen(fn, "r");

	if (fp == NULL) {
		printf("Could not open file\n");
		return 1;
	}

	int ret;

	if (mode == MODE_MODIFY) {
		ret = ini_modify(fp, section, key, value, fn);

		if (ret == 0) {
			ret = 0;
			printf("Success!\n");
		} else {
			ret = 1;
			printf("Failure!\n");
		}
	} else if (mode == MODE_DELETE) {
		ret = ini_delete(fp, section, key, fn);

		if (ret == 0) {
			ret = 0;
			printf("Success!\n");
		} else {
			ret = 1;
			printf("Failure!\n");
		}
	} else if (mode == MODE_READ) {
		char returned_value[MAX_LINE_LEN + 1];
		ret = ini_read(fp, section, key, returned_value);

		if (ret == 0) {
			printf("Found!\n");
			printf("%s\n", returned_value);
		} else if (ret == -1) {
			printf("Not found\n");
			ret = 1;
		} else if (ret == -2) {
			ret = 1;
			printf("A line length exceeded the maximum line length of %d, this is set at compile time.\n", MAX_LINE_LEN);
		}
	} else if (mode == MODE_ADD) {
		ret = ini_add(fp, section, key, value, fn);
	} else {
		print_usage(argv[0]);
		return 1;
	}

	fclose(fp);

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
}
