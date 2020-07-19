
#ifndef __INI_H__
#define __INI_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#define MAX_LINE_LEN 1024

#define IS_WHITESPACE(x) ((x == '\n') || (x == '\r') || (x == '\t') || (x == '\v') || (x == ' ') || (x == '\f'))


typedef enum {
	INI_SUCCESS = 0,
	INI_NOT_FOUND, 
	INI_NO_WRITE, 
	INI_MAX_LINE_REACHED, 
	INI_EOF_REACHED, 
	INI_NOWRITTEN, 
	INI_ALREADY_EXISTS, 
	INI_NOTKEYVAL, 
	INI_COMMENT
} return_codes;


int ini_modify(FILE * fp, char * section, char * key, char * value, char * fn);
int ini_read(FILE * fp, char * section, char * key, char * value);
int ini_delete(FILE * fp, char * section, char * key, char * fn);
int ini_add(FILE * fp, char * section, char * key, char * value, char * fn);

int ini_rename_section(FILE * fp, char * section, char * new_name, char * fn);
int ini_read_section(FILE * fp, char * section, char * returned_data);
int ini_delete_section(FILE * fp, char * section, char * fn);
int ini_add_section(FILE * fp, char * section, char * fn);




#endif
