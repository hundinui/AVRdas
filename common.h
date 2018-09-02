#ifndef _COMMON_H_
#define _COMMON_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <stdbool.h>

#define TABLE_SIZE 116

#ifndef _MSC_VER
char *strupr(char *str);
char *strlwr(char *str);
#endif

#endif
