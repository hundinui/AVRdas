#include "common.h"

// strlwr is present on MSVC
#ifndef _MSC_VER
char *strlwr(char *str) {
	unsigned char *p = (unsigned char *)str;
	while (*p) {
		*p = tolower((unsigned char)*p);
		p++;
	}
	return str;
}

char *strupr(char *str) {
	unsigned char *p = (unsigned char *)str;
	while (*p) {
		*p = toupper((unsigned char)*p);
		p++;
	}
	return str;
}
#endif
