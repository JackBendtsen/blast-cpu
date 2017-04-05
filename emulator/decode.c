#include "cpu.h"

/*
 * This function is responsible for decoding an instruction once it has been loaded into to instruction register from memory,
 * as well as incrementing the program counter (PC).
 *
 * Based on the contents of the instruction register, it determines which instruction will be executed,
 * and the nature of the arguments that will power it.
*/
void decode(regs *r) {
	r->cmd = r->insr >> 16;
	// If the current instruction is a nop, increment PC by 1, else increment PC by 3
	r->dw[0] = r->cmd ? 3 : 1;

	r->ta = r->pc;
	if (add_address(&r->ta, r->dw[0]) < 0) {
		printf("Error Incrementing PC\n");
		r->itr |= 7; // Halt execution
		return;
	}
	r->pc = r->ta;
	r->ta = r->insr; // Set the temporary address register to the value of the instruction register
	r->dw[3] = 0;

	/*
	 * This loop determines the nature of the arguments in the current instruction.
	 * The arguments are kept in reverse order, so to decode the arguments, the loop operates in reverse order.
	 * Every round of the loop, the temporary address register gets shifted 4 bits to the right.
	 * 
	 * Each of the 5 argument slots has a type (see cpu.h for reference).
	 * If an argument is of type 1 or 3, the contents of the register indexed by the argument
	 *  gets placed in the next temporary data register.
	 * If it's of type 2 or 3, the return register is set to that argument.
	 * If it's of type 4, it gets placed in the next temporary data register as-is.
	 * If it's of type 5, it becomes the next digit in the 4th temporary data register.
	*/
	int i, idx = 0, n = 0, shift = 0;
	for (i = 4; i >= 0; i--) {
		int type = ops[r->cmd].dec[i];
		if (n && (type != 5 || i == 0)) r->dw[idx++] = r->dw[3];
		switch (type) {
			case 1:
			case 3:
				if (idx >= 4) break;
				if (i == 0) r->tr = r->cmd & 7;
				else r->tr = r->ta & 7;
				r->dw[idx++] = r->r[r->tr];
				if (type == 3) r->rr = r->tr;
				break;
			case 2:
				if (i == 0) r->rr = r->cmd & 7;
				else r->rr = r->ta & 7;
				break;
			case 4:
				if (i < 1) continue;
				else r->tr = r->ta & 7;
				r->dw[idx++] = r->tr;
				break;
			case 5:
				if (i < 1) continue;
				n = 1;
				r->dw[3] |= (r->ta & 0xf) << (shift++ * 4);
				break;
		}
		r->ta >>= 4;
	}
}
