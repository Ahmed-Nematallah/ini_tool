
#include "ini.h"


static int read_line(FILE * fp, char * buffer) {
	int i = 0;
	int c;

	while (i < MAX_LINE_LEN) {
		c = fgetc(fp);

		if (c == '\n') {
			buffer[i] = 0;
			break;
		} else if (c != EOF) {
			buffer[i] = c;
		} else if (i == 0) {
			return -1;
		} else {
			buffer[i] = 0;
			break;
		}

		i++;
	}

	if (i == MAX_LINE_LEN)
		return -2;

	return 0;
}

static int count_whitespaces(char * buf) {
	int i = 0;
	while (IS_WHITESPACE(buf[i]))
		i++;
	
	return i;
}

static int section_match(char * section, char * buffer) {
	int whitespace_count = count_whitespaces(buffer);

	if (buffer[whitespace_count] == '[' && 
	    strncmp(section, buffer, strlen(section)) && 
	    buffer[whitespace_count + strlen(section) + 1] == ']')
		return 1;
	else if (buffer[whitespace_count] == '[')
		return 0;
	else
		return -1;
}

static int read_key_value(char * buffer, char * key, char * value) {
	int i, j;

	int whitespace_count = count_whitespaces(buffer);

	for (i = whitespace_count; i < MAX_LINE_LEN; i++) {
		if (buffer[i] != '\0' && buffer[i] != '=' && buffer[i] != ';') {
			key[i - whitespace_count] = buffer[i];
		} else if (buffer[i] == '\0') {
			return -1;
		} else if (buffer[i] == ';') {
			return -2;
		} else {
			key[i - whitespace_count] = 0;
			break;
		}
	}

	if (i == MAX_LINE_LEN)
		return -4;

	for (j = i - whitespace_count - 1; j >= 0; j--) {
		if (!IS_WHITESPACE(key[j])) {
			break;
		}

		key[j] = 0;
	}

	buffer += i + 1;

	whitespace_count = count_whitespaces(buffer);

	for (i = whitespace_count; i < MAX_LINE_LEN; i++) {
		if (buffer[i] != '\0' && buffer[i] != ';') {
			value[i - whitespace_count] = buffer[i];
		} else if (buffer[i] == '\0') {
			value[i - whitespace_count] = 0;
			break;
		} else if (buffer[i] == ';') {
			value[i - whitespace_count] = 0;
			break;
		}
	}

	if (i == MAX_LINE_LEN)
		return -4;

	for (j = i - whitespace_count - 1; j >= 0; j--) {
		if (!IS_WHITESPACE(value[j])) {
			break;
		}

		value[j] = 0;
	}

	return 0;
}

int ini_modify(FILE ** fp, char * section, char * key, char * value, char * fn) {
	char buffer[MAX_LINE_LEN + 2];
	char line_key[MAX_LINE_LEN + 1];
	char line_value[MAX_LINE_LEN + 1];

	int loc = ftell(*fp);
	fseek(*fp, 0L, SEEK_END);
	int len = ftell(*fp);

	fseek(*fp, loc, SEEK_SET);

	char file_mem[len + MAX_LINE_LEN + 2];

	file_mem[0] = 0;

	int ret;

	bool in_section = false;
	bool written = false;

	while ((ret = read_line(*fp, buffer)) == 0) {
		if (section) {
			int sec_mat = section_match(section, buffer);
			if (sec_mat == 1) {
				in_section = true;
			} else if (sec_mat == 0) {
				in_section = false;
			}
		}

		if (in_section || section == NULL) {
			if (read_key_value(buffer, line_key, line_value) == 0) {
				if (strcmp(key, line_key) == 0) {
					sprintf(buffer, "%s = %s", key, value);
					written = true;
				}
			}
		}

		strcat(buffer, "\n");
		strcat(file_mem, buffer);
	}

	if (written == false) {
		printf("Could not find key in section.\n");
		return -6;
	}

	fclose(*fp);

	*fp = fopen(fn, "w");

	if (*fp == NULL) {
		printf("Could not reopen file in write mode.\n");
		return -5;
	}

	fputs(file_mem, *fp);
	
	exit(0);

	return ret;
}

int ini_read(FILE ** fp, char * section, char * key, char * value) {
	char buffer[MAX_LINE_LEN + 1];

	char line_key[MAX_LINE_LEN + 1];
	char line_value[MAX_LINE_LEN + 1];

	int ret;

	bool in_section = false;

	while ((ret = read_line(*fp, buffer)) == 0) {
		if (section) {
			int sec_mat = section_match(section, buffer);
			if (sec_mat == 1) {
				in_section = true;
			} else if (sec_mat == 0) {
				in_section = false;
			}
		}

		if (in_section || section == NULL) {
			if (read_key_value(buffer, line_key, line_value) == 0) {
				if (strcmp(key, line_key) == 0) {
					strncpy(value, line_value, MAX_LINE_LEN + 1);
					break;
				}
			}
		}
	}
	
	return ret;
}

int ini_delete(FILE ** fp, char * section, char * key, char * fn) {
	char buffer[MAX_LINE_LEN + 2];
	char line_key[MAX_LINE_LEN + 1];
	char line_value[MAX_LINE_LEN + 1];

	int loc = ftell(*fp);
	fseek(*fp, 0L, SEEK_END);
	int len = ftell(*fp);

	fseek(*fp, loc, SEEK_SET);

	char file_mem[len + MAX_LINE_LEN + 2];

	file_mem[0] = 0;

	int ret;

	bool in_section = false;
	bool written = false;

	while ((ret = read_line(*fp, buffer)) == 0) {
		if (section) {
			int sec_mat = section_match(section, buffer);
			if (sec_mat == 1) {
				in_section = true;
			} else if (sec_mat == 0) {
				in_section = false;
			}
		}

		if (in_section || section == NULL) {
			if (read_key_value(buffer, line_key, line_value) == 0) {
				if (strcmp(key, line_key) == 0) {
					written = true;
					continue;
				}
			}
		}

		strcat(buffer, "\n");
		strcat(file_mem, buffer);
	}

	if (written == false) {
		printf("Could not find key in section.\n");
		return -6;
	}

	fclose(*fp);

	*fp = fopen(fn, "w");

	if (*fp == NULL) {
		printf("Could not reopen file in write mode.\n");
		return -5;
	}

	fputs(file_mem, *fp);
	
	exit(0);

	return ret;
}

int ini_add(FILE ** fp, char * section, char * key, char * value, char * fn) {

}
