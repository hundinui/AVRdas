#include "common.h"
#include "instructions.h"
#include "disasm_instructions.h"
#include "avrasm.h"
#include "preprocessor.h"

#define MNEM_SPACING 6
#define FUNC_ALLOC 32

FILE *outputfile;
bool hasDefinitions;
bool prefixInstruction;

// opcode - opcode
// operands - returns the operands there
// next - next opcode, used if opcode is 16 bit
// dual - if 16 bit then set to 1 to not re-read the second part
// returns pointer to char containing mnemonics text
const char *get_mnemonics(unsigned short opcode, char **operands, unsigned short next, int *dual) {
	for (int i = 0; i <= (TABLE_SIZE); i++) {
		if (instruction_table[i].opcode == (opcode & ~instruction_table[i].mask)) {
			if (instruction_table[i].words == 2) { // if 32bit opcode then set dual as 1 to not disassemble the second part again
				*operands = instruction_disasm[i](opcode, next);
				*dual = 1;
			}
			else {
				*operands = instruction_disasm[i](opcode, 0);
				*dual = 0;
			}
			if (strcmp(instruction_table[i].mnemonic, "LDD") == 0) { // temp fix for LDD/LD 
				return ((((opcode >> 8) & 0x000E) != 0) || (((opcode >> 13) & 0x000E) != 0)) ? "ld":"ldd";
			}
			return instruction_table[i].mnemonic;
		}
	}
	*operands = '\0';
	return ".unk";//"unknown opcode";
}

//
unsigned short get_opcode(const char *mnemonics) {
	for (int i = 0; i <= sizeof(instruction_table); i++) {
		if (instruction_table[i].mnemonic == mnemonics) {
			return instruction_table[i].opcode;
		}
	}
	return 0;
}

int resetJumpDone;

// returns 0 if 8 or 1 if 16 bit opcode
// opcode - opcode
// next - next opcode
int disasm_opcode(unsigned short opcode, unsigned short next, int address) {
	const char *mnem;
	char *oper = NULL;
	int dual = 0;
	mnem = get_mnemonics(opcode, &oper, next, &dual);

	if (resetJumpDone == 0) {
		if (strstr(mnem, "jmp") != NULL) {
			char *dup = strdup(oper);
			char *ptr = strtok(dup, " ,+-");
			int codenumber;
			while (ptr != NULL) {
				if ((codenumber = strtol(ptr, NULL, ppGetBase(ptr))) != 0L) {
					resetJumpDone = 2 * (codenumber);
					oper = "RESET";
					break;
				}
				ptr = strtok(NULL, " ,+-");
			}
			free(dup);
		}
	}
	// lazy opcode formatting
	char format[FUNC_ALLOC + MNEM_SPACING];
	int i = strlen(mnem);
	if (MNEM_SPACING != i) {
		strcpy(format, mnem);
		for (;i <= MNEM_SPACING; i++) {
			format[i] = ' ';
		}
		format[i] = '\0';
	}
	if (prefixInstruction == true) {
		if(dual == 1) {
			fprintf(outputfile, "%04X:%04X %04X   %s%s\n", address / 2, opcode, next, format, oper);
		}
		else {
			fprintf(outputfile, "%04X:%04X        %s%s\n", address / 2, opcode, format, oper);
		}
	}
	else {
		fprintf(outputfile, "%s %s\n", format, oper);
	}
	return dual;
}

/*void debug_assemble(char *mnemonic, unsigned int operand1, unsigned int operand2) {
	unsigned short dual;
	unsigned short op = asm_opcode(mnemonic, operand1, operand2, &dual);
	dual ? printf("%s %i %i - 0x%04X - ", mnemonic, operand1, operand2, op) : printf("%s %i %i - 0x%04X 0x%04X - ", mnemonic, operand1, operand2, op, dual);
	disasm_opcode(op, NULL, NULL);
}*/

int asmGetSize(char *mnemonic) {
	for (int i = 0; i < (sizeof(instruction_table) / sizeof(instruction_table[0])); i++) {
		if (strcmp(instruction_table[i].mnemonic, mnemonic) == 0) {
			return instruction_table[i].words;
		}
	}
	//printf("Invalid instruction!\n");
	return 0;
}
