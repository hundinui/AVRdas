#include "common.h"
#include "preprocessor.h"
#include "defstorage.h"
#include "avrasm.h"

static bool inMacro;
static char *macroName;

bool ppIsNumber(char *txt) {
	if (isdigit(txt[0]) && (txt[1] == 'X' || txt[1] == 'B' || isdigit(txt[1]))) {
		while (*txt != '\0') {
			if (isdigit(*txt++) == 0) {
				return false;
			}
		}
		return true;
	}
	return false;
}

int ppGetBase(char *txt) {
	int ret = 0;
	switch (txt[0]) {
	case('B'):
		if (isdigit(txt[3])) {
			ret = 2;
		}
		else {
			return 0;
		}
		break;
	case('X'):
		return 16;
	default:
		switch (txt[1]) {
		case('B'):
			if (isdigit(txt[3])) {
				ret = 2;
				goto twoplaces;
			}
			else {
				return 0;
			}
		case('X'):
			return 16;
		default:
			return 0;
		}
	}
	memmove(txt, txt + 1, strlen(txt));
	return ret;
twoplaces:
	memmove(txt, txt + 2, strlen(txt));
	return ret;
}

int ppCleanLine(char *line, int linenum, int pass) {
	char *ptr;
	char *ptr2;
	char *line2;
	line2 = strdup(line);
	int i = 0;
	int len = 0;
	if (line == NULL || line[0] == '\0') {
		printf("ERROR: Line %i is NULL!\n", linenum);
		return 0;
	}
	if (line[0] == '\t' || line[0] == ' ') { // get rid of leading tabs and spaces
		while (line[i] == '\t' || line[i] == ' ') {
			++i;
		}
		memmove(line, line + i, strlen(line + i));
	}
	strupr(line);
	if ((ptr = strchr(line, '\n')) != NULL) { // get rid of newlines
		*ptr = '\0';
	}
	i = 0;
	switch (line[0]) {
	case('\0'):
		break;
	case('#'):
	case('.'): // preprocessor switch
		if (pass == 2) { // SECOND PASS -----------------------------------------------------------------------------------------------------------------------
			ptr = strtok(line, " ");
			switch (ptr[1]) {
			case('E'):
				if (line[2] == 'N' && line[4] == 'M') { // .ENDMACRO
					inMacro = false;
					line[0] = '\0';
				}
				break;
			case('M'):
				if (line[2] == 'A') { // MACRO
					inMacro = true;
					line[0] = '\0';
				}
				break;
			case('O'):
				switch (line[2]) {
				case('R'): // .ORG
					;
					int codenumber;
					ptr = strtok(NULL, " ,");
					codenumber = strtol(ptr, NULL, ppGetBase(ptr));
					printf("org 0x%04X \n", codenumber);

					int bytes = getNumBytes();
					if (getNumBytes() != codenumber) {
						setNumBytes(codenumber);
					}

					break;
				}
			default:
				line[0] = '\0';
				break;
			}
		}
		else { // FIRST PASS -------------------------------------------------------------------------------------------------------------------------
			char *tok[4];
			ptr = strtok(line, " \t"); // lets split up the few first arguments so if they're needed i dont have to type the damn thing everywhere
			while (ptr != NULL && i != 4) {
				//printf("%s\n", ptr);
				tok[i] = ptr;
				ptr = strtok(NULL, " \t");
				++i;
			}

			switch (line[1]) {
			case('B'): // .BYTE
				break;
			case('C'):
				switch (line[5]) {
				case('S'): // .CSEGSIZE
					break;
				case('\0'): //.CSEG
					break;
				}
				break;
			case('D'):
				switch (line[2]) {
				case('B'): // .DB
					ptr2 = strtok(line2, " ,");
					int codenumber;
					while (ptr2 != NULL) {
						if ((codenumber = strtol(ptr2, NULL, ppGetBase(ptr2))) != 0L) {
//							char nrbuf[32];
							addNumBytes(1);
						}
						ptr2 = strtok(NULL, " ,");
					}
					break;
				case('E'): // .DEF
					ppStoreDef(tok[1], tok[3], TYPE_DEF);
					//ppStorePrint();
					break;
				case('S'): // .DSEG
					break;
				case('W'): // .DW
					break;
				case('D'): // .DD
					break;
				case('Q'): // .DQ
					break;
				}
				break;
			case('E'):
				switch (line[2]) {
				case('N'):
					switch (line[4]) {
					case('M'): // .ENDMACRO
						inMacro = false;
						break;
					case('I'): // .ENDIF
						break;
					}
					break;
				case('Q'): // .EQU
					ppStoreDef(tok[1], tok[3], TYPE_EQU);
					break;
				case('S'): // .ESEG
					break;
				case('X'): // .EXIT
					break;
				case('L'): // .ELIF
					break;
				case('R'): // .ERROR
					break;
				}
				break;
			case('I'): // .INCLUDE
				switch (line[2]) {
				case('N'): // .INCLUDE
					;
					FILE *in;
					char readbuf[1024];
					if (tok[1][0] == '"') {
						memmove(tok[1], tok[1] + 1, strlen(tok[1]));
					}
					if (tok[1][strlen(tok[1]) - 1] == '"') {
						tok[1][strlen(tok[1]) - 1] = '\0';
					}
					in = fopen(tok[1], "r");
					if (in == NULL) {
						strlwr(tok[1]);
						in = fopen(tok[1], "r");
						if (in == NULL) {
							printf("Error opening include file %s!\n", tok[1]);
							break;
						}
					}
					while (1) { // fugg
						++i;
						if (fgets(readbuf, 1024, in) != NULL) {
							ppCleanLine(readbuf, i, 1);
							//printf("%s\n", readbuf);
						}
						else {
							break;
						}
						//ppStorePrint();
					}
					fclose(in);
					break;
				case('F'): // .IF
					break;
				}
				break;
			case('L'):
				switch (line[5]) {
				case('\0'): // .LIST
					break;
				case('M'): // .LISTMAC
					break;
				}
				break;
			case('M'):
				switch (line[2]) {
				case('A'): // .MACRO
					inMacro = true;
					len = strlen(tok[1]);
					macroName = (char *)malloc(sizeof(char) * len);
					memcpy(macroName, tok[1], len + 1);
					ppMacroBegin(tok[1]);
					break;
				case('E'): // .MESSAGE
					printf("%i %s\n", i, line);
					break;
				}
				break;
			case('N'):
				switch (line[3]) {
				case('L'): // .NOLIST
					break;
				case('O'): // .NOOVERLAP
					break;
				}
				break;
			case('O'):
				switch (line[2]) {
				case('R'): // .ORG
					;
					int codenumber;
					codenumber = strtol(tok[1], NULL, ppGetBase(tok[1]));
					printf("org 0x%04X \n", codenumber);
					if (getNumBytes() != codenumber) {
						setNumBytes(codenumber);
					}
					break;
				case('V'): // .OVERLAP
					break;
				}
				break;
			case('S'): // .SET
				break;
			case('U'): // .UNDEF
				break;
			case('W'): // .WARNING
				break;
			}
		}
		break;
	case(';'): // comment line
		line[0] = '\0'; // no need to parse those :)
		break;
	default: // not a preprocessor switch
		if (pass == 1) {
			if (inMacro) {
				ppMacroAppend(macroName, line);

			}
			else {
				ptr = strtok(line, " ,");
				char *mac = NULL;
				int bytes = 0;
				if (ptr[strlen(ptr) - 1] == ':') { // its a label!
					printf("Label: %s", ptr);
					ptr[strlen(ptr) - 1] = '\0';
					char jmpstr[128];
					jmpstr[0] = '\0';
					sprintf(jmpstr, "%i\n", getNumBytes());
					printf(" %i\n", getNumBytes());
					ppStoreDef(ptr, jmpstr, TYPE_LABEL);
					//ppStorePrint();
				}
				else if ((mac = ppStoreGetDef(ptr, NULL, &bytes)) != NULL) {
					addNumBytes(bytes);
				}
				else {
					strlwr(ptr); // TODO!!!!!!!! make everything upper/lower case so no stupid conversions needed
					addNumBytes(asmGetSize(ptr) * 2);
				}
			}
		}
		else {
			if (inMacro) {
				line[0] = '\0';
				break;
			}
			char *tok[4];
			if ((ptr = strchr(line, ',')) != NULL) { // get rid of newlines
				*ptr = ' ';
			}
			ptr = strtok(line, " ,"); // lets split up the few first arguments so if they're needed i dont have to type the damn thing everywhere
			if (ptr[strlen(ptr) - 1] != ':') { // its not a label!
				strlwr(ptr);
				addNumBytes(asmGetSize(ptr) * 2);
				strupr(ptr); // TODO!!!!! fix this stupid shit
				while (i != 4) { // split to tokens
					//printf("%s\n", ptr);
					tok[i] = ptr;
					ptr = strtok(NULL, " ");
					++i;
				}
				for (i = 0; i < 4; ++i) { // check if token exists in storage
					char *tmp;
					int type;
					int macrobytes;
					int codenumber;
					if (tok[i] != NULL) {
						//codenumber = strtol(tok[i], NULL, ppGetBase(tok[i]));
						if (tok[i][0] == ';' || (tok[i][0] == '/' && tok[i][1] == '/')) {
							tok[i] = NULL;
						}
						else if ((tmp = ppStoreGetDef(tok[i], &type, &macrobytes)) != NULL) {
							if (type == TYPE_LABEL) { // if jump label then calculate offset
								//int offset1 = getNumBytes() - 2;
								int offset = (int)((atoi(tmp) - (getNumBytes() - 2)) * 0.5);
								sprintf(tok[i], "PC + %i", offset); // why did i have nul terminated strings here hmmm "PC + %i\0"
								//int offset2 = atoi(tmp);
								//printf("JUMP !!!!!! PC %i or %i\n", offset1, offset2);
							}
							else if (type == TYPE_MACRO) { // macro handling, fuck
								addNumBytes(macrobytes);
								char *dup = strdup(tmp); // strtok zero terminates after every delimiter
								// fucking hell, lets parse the macro again since there might be some jumps in there :@@@
								ptr = strtok(dup, " ");
								line[0] = '\0';
								while (ptr != NULL) {
									char *comma = strchr(ptr, ',');
									char *newline = strchr(ptr, '\n');
									if (comma != NULL) { // if leading comma then lets do it here and null it
										if ((comma - ptr + 1) == 0) {
											strcat(line, ",");
											memmove(ptr, comma, strlen(ptr));
											comma = NULL;
										}
										else {
											*comma = '\0';
										}
									}
									if (newline != NULL) {
										*newline = '\0';
									}
								fuckery:
									if ((tmp = ppStoreGetDef(ptr, &type, &macrobytes)) != NULL) {
										if ((codenumber = strtol(tmp, NULL, ppGetBase(tmp))) != 0L) {
											char nrbuf[32];
											//printf("converted %s to %i!----------\n", tmp, codenumber);
											sprintf(nrbuf, "%i", codenumber);
											strcat(line, nrbuf);
										}
										else {
											strcat(line, tmp);
										}
									}
									//else if (ppIsNumber(ptr) == true) {
									else if ((codenumber = strtol(ptr, NULL, ppGetBase(ptr))) != 0L) {
										//codenumber = strtol(ptr, NULL, 0);
										char nrbuf[32];
										sprintf(nrbuf, "%i", codenumber);
										strcat(line, nrbuf);
										//printf("%ld ------ NUMBER CONVERTED!\n", codenumber);
									}
									else {
										strcat(line, ptr);

									}
									if (comma != NULL) { // if trailing comma then add it after
										strcat(line, ",");
									}
									if (newline != NULL) { // not very elegant but fuck it for now
										ptr = newline + 1;
										newline = NULL;
										strcat(line, "\n");
										goto fuckery;
									}
									strcat(line, " ");
									ptr = strtok(NULL, " ");
								}
								free(dup);
								tok[0] = NULL;
								//tok[i] = tmp;
							}
							else {
								tok[i] = tmp;
							}
						} else if ((codenumber = strtol(tok[i], NULL, ppGetBase(tok[i]))) != 0L) {
							//codenumber = strtol(ptr, NULL, 0);
							//char nrbuf[32];
							sprintf(tok[i], "%i", codenumber);
							//strcat(line, nrbuf);
							//printf("%ld ------ NUMBER CONVERTED!\n", codenumber);
						}
					}
				}
				if (tok[0] != NULL) {
					if (tok[1] == NULL) {
						sprintf(line, "%s", tok[0]);
					}
					else if (tok[2] == NULL) {
						sprintf(line, "%s %s", tok[0], tok[1]);
					}
					else {
						sprintf(line, "%s %s, %s", tok[0], tok[1], tok[2]);
					}
				}
			}
			else { // a label
				// TODO!! use the set types to check instead
				line[0] = '\0';
			}
		}
		break;
		free(line2);
	}
	return 0;
}
