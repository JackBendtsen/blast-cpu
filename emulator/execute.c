#include "cpu.h"
#include "ram.h"

// ------------------- Execute --------------------

// This function selects and runs one of the instruction cycle functions below that acts as one CPU cycle.
// Some instructions require two execute cycles (the first being called execute and the second called memory)
// to function, so this function will be invoked twice per such an instruction.

void execute(regs *r) {
	// In this case, XOR gives us what we want while being faster than subtraction,
	// despite subtraction being the technically correct approach.
	int s = r->state ^ 2;
	if (s < 0 || s > 1) return;

	if (ops[r->cmd].exec[s]) ops[r->cmd].exec[s](r);
}

// ----------------- Instructions -----------------

// --- Execute Cycle ---

void R(regs *r) {
	r->pc = r->lr;
}
void L(regs *r) {
	r->ta = r->pc;
	if (add_address(&r->ta, r->dw[0]) < 0) return;
	r->lr = r->ta;
}
void JA(regs *r) {
	r->ta = r->ar;
	if (add_address(&r->ta, r->dw[0]) < 0) return;
	r->pc = r->ta;
}
void SHL(regs *r) {
	r->dw[1] &= 0xf;
	r->dw[0] <<= r->dw[1];
	r->r[r->rr] = r->dw[0];
}
void SHLI(regs *r) {
	r->dw[1] &= 0xf;
	r->dw[0] <<= r->dw[1];
	r->r[r->rr] = r->dw[0];
}
void SHR(regs *r) {
	r->dw[1] &= 0xf;
	r->dw[0] >>= r->dw[1];
	r->r[r->rr] = r->dw[0];
}
void SHRI(regs *r) {
	r->dw[1] &= 0xf;
	r->dw[0] >>= r->dw[1];
	r->r[r->rr] = r->dw[0];
}
void EB(regs *r) {
	if (r->dw[0] & 0x0080) r->dw[1] = r->dw[0] | 0xFF00;
	else r->dw[1] = r->dw[0] & 0x00FF;
	r->r[r->rr] = r->dw[1];
}
void ET(regs *r) {
	if (r->dw[0] & 0x0800) r->dw[1] = r->dw[0] | 0xF000;
	else r->dw[1] = r->dw[0] & 0x0FFF;
	r->r[r->rr] = r->dw[1];
}
void ADD(regs *r) {
	_add(r);
	r->ov = r->dw[2];
	r->r[r->rr] = r->dw[0];
}
void SUB(regs *r) {
	_sub(r);
	r->ov = r->dw[2];
	r->r[r->rr] = r->dw[0];
}
void OR(regs *r) {
	r->dw[0] |= r->dw[1];
	r->r[r->rr] = r->dw[0];
}
void AND(regs *r) {
	r->dw[0] &= r->dw[1];
	r->r[r->rr] = r->dw[0];
}
void XOR(regs *r) {
	r->dw[0] ^= r->dw[1];
	r->r[r->rr] = r->dw[0];
}
void M(regs *r) {
	r->r[r->rr] = r->dw[0];
}
void ADDA(regs *r) {
	add_address(&r->ar, r->dw[0]);
}
void ADDIA(regs *r) {
	add_address(&r->ar, r->dw[0]);
}
void LIA(regs *r) {
	r->ar &= ~0xFFFF;
	r->ar |= r->dw[0];
}
void LIAS(regs *r) {
	r->ar &= 0xFFFF;
	r->ar |= (int)(r->dw[0] << 16);
}
void MTAR(regs *r) {
	r->dw[1] &= 0x1F;
	r->ar &= ~(0xFFFF << r->dw[1]);
	r->ar |= (int)(r->dw[0] << r->dw[1]);
	r->ar &= 0xFFFFF;
}
void MFAR(regs *r) {
	r->dw[0] &= 0x1F;
	r->dw[1] = (u16)(r->ar >> r->dw[0]);
	r->r[r->rr] = r->dw[1];
}
void MFIR(regs *r) {
	r->dw[0] = r->itr;
	r->r[r->rr] = r->dw[0];
}
void MFOV(regs *r) {
	r->dw[0] = r->ov;
	r->r[r->rr] = r->dw[0];
}
void INT(regs *r) {
	r->itr = r->dw[0];
}
void B(regs *r) {
	r->ta = r->pc;
	if (add_address(&r->ta, r->dw[0]) < 0) return;
	r->pc = r->ta;
}
void IO0_x(regs *r) {
	r->dw[0] &= 0x7FE;
}
void IO1_x(regs *r) {
	r->dw[1] &= 0x7FE;
}
void ADDI(regs *r) {
	_add(r);
	r->ov = (u8)r->dw[2];
	r->r[r->rr] = r->dw[0];
}
void ORI(regs *r) {
	r->dw[0] |= r->dw[1];
	r->r[r->rr] = r->dw[0];
}
void ANDI(regs *r) {
	r->dw[0] &= r->dw[1];
	r->r[r->rr] = r->dw[0];
}
void XORI(regs *r) {
	r->dw[0] ^= r->dw[1];
	r->r[r->rr] = r->dw[0];
}
void BN(regs *r) {
	if (!(r->dw[1] & 0x8000)) return;
	r->ta = r->pc;
	if (add_address(&r->ta, r->dw[0]) < 0) return;
	r->pc = r->ta;
}
void BP(regs *r) {
	if (r->dw[1] & 0x8000) return;
	r->ta = r->pc;
	if (add_address(&r->ta, r->dw[0]) < 0) return;
	r->pc = r->ta;
}
void BZ(regs *r) {
	if (r->dw[1]) return;
	r->ta = r->pc;
	if (add_address(&r->ta, r->dw[0]) < 0) return;
	r->pc = r->ta;
}
void BQ(regs *r) {
	if (!r->dw[1]) return;
	r->ta = r->pc;
	if (add_address(&r->ta, r->dw[0]) < 0) return;
	r->pc = r->ta;
}
void MEM_x(regs *r) {
	r->ta = r->ar;
	if (add_address(&r->ta, r->dw[0]) < 0) return;
}
void JR(regs *r) {
	r->ta = r->dw[1] << 4;
	if (add_address(&r->ta, r->dw[0]) < 0) return;
	r->pc = r->ta;
}
void LI(regs *r) {
	r->r[r->rr] = r->dw[0];
}
void AMEM_x(regs *r) {
	r->ta = (int)(r->dw[0] << 4);
	if (add_address(&r->ta, r->dw[1]) < 0) return;
}
void AMEM2_x(regs *r) {
	r->ta = (int)(r->dw[1] << 4);
	if (add_address(&r->ta, r->dw[0]) < 0) return;
}

