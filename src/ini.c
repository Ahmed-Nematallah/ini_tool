
#include "ini.h"

typedef enum {
	SECTION_MATCH, 
	SECTION_NOMATCH, 
	SECTION_NOSECTION
} section_match_return_codes;

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
			return INI_EOF_REACHED;
		} else {
			buffer[i] = 0;
			break;
		}

		i++;
	}

	if (i == MAX_LINE_LEN)
		return INI_MAX_LINE_REACHED;

	return INI_SUCCESS;
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
	    strncmp(section, buffer + whitespace_count + 1, strlen(section)) == 0 && 
	    buffer[whitespace_count + strlen(section) + 1] == ']') {
		return SECTION_MATCH;
	} else if (buffer[whitespace_count] == '[') {
		return SECTION_NOMATCH;
	} else {
		return SECTION_NOSECTION;
	}
}

static int read_key_value(char * buffer, char * key, char * value) {
	int i, j;

	int whitespace_count = count_whitespaces(buffer);

	for (i = whitespace_count; i < MAX_LINE_LEN; i++) {
		if (buffer[i] != '\0' && buffer[i] != '=' && buffer[i] != ';') {
			key[i - whitespace_count] = buffer[i];
		} else if (buffer[i] == '\0') {
			return INI_NOTKEYVAL;
		} else if (buffer[i] == ';') {
			return INI_COMMENT;
		} else {
			key[i - whitespace_count] = 0;
			break;
		}
	}

	if (i == MAX_LINE_LEN)
		return INI_MAX_LINE_REACHED;

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
		return INI_MAX_LINE_REACHED;

	for (j = i - whitespace_count - 1; j >= 0; j--) {
		if (!IS_WHITESPACE(value[j])) {
			break;
		}

		value[j] = 0;
	}

	return INI_SUCCESS;
}

int ini_modify(char * section, char * key, char * value, char * fn) {
	char buffer[MAX_LINE_LEN + 2];
	char line_key[MAX_LINE_LEN + 1];
	char line_value[MAX_LINE_LEN + 1];

	FILE * fp = fopen(fn, "r");

	if (fp == NULL) return INI_NO_FILE;

	fseek(fp, 0L, SEEK_END);
	int len = ftell(fp);

	rewind(fp);

	char file_mem[len + MAX_LINE_LEN + 2];

	file_mem[0] = 0;

	int ret;

	bool in_section = false;
	bool written = false;

	while ((ret = read_line(fp, buffer)) == INI_SUCCESS) {
		if (section) {
			int sec_mat = section_match(section, buffer);
			if (sec_mat == SECTION_MATCH) {
				in_section = true;
				goto INI_MODIFY_CONT;
			} else if (sec_mat == SECTION_NOMATCH) {
				in_section = false;
				goto INI_MODIFY_CONT;
			}
		}

		if (in_section || section == NULL) {
			int kvret = read_key_value(buffer, line_key, line_value);
			if (kvret == INI_SUCCESS) {
				if (strcmp(key, line_key) == 0) {
					sprintf(buffer, "%s = %s", key, value);
					written = true;
				}
			} else if (kvret == INI_MAX_LINE_REACHED) {
				fclose(fp);
				return INI_MAX_LINE_REACHED;
			}
			
		}

INI_MODIFY_CONT:
		strcat(buffer, "\n");
		strcat(file_mem, buffer);
	}

	fclose(fp);

	if (written == false) {
		return INI_NOWRITTEN;
	}

	FILE * fout = fopen(fn, "w");

	if (fout == NULL) {
		return INI_NO_WRITE;
	}

	fputs(file_mem, fout);

	fclose(fout);

	if (ret == INI_EOF_REACHED)
		ret = INI_SUCCESS;

	return ret;
}

int ini_read(char * section, char * key, char * value, char * fn) {
	FILE * fp = fopen(fn, "r");

	if (fp == NULL) return INI_NO_FILE;

	char buffer[MAX_LINE_LEN + 1];

	char line_key[MAX_LINE_LEN + 1];
	char line_value[MAX_LINE_LEN + 1];

	int ret;

	bool in_section = false;

	rewind(fp);

	bool found = false;

	while ((ret = read_line(fp, buffer)) == INI_SUCCESS) {
		if (section) {
			int sec_mat = section_match(section, buffer);
			if (sec_mat == SECTION_MATCH) {
				in_section = true;
				goto INI_READ_CONT;
			} else if (sec_mat == SECTION_NOMATCH) {
				in_section = false;
				goto INI_READ_CONT;
			}
		}

		if (in_section || section == NULL) {
			int kvret = read_key_value(buffer, line_key, line_value);
			if (kvret == INI_SUCCESS) {
				if (strcmp(key, line_key) == 0) {
					found = true;
					strncpy(value, line_value, MAX_LINE_LEN + 1);
					value[strlen(line_value)] = 0;
					break;
				}
			} else if (kvret == INI_EOF_REACHED) {
				fclose(fp);
				return INI_EOF_REACHED;
			}
		}

INI_READ_CONT:
		;
	}

	if (!found)
		ret = INI_NOT_FOUND;

	if (ret == INI_EOF_REACHED)
		ret = INI_SUCCESS;

	fclose(fp);

	return ret;
}

