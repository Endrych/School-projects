#include <stdio.h>
#include "help_function.h"
#include "status_code.h"

int main()
{
    unsigned char status_code_before = 63;
    int data_length_before = 1022;
    unsigned char * buffer;
    buffer = malloc(2* sizeof(char));
    printf("BEFORE\n_________________\n");
    printf("STATUS CODE: %d\n DATA LENGTH: %d\n", (int)status_code_before, data_length_before);
    convert_header_to_binary(status_code_before,data_length_before, buffer);
    printf("BUFFER: %d %d\n",(int)buffer[0], (int)buffer[1]);
    int data_length_after;
    unsigned char status_code_after;
    convert_header_from_binary(&status_code_after,&data_length_after ,buffer);
    printf("AFTER\n_________________\n");
    printf("STATUS CODE: %d\n DATA LENGTH: %d\n", (int)status_code_after, data_length_after);
    
    return 0;
}