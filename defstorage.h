#ifndef _DEFS_H_
#define _DEFS_H_

#define TYPE_DEF 1
#define TYPE_EQU 2
#define TYPE_MACRO 4
#define TYPE_LABEL 8

typedef struct definition definition;

typedef struct definition {
	char *name;
	char *def;
	int size; // used for macros
	int sizeused; // again, for macros
	int bytes; // no of words, for macro
	int type;
	bool active;
	definition *next;
	definition *prev;
} definition;

int getNumBytes();

void addNumBytes(int n);

void setNumBytes(int n);

void zeroNumBytes();

char *ppStoreGetDef(char * name, int *type, int *macrobytes);

char *ppStoreGetName(char *def, int *type, int *macrobytes);

void ppStoreInit();

int ppStoreDef(char * name, char * definition, int type);

void ppStorePrint();

void ppStorePrintMacros();

int ppMacroBegin(char * name);

void ppMacroAppend(char * name, char * txt);

#endif