int ini_delete(char * section, char * key, char * fn) {
	FILE * fp = fopen(fn, "r");

	if (fp == NULL) return INI_NO_FILE;

	char buffer[MAX_LINE_LEN + 2];
	char line_key[MAX_LINE_LEN + 1];
	char line_value[MAX_LINE_LEN + 1];

	fseek(fp, 0L, SEEK_END);
	int len = ftell(fp);

	rewind(fp);

	char file_mem[len + MAX_LINE_LEN + 2];

	file_mem[0] = 0;

	int ret;

	bool in_section = false;
	bool written = false;

	while ((ret = read_line(fp, buffer)) == INI_SUCCESS) {
		if (section) {
			int sec_mat = section_match(section, buffer);
			if (sec_mat == SECTION_MATCH) {
				in_section = true;
				goto INI_DELETE_CONT;
			} else if (sec_mat == SECTION_NOMATCH) {
				in_section = false;
				goto INI_DELETE_CONT;
			}
		}

		if (in_section || section == NULL) {
			int kvret = read_key_value(buffer, line_key, line_value);
			if (kvret == 0) {
				if (strcmp(key, line_key) == 0) {
					written = true;
					continue;
				}
			} else if (kvret == INI_EOF_REACHED) {
				fclose(fp);
				return INI_EOF_REACHED;
			}
		}

INI_DELETE_CONT:
		strcat(buffer, "\n");
		strcat(file_mem, buffer);
	}

	fclose(fp);

	if (written == false) {
		return INI_NOWRITTEN;
	}

	FILE * fout = fopen(fn, "w");

	if (fout == NULL) {
		return INI_NO_WRITE;
	}

	fputs(file_mem, fout);

	fclose(fout);

	if (ret == INI_EOF_REACHED)
		ret = INI_SUCCESS;

	return ret;
}

int ini_add(char * section, char * key, char * value, char * fn) {
	char buffer[MAX_LINE_LEN + 2];
	char line_key[MAX_LINE_LEN + 1];
	char line_value[MAX_LINE_LEN + 1];

	FILE * fp = fopen(fn, "r");

	if (fp == NULL) return INI_NO_FILE;

	fseek(fp, 0L, SEEK_END);
	int len = ftell(fp);

	rewind(fp);

	char file_mem[len + MAX_LINE_LEN + 2];

	file_mem[0] = 0;

	int ret;

	bool in_section = false;
	bool written = false;
	bool exists = false;

	while ((ret = read_line(fp, buffer)) == INI_SUCCESS) {
		if (section) {
			int sec_mat = section_match(section, buffer);
			if (sec_mat == SECTION_MATCH) {
				in_section = true;
				goto INI_ADD_CONT;
			} else if (sec_mat == SECTION_NOMATCH) {
				if (in_section && !exists) {
					char buffer2[MAX_LINE_LEN + 1];
					sprintf(buffer2, "%s = %s\n\n", key, value);
					strcat(file_mem, buffer2);
					written = true;
				}
				in_section = false;
				goto INI_ADD_CONT;
			}
		}

		if (in_section || section == NULL) {
			int kvret = read_key_value(buffer, line_key, line_value);
			if (kvret == INI_SUCCESS) {
				if (strcmp(key, line_key) == 0) {
					exists = true;
				}
			} else if (kvret == INI_MAX_LINE_REACHED) {
				fclose(fp);
				return INI_MAX_LINE_REACHED;
			}
			
		}

INI_ADD_CONT:
		strcat(buffer, "\n");
		strcat(file_mem, buffer);
	}

	fclose(fp);

	if (exists) {
		return INI_ALREADY_EXISTS;
	}

	if (!exists && (!section || in_section)) {
		sprintf(buffer, "%s = %s\n\n", key, value);
		written = true;
		strcat(file_mem, buffer);
	}

	if (section && !written) {
		return INI_NOT_FOUND;
	}

	FILE * fout = fopen(fn, "w");

	if (fout == NULL) {
		return INI_NO_WRITE;
	}

	fputs(file_mem, fout);

	fclose(fout);

	if (ret == INI_EOF_REACHED)
		ret = INI_SUCCESS;

	return ret;
}

