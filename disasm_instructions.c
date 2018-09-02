// AVR instruction table
#include "common.h"
//#include "instructions.h"
#include "disasm_instructions.h"
#include "defstorage.h"
#include "avrasm.h"
#define FUNC_ALLOC 128

char tmp[FUNC_ALLOC];
int zsearch;

#ifdef HIGHER_CASE
const char *registers[] = { "R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15", "R16", "R17", "R18", "R19", "R20", "R21", "R22", "R23", "R24", "R25", "R26", "R27", "R28", "R29", "R30", "R31", "R32" };
#else
const char *registers[] = { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15", "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23", "r24", "r25", "r26", "r27", "r28", "r29", "r30", "r31", "r32" };
#endif

// TODO! load Z regs from definition
const char *toLDIRegister(unsigned short num) {
	if (num >= 0 && num < 16) {
		return registers[num + 16];
	}
	else {
		return "Malformed LDI register value!";
	}
}

const char *toRegister(unsigned short num) {
	if (num >= 0 && num < 32) {
		return registers[num];
	}
	else {
		return "Malformed register value!";
	}
}

char *func(unsigned short opcode, unsigned short next) {
	return " ";
}

char *func_l(unsigned short opcode, unsigned short next) {
	int8_t offset = ((opcode >> 3) & ~0xFF80) + 1;
	int sign = ((offset >> 6) != 1);
	//sprintf(tmp, sign ? ".+%d":".%d" , sign ? (~offset & 0x007F):~(~offset & 0x007F));
	//sprintf(tmp, sign ? ".+%d" : ".%d", sign ? offset : ~(~offset & 0x007F));
	sprintf(tmp, sign ? "PC + %d" : "PC %d", sign ? offset : ~(~offset & 0x007F));
	return tmp;
}

char *func_d_M(unsigned short opcode, unsigned short next) {
	sprintf(tmp, "%s, 0x%02X", toLDIRegister(((opcode >> 4) & ~0xFFF0)), ((opcode >> 4) & ~0xFF0F) | (opcode & ~0xFFF0));
	return tmp;
}

char *func_r(unsigned short opcode, unsigned short next) {
	sprintf(tmp, "%s", toRegister((opcode >> 4) & ~0xFFE0)); // format string was "%s\0" , why?
	return tmp;
}

char *func_p_s(unsigned short opcode, unsigned short next) { // operation on IO, check if we know name of IO address
	char *name = NULL;
	int value = (opcode >> 3) & ~0xFFE0;
	if (hasDefinitions == true) {
		sprintf(tmp, "0X%02X", value);
		name = ppStoreGetName(tmp, NULL, NULL);
		if (name != NULL) {
			sprintf(tmp ,"%s, %i", name, (opcode & ~0xFFF8));
			return tmp;
		}
	}
	sprintf(tmp, "0x%02X, %i", value, (opcode & ~0xFFF8));
	return tmp;
}

char *func_r_r(unsigned short opcode, unsigned short next) {
	//memset(&tmp, '\0', FUNC_ALLOC); // why was this here, can't remember, leaving it uncommented here in case it was actually needed somewhere
	sprintf(tmp, "%s, %s", toRegister((opcode >> 4) & ~0xFFE0), toRegister((opcode & ~0xFFF0) | ((opcode >> 5) & 0x0010)));
	return tmp;
}

char *func_S(unsigned short opcode, unsigned short next) {
	sprintf(tmp, "%i", (opcode >> 4) & ~0xFFF8);
	return tmp;
}

char *func_r_z(unsigned short opcode, unsigned short next) {
	sprintf(tmp, "%s, %s", toRegister((opcode >> 4) & ~0xFFE0), ((opcode & ~0xFFFE) == 1) ? "Z+" : "Z");
	return tmp;
}

char *func_d_n(unsigned short opcode, unsigned short next) {
	sprintf(tmp, "%s, 0x%02X", toLDIRegister(((opcode >> 4) & ~0xFFF0)), ((opcode >> 4) & ~0xFF0F) | (opcode & ~0xFFF0));
	return tmp;
}

char *func_d(unsigned short opcode, unsigned short next) {
	sprintf(tmp, "%s", toLDIRegister((opcode >> 4) & ~0xFFF0));
	return tmp;
}

char *func_r_s(unsigned short opcode, unsigned short next) {
	sprintf(tmp, "%s, %i", toRegister((opcode >> 4) & ~0xFFE0), (opcode & ~0xFFF8));
	return tmp;
}

char *func_r_P(unsigned short opcode, unsigned short next) { // add when i double check in
	char *name;
	int value = ((opcode >> 5) & 0x0030) | (opcode & 0x000F);
	if (hasDefinitions == true) {
		sprintf(tmp, "0x%02X", value);
		name = ppStoreGetName(tmp, NULL, NULL);
		if (name != NULL) {
			sprintf(tmp, "%s, %s", toRegister((opcode >> 4) & ~0xFFE0), name);
			return tmp;
		}
	}
	sprintf(tmp, "%s, 0x%04X", toRegister((opcode >> 4) & ~0xFFE0), value);
	return tmp;
}

char *func_P_r(unsigned short opcode, unsigned short next) { // not checked
	char *name;
	int value = ((opcode & 0x000F) | ((opcode & 0x0600) >> 5));
	if (hasDefinitions == true) {
		sprintf(tmp, "0X%02X", value);
		name = ppStoreGetName(tmp, NULL, NULL);
		if (name != NULL) {
			sprintf(tmp, "%s, %s", name, toRegister((opcode >> 4) & 0x001F));
			return tmp;
		}
	}
	sprintf(tmp, "0x%02X, %s", value, toRegister((opcode >> 4) & 0x001F));
	return tmp;
}

char *getADIWreg(unsigned short num) {
	sprintf(tmp, "r%d", 24 + (num * 2));
	return tmp;
}
char *func_w_K(unsigned short opcode, unsigned short next) {
	sprintf(tmp, "%s, 0x%02X", getADIWreg((opcode >> 4) & ~0xFFFC), (((opcode & ~0xFF3F) >> 2) | (opcode & ~0xFFF0)));
	return tmp;
}

char *func_s_l(unsigned short opcode, unsigned short next) {
	return "This should never get called and instead decoded as one of the other branch instructions!!!!";
}

char *func_h(unsigned short opcode, unsigned short next) {
	sprintf(tmp, "0x%04X", next);
	return tmp;
}

char *func_v_v(unsigned short opcode, unsigned short next) {
	sprintf(tmp, "%s, %s", toRegister(((opcode >> 4) & ~0xFFF0) * 2), toRegister((opcode & ~0xFFF0) * 2));
	return tmp;
}

char *func_d_d(unsigned short opcode, unsigned short next) {
	sprintf(tmp, "%s, %s", toLDIRegister((opcode >> 4) & ~0xFFF0), toLDIRegister(opcode & ~0xFFF0));
	return tmp;
}

char *func_a_a(unsigned short opcode, unsigned short next) {
	sprintf(tmp, "%s, %s", toRegister((opcode >> 4) & ~0xFFF8), toRegister(opcode & ~0xFFF8));
	return tmp;
}

char *func_i_r(unsigned short opcode, unsigned short next) { // TODO: STS for ATtiny10 based devices
	char *name;
	if (hasDefinitions == true) {
		sprintf(tmp, "0X%02X", next);
		name = ppStoreGetName(tmp, NULL, NULL);
		if (name != NULL) {
			sprintf(tmp, "%s, %s", name, toRegister((opcode >> 4) & ~0xFFE0));
			return tmp;
		}
	}
	sprintf(tmp, "0x%04X, %s", next, toRegister((opcode >> 4) & ~0xFFE0)); // format string was "0x%04X, %s\0", why did i have the nul there?
	return tmp;
}

char *func_r_i(unsigned short opcode, unsigned short next) {
	sprintf(tmp, "%s, 0x%02X", toRegister((opcode >> 4) & ~0xFFE0), next);
	return tmp;
}

char *func_r_b(unsigned short opcode, unsigned short next) {
	char *ptr[4];
	switch ((opcode >> 3)) {
	case 0: // X
		*ptr = "X";
		break;
	case 1: // Y
		*ptr = "Y";
		break;
	}
	sprintf(tmp, "%s, %i", toRegister((opcode >> 4) & ~0xFFE0), next);
	return tmp;
}

char *func_r_e(unsigned short opcode, unsigned short next) { // should work, untested tho
	//sprintf(tmp, "%s, %s", toRegister((opcode >> 4) & ~0xFFE0), ((opcode & ~0xFFFC) == 0) ? "X" : ((opcode & ~0xFFFC) == 1) ? "X+" : "-X", toRegister((opcode >> 4) & ~0xFFE0));
	const char *r = toRegister((opcode >> 4) & ~0xFFE0);
	const char *val = NULL;

	switch ((opcode & 0x0007)) {
	case 4:
		val = "X";
		break;
	case 5:
		val = "X+";
		break;
	case 6:
		val = "-X";
		break;
	default:
		val = "Shouldn't happen";
		break;
	}
	sprintf(tmp, "%s, %s", r, val);
	return tmp;
}

char *func_b_r(unsigned short opcode, unsigned short next) {
	//int z = (opcode >> 3) & 0x0001; // if 1 then ST/STD on Z reg, if 2 then on Y, 3 on X
	//int reg = (opcode >> 3) & 0x0001;
	char *val = NULL;
	const char *reg = toRegister((opcode >> 4) & 0x001F);
	if (((opcode & 0x000C) >> 2) == 3) { // ST on X
		switch (opcode & 0x0003) {
		case(0):
			val = "X";
			break;
		case(1):
			val = "X+";
			break;
		case(2):
			val = "-X";
			break;
		}
	}
	else if (((opcode >> 3) & 0x0001) == 1) { // ST on Y
		if (((opcode >> 12) & 0x0001) == 0) {
			int q = ((opcode >> 8) & 0x0020) | ((opcode >> 7) & 0x0018) | (opcode & 0x0007);
			sprintf(tmp, "Y+%i, %s", q, reg);
			goto displace;
		}
		switch (opcode & 0x0003) {
		case(0):
			val = "Y";
			break;
		case(1):
			val = "Y+";
			break;
		default:
			val = "ST Y error, should not happen!";
			break;
		}
	}
	else { // ST on Z
		if (((opcode >> 12) & 0x0001) == 0) {
			int q = ((opcode >> 8) & 0x0020) | ((opcode >> 7) & 0x0018) | (opcode & 0x0007);
			sprintf(tmp, "Z+%i, %s", q, reg);
			goto displace;
		}
		switch (opcode & 0x0003) {
		case(0):
			val = "Z";
			break;
		case(1):
			val = "Z+";
			break;
		default:
			val = "ST Z error, should not happen!";
			break;
		}
	}
	sprintf(tmp, "%s, %s", val, reg);
	/*switch ((opcode & 0x0007)) {
	case(0): // ST Z, Rr 0 ≤ r ≤ 31 PC ← PC + 1
		sprintf(tmp, z == 1 ? "Z, %s" : "Y, %s", reg);
		break;
	case(1): // ST Z+, Rr 0 ≤ r ≤ 31 PC ← PC + 1
		sprintf(tmp, z == 1 ? "Z+, %s" : "Y+, %s", reg);
		break;
	case(2): // ST -Z, Rr 0 ≤ r ≤ 31 PC ← PC + 1
		sprintf(tmp, z == 1 ? "-Z, %s" : "-Y, %s", reg);
		break;
	default: // STD Z + q, Rr 0 ≤ r ≤ 31, 0 ≤ q ≤ 63 PC ← PC + 1
		int q = ((opcode >> 8) & 0x0040) | ((opcode >> 7) & 0x0030) | (opcode & 0x0007);
		sprintf(tmp, z == 1 ? "Z+%i, %s" : "Y+%i, %s" , q, reg);
		break;
	}*/
	displace:
	return tmp;
	//return "decoding not implemented yet";
}

char *func_e_r(unsigned short opcode, unsigned short next) {
	return func_b_r(opcode, next); // lets do everything there instead
	/*sprintf(tmp, "%s, %s", ((opcode & ~0xFFFC) == 0) ? "X":((opcode & ~0xFFFC) == 1) ? "X+":"-X", toRegister((opcode >> 4) & ~0xFFE0));
	return tmp;*/
}

char *func_addr(unsigned short opcode, unsigned short next) {
	int16_t val = (opcode & 0x0FFF) + 1;
	int sign = ((val >> 11) != 1);
	if (val == 4096) {
		tmp[0] = 'P';
		tmp[1] = 'C';
		tmp[2] = '\0';
		return tmp;
	}
	//sprintf(tmp, sign ? ".+%d" :".%d", sign ? val:~(~val & 0x0FFF));
	sprintf(tmp, sign ? "PC + %d" : "PC %d", sign ? val : ~(~val & 0x0FFF));
	return tmp;
}

char *data(unsigned short opcode, unsigned short next) {
	sprintf(tmp, "0x%02x, 0x%02x", (opcode >> 8) & 0x00FF, opcode & 0x00FF);
	return tmp;
}

char *(*instruction_disasm[TABLE_SIZE])(unsigned short opcode, unsigned short next) = {
	func, func, func, func, func, func, func, func, func, func, func, func, func, func, func,
	func, func_S, func_S, func, func, func_r_z, func_r_z, func, func, func, func, func, func,
	func, func_r_r, func_r_r, func_r_r, func_r_r, func_r_r, func_r_r, func_r_r, func_r_r,
	func_r_r, func_r_r, func_r_r, func_r_r, func_r, func_r, func_r, func_r, func_d_M, func_d_n,
	func_d_M, func_d, func_d_M, func_d_M, func_d_M, func_d_M, func_d_M, func_r_s, func_r_s,
	func_r_s, func_r_s, func_r_P, func_P_r, func_w_K, func_w_K, func_p_s, func_p_s, func_p_s,
	func_p_s, func_l, func_l, func_l, func_l, func_l, func_l, func_l, func_l, func_l, func_l,
	func_l, func_l, func_l, func_l, func_l, func_l, func_l, func_l, func_s_l, func_s_l, func_addr,
	func_addr, func_h, func_h, func_r, func_r, func_r, func_r, func_r, func_r, func_r, func_r,
	func_r, func_r, func_v_v, func_d_d, func_a_a, func_a_a, func_a_a, func_a_a, func_i_r, func_r_i,
	func_r_b, func_r_e, func_b_r, func_e_r, func, func, data
};
