#ifndef _IHEX_H_
#define _IHEX_H_

#include <stdio.h>
/* this loads an intel hex file into the memory[] array */
int load_file(char *filename);

/* this writes a part of memory[] to an intel hex file */
void save_file(char *command);

/* this is used by load_file to get each line of intex hex */
int parse_hex_line(char *theline, int bytes[], int *addr, int *num, int *code);

/* this does the dirty work of writing an intel hex file */
/* caution, static buffering is used, so it is necessary */
/* to call it with end=1 when finsihed to flush the buffer */
/* and close the file */
void hexout(FILE *fhex, int byte, int memory_location, int end);

extern unsigned char memory[65536];
extern int total;
extern int maxaddr;

#endif