#ifndef DISASM_H
#define DISASM_H
#include "common.h"

#define ZSEARCH_NONE 0
#define ZSEARCH_FIRST 1
#define ZSEARCH_SECOND 2

extern int zsearch;
extern const char *registers[];

char *(*instruction_disasm[TABLE_SIZE])(unsigned short opcode, unsigned short next);

#endif