#ifndef HELP_FUNCTION_H
#define HELP_FUNCTION_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "status_code.h"
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

void convert_header_to_binary(unsigned char number_code, int data_length, unsigned char * binary);
void convert_header_from_binary(unsigned char * number_code, int * data_length, unsigned char * binary);
int prepare_first_buffer(unsigned char number_code, char* filename, char * buffer, int buffer_length, FILE * input);
int set_filename_to_binary(char * filename, char* buffer);
char * get_filename_from_binary(char * buffer);
int write_from_file_to_binary(FILE * file_descriptor, char * buffer, int start, int max_length);
unsigned long hash(unsigned char *str);
void generate_path_to_file(char * path);

#endif