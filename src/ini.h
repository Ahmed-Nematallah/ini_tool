
#ifndef __INI_H__
#define __INI_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#define MAX_LINE_LEN 1024

#define IS_WHITESPACE(x) ((x == '\n') || (x == '\r') || (x == '\t') || (x == '\v') || (x == ' ') || (x == '\f'))


int ini_modify(FILE ** fp, char * section, char * key, char * value, char * fn);
int ini_read(FILE ** fp, char * section, char * key, char * value);
int ini_delete(FILE ** fp, char * section, char * key, char * fn);
int ini_add(FILE ** fp, char * section, char * key, char * value, char * fn);





#endif
