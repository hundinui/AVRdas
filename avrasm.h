#ifndef AVRASM_H
#define AVRASM_H

extern FILE *outputfile;
extern bool hasDefinitions;
extern bool prefixInstruction;

extern int resetJumpDone;


#define TYPE_ZH 1
#define TYPE_ZL 0

/*
 * opcode - opcode
 * operands - returns the operands there
 * next - next opcode, used if opcode is 16 bit
 * dual - if 16 bit then set to 1 to not re-read the second part
 * returns pointer to char containing mnemonics text
 */
const char * get_mnemonics(unsigned short opcode, char ** operands, unsigned short next, int *dual);

/*
 * Returns opcode by mnemonic
 */
unsigned short get_opcode(const char * mnemonics);

/*
 * returns 0 if 8 or 1 if 16 bit opcode
 * opcode - opcode
 * next - next opcode
 */
int disasm_opcode(unsigned short opcode, unsigned short next, int address);

//void debug_assemble(char * mnemonic, unsigned int operand1, unsigned int operand2);

/*
 * Returns the size of the opcode
 */
int asmGetSize(char * mnemonic);

//unsigned short asm_opcode(char * mnemonic, unsigned int operand1, unsigned int operand2, unsigned short *dual);

#endif
