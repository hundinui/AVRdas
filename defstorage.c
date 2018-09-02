// definition storage
#include "common.h"
#include "avrasm.h"
#include "defstorage.h"

definition *start;
static int bytes;

int getNumBytes() {
	return bytes;
}

void addNumBytes(int n) {
	bytes += n;
	//printf("bytes: %i ", bytes);
}

void setNumBytes(int n) {
	bytes = n;
	//printf("bytes: %i ", bytes);
}

void zeroNumBytes() {
	bytes = 0;
}

definition *getLatest() {
	definition *d = start;
	while (d->next != NULL) {
		d = d->next;
	}
	return d;
}

definition *getTop() {
	definition *d = getLatest();
	if (d == NULL) {
		return d;
	}
	return d->next;
}

definition *ppStoreGet(char *name) {
	definition *d = start;
	while (d != NULL) {
		if (strcmp(d->name, name) == 0) {
			return d;
		}
		d = d->next;
	}
	return NULL;
}

char *ppStoreGetName(char *def, int *type, int *macrobytes) {
	definition *d = start;
	while (d != NULL) {
		if (strcmp(d->def, def) == 0) {
			if (type != NULL) {
				*type = d->type;
			}
			if (d->type == TYPE_MACRO && macrobytes != NULL) {
				*macrobytes = d->bytes;
			}
			return d->name;
		}
		d = d->next;
	}
	return NULL;
}

char *ppStoreGetDef(char *name, int *type, int *macrobytes) {
	definition *d = start;
	while (d != NULL) {
		if (strcmp(d->name, name) == 0) {
			if (type != NULL) {
				*type = d->type;
			}
			if (d->type == TYPE_MACRO && macrobytes != NULL) {
				*macrobytes = d->bytes;
			}
			return d->def;
		}
		d = d->next;
	}
	return NULL;
}

void ppStoreInit() {
	bytes = 0;
	start = (definition *)malloc(sizeof(definition) + 1);
	start->name = (char *)malloc(sizeof(char) * strlen("OBMASM_VERSION") + 1);
	sprintf(start->name, "%s", "OBMASM_VERSION");
	start->def = (char *)malloc(sizeof(char) * strlen("0.9") + 1); // todoo
	sprintf(start->def, "%s", "0.9");
	start->next = NULL;
	start->prev = NULL;
	start->active = true;
	start->type = TYPE_EQU;
}

int ppStoreDef(char *name, char *def, int type) {
	definition *d, *n;
	if (name == NULL || def == NULL) {
		printf("Error parsing definition!\n");
		return 1;
	}
	d = getLatest();
	n = d->next;
	n = (definition *)malloc(sizeof(definition) + 1);
	n->name = (char *)malloc(sizeof(char) * strlen(name) + 1);
	memcpy(n->name, name, strlen(name) + 1);
	n->def = (char *)malloc(sizeof(char) * strlen(def) + 1);
	memcpy(n->def, def, strlen(def) + 1);
	n->type = type;
	n->active = true;
	n->size = -1; // not a macro so dont care
	n->next = NULL;
	d->next = n;
	n->prev = d;
	return 0;
}

void ppStorePrint() {
	definition *d = start;
	while (d != NULL) {
		printf("%s - %s - %i\n", d->name, d->def, d->type);
		d = d->next;
	}
}

void ppStorePrintMacros() {
	definition *d = start;
	while (d != NULL) {
		if (d->type == TYPE_MACRO) {
			printf("%s - %s - %i\n", d->name, d->def, d->bytes);
		}
		d = d->next;
	}
}


int ppMacroBegin(char *name) {
	definition *d, *n;
	if (name == NULL) {
		printf("Macro name cannot be NULL!\n");
		return 1;
	}
	d = getLatest();
	n = d->next;
	n = (definition *)malloc(sizeof(definition) + 1);
	int len = strlen(name);
	n->name = (char *)malloc(sizeof(char) * len + 1);
	memcpy(n->name, name, len + 1);
	n->def = (char *)malloc(sizeof(char) * 1024 + 1); // lets allocate in chuncks as needed
	n->size = 1024;
	n->sizeused = 0;
	n->bytes = 0;
	n->def[0] = '\0';
	n->type = TYPE_MACRO;
	n->active = true;
	n->next = NULL;
	d->next = n;
	n->prev = d;
	return 0;
}

void ppMacroAppend(char *name, char *txt) {
	definition *d = ppStoreGet(name);
	char *ptr;// , *tmp;
	if (d != NULL && txt != NULL) {
		if ((strlen(txt) + d->sizeused) > (unsigned int)d->size) {
			d->def = (char *)realloc(d->def, sizeof(char) * d->size + 1025);
			d->size += 1024;
		}
		strcat(d->def, txt);
		strcat(d->def, "\n");
		ptr = strtok(txt, " ,\n");
		strlwr(ptr);
		d->bytes += asmGetSize(ptr) * 2;
		printf("bytes: %i  ", d->bytes);
		//strcat(d->def, "\n");
		//addNumBytes(asmGetSize(ptr) * 2);
		
	}
}