int ini_rename_section(char * section, char * new_name, char * fn) {
	FILE * fp = fopen(fn, "r");

	if (fp == NULL) return INI_NO_FILE;

	char buffer[MAX_LINE_LEN + 2];

	fseek(fp, 0L, SEEK_END);
	int len = ftell(fp);

	rewind(fp);

	char file_mem[len + MAX_LINE_LEN + 2];

	file_mem[0] = 0;

	int ret;

	bool written = false;

	while ((ret = read_line(fp, buffer)) == INI_SUCCESS) {
		int sec_mat = section_match(section, buffer);
		if (sec_mat == SECTION_MATCH) {
			sprintf(buffer, "[%s]", new_name);
			written = true;
		}

		strcat(buffer, "\n");
		strcat(file_mem, buffer);
	}

	fclose(fp);

	if (written == false) {
		return INI_NOWRITTEN;
	}

	FILE * fout = fopen(fn, "w");

	if (fout == NULL) {
		return INI_NO_WRITE;
	}

	fputs(file_mem, fout);

	fclose(fout);

	if (ret == INI_EOF_REACHED)
		ret = INI_SUCCESS;

	return ret;
}

int ini_read_section(char * section, char * returned_data, char * fn) {
	FILE * fp = fopen(fn, "r");

	if (fp == NULL) return INI_NO_FILE;

	char buffer[MAX_LINE_LEN + 1];

	char line_key[MAX_LINE_LEN + 1];
	char line_value[MAX_LINE_LEN + 1];

	int ret;

	bool in_section = false;

	rewind(fp);

	bool found = false;

	while ((ret = read_line(fp, buffer)) == INI_SUCCESS) {
		int sec_mat = section_match(section, buffer);
		if (sec_mat == SECTION_MATCH) {
			in_section = true;
			found = true;
			goto INI_READ_SECTION_CONT;
		} else if (sec_mat == SECTION_NOMATCH) {
			in_section = false;
			goto INI_READ_SECTION_CONT;
		}

		if (in_section) {
			int kvret = read_key_value(buffer, line_key, line_value);
			if (kvret == INI_SUCCESS) {
				strcat(returned_data, line_key);
				strcat(returned_data, "\n");
			} else if (kvret == INI_EOF_REACHED) {
				fclose(fp);
				return INI_EOF_REACHED;
			}
		}

INI_READ_SECTION_CONT:
		;
	}

	if (!found)
		ret = INI_NOT_FOUND;

	if (ret == INI_EOF_REACHED)
		ret = INI_SUCCESS;

	fclose(fp);

	return ret;
}

int ini_delete_section(char * section, char * fn) {
	FILE * fp = fopen(fn, "r");

	if (fp == NULL) return INI_NO_FILE;

	char buffer[MAX_LINE_LEN + 1];

	fseek(fp, 0L, SEEK_END);
	int len = ftell(fp);
	
	rewind(fp);

	char file_mem[len + 1];

	file_mem[0] = 0;

	int ret;

	bool in_section = false;
	bool written = false;

	while ((ret = read_line(fp, buffer)) == INI_SUCCESS) {
		int sec_mat = section_match(section, buffer);
		if (sec_mat == SECTION_MATCH) {
			in_section = true;
			written = true;
		} else if (sec_mat == SECTION_NOMATCH) {
			in_section = false;
		}

		if (in_section) {
			continue;
		}

		strcat(buffer, "\n");
		strcat(file_mem, buffer);
	}

	fclose(fp);

	if (written == false) {
		return INI_NOWRITTEN;
	}

	FILE * fout = fopen(fn, "w");

	if (fout == NULL) {
		return INI_NO_WRITE;
	}

	fputs(file_mem, fout);

	if (ret == INI_EOF_REACHED)
		ret = INI_SUCCESS;

	fclose(fout);

	return ret;
}

int ini_add_section(char * section, char * fn) {
	FILE * fp = fopen(fn, "r");

	if (fp == NULL) return INI_NO_FILE;

	char buffer[MAX_LINE_LEN + 1];

	int ret;

	rewind(fp);

	bool found = false;

	while ((ret = read_line(fp, buffer)) == INI_SUCCESS) {
		int sec_mat = section_match(section, buffer);
		if (sec_mat == SECTION_MATCH) {
			found = true;
			break;
		}
	}

	fclose(fp);

	if (found) {
		ret = INI_SECTION_EXISTS;
	} else {
		fp = fopen(fn, "a");
		fprintf(fp, "\n[%s]\n", section);
		fclose(fp);
		ret = INI_SUCCESS;
	}

	return ret;
}