// --- Memory Cycle ---

void WR_m(regs *r) {
	io_mem[r->dw[0]] = (u8)(r->dw[1] >> 8);
	io_mem[r->dw[0] + 1] = (u8)r->dw[1];
}
void WI_m(regs *r) {
	io_mem[r->dw[1]] = (u8)(r->dw[0] >> 8);
	io_mem[r->dw[1] + 1] = (u8)r->dw[0];
}
void READ_m(regs *r) {
	r->dw[1] = io_mem[r->dw[0]] << 8;
	r->dw[1] |= io_mem[r->dw[0] + 1];
	r->r[r->rr] = r->dw[1]; // This line should technically have a cycle to itself but I really want a low cycle count. Let me dream!
}

void LW_m(regs *r) {
	r->dw[1] = read_word(0, r->ta);
	r->r[r->rr] = r->dw[1]; // Same story for this line
}
void LB_m(regs *r) {
	r->dw[1] = read_byte(0, r->ta);
	r->r[r->rr] = r->dw[1]; // And this line
}
void SW_m(regs *r) {
	write_word(0, r->ta, r->dw[1]);
}
void SB_m(regs *r) {
	write_byte(0, r->ta, (u8)r->dw[1]);
}

void LA_m(regs *r) {
	r->ar = read_triple(0, r->ta);
	r->ar &= 0xFFFFF;
}
void SA_m(regs *r) {
	write_triple(0, r->ta, r->ar);
}
void LL_m(regs *r) {
	r->lr = read_triple(0, r->ta);
	r->lr &= 0xFFFFF;
}
void SL_m(regs *r) {
	write_triple(0, r->ta, r->lr);
}

// ----------------- Helper Functions -----------------

inline void _add(regs *r) {
	r->ta = r->dw[0] + r->dw[1];
	r->dw[0] = r->ta & 0xFFFF;
	r->dw[2] = (u16)(r->ta >> 16);
	if (r->dw[2] && r->dw[1] & 0x8000) r->dw[2] = 0xFFFF;
}

inline void _sub(regs *r) {
	r->ta = r->dw[0] - r->dw[1];
	r->dw[0] = r->ta & 0xFFFF;
	r->dw[2] = (u16)(r->ta >> 16);
	if (r->dw[2] && r->dw[1] & 0x8000) r->dw[2] = 1;
}

inline int add_address(int *addr, int dw) {
	int a = *addr;
	a += dw;
	if (dw & 0x8000) a -= 0x10000;
	if (a & 0xF00000) return -1;
	*addr = a;
	return 0;
}