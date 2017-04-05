#ifndef BLAST_CPU_H
#define BLAST_CPU_H

#include "blast.h"

#define N_INS 48
#define N_IDX_OPS 32

// CPU Registers
typedef struct {
	u16 r[8];    // General Purpose Registers
	u16 dw[4];   // Temporary 16-bit Data Registers
	int ar;      // Address Register
	int ta;      // Temporary Address Register
	int insr;    // Full Instruction Register
	u8 cmd;      // Instruction Command Register
	u8 tr;       // Temporary Register Index
	u8 rr;       // Return Register Index
	u8 state;    // Cycle State Register
	u16 ov;      // Overflow Register
	u16 itr;     // Interrupt Register
	int pc;      // Program Counter
	int lr;      // Linking Register
	u8 dbg;      // Debug On/Off, not a register
	u8 step;     // Single Stepping On/Off, not a register
} regs;

typedef void (*func)(regs *);

typedef struct {
	char name[6]; // Instruction Name
	int dec[5];   // Opcode Layout
	func exec[2]; // Instruction Cycle Function Pointers
} op_t;

int add_address(int *addr, int dw);
void _add(regs *r);
void _sub(regs *r);

void disasm(regs *r, char *args);
void pause(regs *r);
int get_op_idx(regs *r);

void cpu_tick(regs *r);

void decode(regs *r);
void prepare(regs *r);
void execute(regs *r);

void R(regs *r);
void L(regs *r);
void JA(regs *r);
void SHL(regs *r);
void SHLI(regs *r);
void SHR(regs *r);
void SHRI(regs *r);
void EB(regs *r);
void ET(regs *r);
void ADD(regs *r);
void SUB(regs *r);
void OR(regs *r);
void AND(regs *r);
void XOR(regs *r);
void M(regs *r);
void ADDA(regs *r);
void ADDIA(regs *r);
void LIA(regs *r);
void LIAS(regs *r);
void LL(regs *r);
void SL(regs *r);
void MTAR(regs *r);
void MFAR(regs *r);
void MFIR(regs *r);
void MFOV(regs *r);
void INT(regs *r);
void B(regs *r);
void IO0_x(regs *r);
void IO1_x(regs *r);
void ADDI(regs *r);
void ORI(regs *r);
void ANDI(regs *r);
void XORI(regs *r);
void BN(regs *r);
void BP(regs *r);
void BZ(regs *r);
void BQ(regs *r);
void MEM_x(regs *r);
void JR(regs *r);
void LI(regs *r);
void AMEM_x(regs *r);
void AMEM2_x(regs *r);

void LL_m(regs *r);
void SL_m(regs *r);
void WR_m(regs *r);
void WI_m(regs *r);
void READ_m(regs *r);
void LW_m(regs *r);
void LB_m(regs *r);
void SW_m(regs *r);
void SB_m(regs *r);
void LA_m(regs *r);
void SA_m(regs *r);

/*
 * In an instruction, each nybble (4-bits) can contain an argument. Each instruction is 3 bytes (24 bits) long.
 * The first byte of an instruction contains the opcode, which determines which instruction it is.
 * The opcode cannot be repurposed as an argument, as it contains necessary information about an instruction that is not its arguments,
 * meaning only the lowest 16 bits of instruction appear to be usable. However, if the highest bit of the opcode byte is set,
 *  the lowest 3 bits of the opcode byte are interpreted as a register index and not part of the opcode,
 *  freeing up the remaining 16 bits of the instruction to be used as one large number.
 * The nature of this special instruction type means that there can only be a maximum of 16 of them (128 / (2 pow 3)).
 * Otherwise, those 3 bits from the opcode byte are out of reach,
 *  meaning that there is only space for 4 nybbles (16 bits) worth of arguments, instead of 5 nybbles.
 * 
 * Each nybble is an argument slot, and each argument slot has an argument type:
 *   0 = no argument,
 *   1 = input register,
 *   2 = output register,
 *   3 = input/output register,
 *   4 = small (self-contained) number,
 *   5 = (part of a) large number
*/

static op_t ops[] = {
	{"nop",   {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"r",     {0, 0, 0, 0, 0}, {R,      NULL}},
	{"l",     {0, 5, 5, 5, 5}, {L,      NULL}},
	{"ja",    {0, 5, 5, 5, 5}, {JA,     NULL}},
	{"shl",   {0, 1, 1, 2, 0}, {SHL,    NULL}},
	{"shli",  {0, 4, 1, 2, 0}, {SHLI,   NULL}},
	{"shr",   {0, 1, 1, 2, 0}, {SHR,    NULL}},
	{"shri",  {0, 4, 1, 2, 0}, {SHRI,   NULL}},
	{"eb",    {0, 1, 2, 0, 0}, {EB,     NULL}},
	{"et",    {0, 1, 2, 0, 0}, {ET,     NULL}},
	{"add",   {0, 1, 1, 2, 0}, {ADD,    NULL}},
	{"sub",   {0, 1, 1, 2, 0}, {SUB,    NULL}},
	{"or",    {0, 1, 1, 2, 0}, {OR,     NULL}},
	{"and",   {0, 1, 1, 2, 0}, {AND,    NULL}},
	{"xor",   {0, 1, 1, 2, 0}, {XOR,    NULL}},
	{"m",     {0, 1, 2, 0, 0}, {M,      NULL}},
	{"adda",  {0, 1, 0, 0, 0}, {ADDA,   NULL}},
	{"addia", {0, 5, 5, 5, 5}, {ADDIA,  NULL}},
	{"lia",   {0, 5, 5, 5, 5}, {LIA,    NULL}},
	{"lias",  {0, 0, 0, 0, 4}, {LIAS,   NULL}},
	{"ll",    {0, 5, 5, 5, 1}, {AMEM_x, LL_m}},
	{"sl",    {0, 5, 5, 5, 1}, {AMEM_x, SL_m}},
	{"mtar",  {0, 5, 5, 1, 0}, {MTAR,   NULL}},
	{"mfar",  {0, 5, 5, 2, 0}, {MFAR,   NULL}},
	{"mfir",  {0, 2, 0, 0, 0}, {MFIR,   NULL}},
	{"mfov",  {0, 2, 0, 0, 0}, {MFOV,   NULL}},
	{"int",   {0, 5, 5, 5, 5}, {INT,    NULL}},
	{"b",     {0, 5, 5, 5, 5}, {B,      NULL}},
	{"wr",    {0, 1, 1, 0, 0}, {IO0_x,  WR_m}},
	{"wi",    {0, 5, 5, 5, 1}, {IO1_x,  WI_m}},
	{"rr",    {0, 1, 2, 0, 0}, {IO0_x,  READ_m}},
	{"ri",    {0, 5, 5, 5, 2}, {IO0_x,  READ_m}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"",      {0, 0, 0, 0, 0}, {NULL,   NULL}},
	{"addi",  {3, 5, 5, 5, 5}, {ADDI,   NULL}},
	{"addi",  {3, 5, 5, 5, 5}, {ADDI,   NULL}},
	{"addi",  {3, 5, 5, 5, 5}, {ADDI,   NULL}},
	{"addi",  {3, 5, 5, 5, 5}, {ADDI,   NULL}},
	{"addi",  {3, 5, 5, 5, 5}, {ADDI,   NULL}},
	{"addi",  {3, 5, 5, 5, 5}, {ADDI,   NULL}},
	{"addi",  {3, 5, 5, 5, 5}, {ADDI,   NULL}},
	{"addi",  {3, 5, 5, 5, 5}, {ADDI,   NULL}},
	{"ori",   {3, 5, 5, 5, 5}, {ORI,    NULL}},
	{"ori",   {3, 5, 5, 5, 5}, {ORI,    NULL}},
	{"ori",   {3, 5, 5, 5, 5}, {ORI,    NULL}},
	{"ori",   {3, 5, 5, 5, 5}, {ORI,    NULL}},
	{"ori",   {3, 5, 5, 5, 5}, {ORI,    NULL}},
	{"ori",   {3, 5, 5, 5, 5}, {ORI,    NULL}},
	{"ori",   {3, 5, 5, 5, 5}, {ORI,    NULL}},
	{"ori",   {3, 5, 5, 5, 5}, {ORI,    NULL}},
	{"andi",  {3, 5, 5, 5, 5}, {ANDI,   NULL}},
	{"andi",  {3, 5, 5, 5, 5}, {ANDI,   NULL}},
	{"andi",  {3, 5, 5, 5, 5}, {ANDI,   NULL}},
	{"andi",  {3, 5, 5, 5, 5}, {ANDI,   NULL}},
	{"andi",  {3, 5, 5, 5, 5}, {ANDI,   NULL}},
	{"andi",  {3, 5, 5, 5, 5}, {ANDI,   NULL}},
	{"andi",  {3, 5, 5, 5, 5}, {ANDI,   NULL}},
	{"andi",  {3, 5, 5, 5, 5}, {ANDI,   NULL}},
	{"xori",  {3, 5, 5, 5, 5}, {XORI,   NULL}},
	{"xori",  {3, 5, 5, 5, 5}, {XORI,   NULL}},
	{"xori",  {3, 5, 5, 5, 5}, {XORI,   NULL}},
	{"xori",  {3, 5, 5, 5, 5}, {XORI,   NULL}},
	{"xori",  {3, 5, 5, 5, 5}, {XORI,   NULL}},
	{"xori",  {3, 5, 5, 5, 5}, {XORI,   NULL}},
	{"xori",  {3, 5, 5, 5, 5}, {XORI,   NULL}},
	{"xori",  {3, 5, 5, 5, 5}, {XORI,   NULL}},
	{"bn",    {1, 5, 5, 5, 5}, {BN,     NULL}},
	{"bn",    {1, 5, 5, 5, 5}, {BN,     NULL}},
	{"bn",    {1, 5, 5, 5, 5}, {BN,     NULL}},
	{"bn",    {1, 5, 5, 5, 5}, {BN,     NULL}},
	{"bn",    {1, 5, 5, 5, 5}, {BN,     NULL}},
	{"bn",    {1, 5, 5, 5, 5}, {BN,     NULL}},
	{"bn",    {1, 5, 5, 5, 5}, {BN,     NULL}},
	{"bn",    {1, 5, 5, 5, 5}, {BN,     NULL}},
	{"bp",    {1, 5, 5, 5, 5}, {BP,     NULL}},
	{"bp",    {1, 5, 5, 5, 5}, {BP,     NULL}},
	{"bp",    {1, 5, 5, 5, 5}, {BP,     NULL}},
	{"bp",    {1, 5, 5, 5, 5}, {BP,     NULL}},
	{"bp",    {1, 5, 5, 5, 5}, {BP,     NULL}},
	{"bp",    {1, 5, 5, 5, 5}, {BP,     NULL}},
	{"bp",    {1, 5, 5, 5, 5}, {BP,     NULL}},
	{"bp",    {1, 5, 5, 5, 5}, {BP,     NULL}},
	{"bz",    {1, 5, 5, 5, 5}, {BZ,     NULL}},
	{"bz",    {1, 5, 5, 5, 5}, {BZ,     NULL}},
	{"bz",    {1, 5, 5, 5, 5}, {BZ,     NULL}},
	{"bz",    {1, 5, 5, 5, 5}, {BZ,     NULL}},
	{"bz",    {1, 5, 5, 5, 5}, {BZ,     NULL}},
	{"bz",    {1, 5, 5, 5, 5}, {BZ,     NULL}},
	{"bz",    {1, 5, 5, 5, 5}, {BZ,     NULL}},
	{"bz",    {1, 5, 5, 5, 5}, {BZ,     NULL}},
	{"bq",    {1, 5, 5, 5, 5}, {BQ,     NULL}},
	{"bq",    {1, 5, 5, 5, 5}, {BQ,     NULL}},
	{"bq",    {1, 5, 5, 5, 5}, {BQ,     NULL}},
	{"bq",    {1, 5, 5, 5, 5}, {BQ,     NULL}},
	{"bq",    {1, 5, 5, 5, 5}, {BQ,     NULL}},
	{"bq",    {1, 5, 5, 5, 5}, {BQ,     NULL}},
	{"bq",    {1, 5, 5, 5, 5}, {BQ,     NULL}},
	{"bq",    {1, 5, 5, 5, 5}, {BQ,     NULL}},
	{"lw",    {2, 5, 5, 5, 5}, {MEM_x,  LW_m}},
	{"lw",    {2, 5, 5, 5, 5}, {MEM_x,  LW_m}},
	{"lw",    {2, 5, 5, 5, 5}, {MEM_x,  LW_m}},
	{"lw",    {2, 5, 5, 5, 5}, {MEM_x,  LW_m}},
	{"lw",    {2, 5, 5, 5, 5}, {MEM_x,  LW_m}},
	{"lw",    {2, 5, 5, 5, 5}, {MEM_x,  LW_m}},
	{"lw",    {2, 5, 5, 5, 5}, {MEM_x,  LW_m}},
	{"lw",    {2, 5, 5, 5, 5}, {MEM_x,  LW_m}},
	{"lb",    {2, 5, 5, 5, 5}, {MEM_x,  LB_m}},
	{"lb",    {2, 5, 5, 5, 5}, {MEM_x,  LB_m}},
	{"lb",    {2, 5, 5, 5, 5}, {MEM_x,  LB_m}},
	{"lb",    {2, 5, 5, 5, 5}, {MEM_x,  LB_m}},
	{"lb",    {2, 5, 5, 5, 5}, {MEM_x,  LB_m}},
	{"lb",    {2, 5, 5, 5, 5}, {MEM_x,  LB_m}},
	{"lb",    {2, 5, 5, 5, 5}, {MEM_x,  LB_m}},
	{"lb",    {2, 5, 5, 5, 5}, {MEM_x,  LB_m}},
	{"sw",    {1, 5, 5, 5, 5}, {MEM_x,  SW_m}},
	{"sw",    {1, 5, 5, 5, 5}, {MEM_x,  SW_m}},
	{"sw",    {1, 5, 5, 5, 5}, {MEM_x,  SW_m}},
	{"sw",    {1, 5, 5, 5, 5}, {MEM_x,  SW_m}},
	{"sw",    {1, 5, 5, 5, 5}, {MEM_x,  SW_m}},
	{"sw",    {1, 5, 5, 5, 5}, {MEM_x,  SW_m}},
	{"sw",    {1, 5, 5, 5, 5}, {MEM_x,  SW_m}},
	{"sw",    {1, 5, 5, 5, 5}, {MEM_x,  SW_m}},
	{"sb",    {1, 5, 5, 5, 5}, {MEM_x,  SB_m}},
	{"sb",    {1, 5, 5, 5, 5}, {MEM_x,  SB_m}},
	{"sb",    {1, 5, 5, 5, 5}, {MEM_x,  SB_m}},
	{"sb",    {1, 5, 5, 5, 5}, {MEM_x,  SB_m}},
	{"sb",    {1, 5, 5, 5, 5}, {MEM_x,  SB_m}},
	{"sb",    {1, 5, 5, 5, 5}, {MEM_x,  SB_m}},
	{"sb",    {1, 5, 5, 5, 5}, {MEM_x,  SB_m}},
	{"sb",    {1, 5, 5, 5, 5}, {MEM_x,  SB_m}},
	{"jr",    {1, 5, 5, 5, 5}, {JR,     NULL}},
	{"jr",    {1, 5, 5, 5, 5}, {JR,     NULL}},
	{"jr",    {1, 5, 5, 5, 5}, {JR,     NULL}},
	{"jr",    {1, 5, 5, 5, 5}, {JR,     NULL}},
	{"jr",    {1, 5, 5, 5, 5}, {JR,     NULL}},
	{"jr",    {1, 5, 5, 5, 5}, {JR,     NULL}},
	{"jr",    {1, 5, 5, 5, 5}, {JR,     NULL}},
	{"jr",    {1, 5, 5, 5, 5}, {JR,     NULL}},
	{"li",    {2, 5, 5, 5, 5}, {LI,     NULL}},
	{"li",    {2, 5, 5, 5, 5}, {LI,     NULL}},
	{"li",    {2, 5, 5, 5, 5}, {LI,     NULL}},
	{"li",    {2, 5, 5, 5, 5}, {LI,     NULL}},
	{"li",    {2, 5, 5, 5, 5}, {LI,     NULL}},
	{"li",    {2, 5, 5, 5, 5}, {LI,     NULL}},
	{"li",    {2, 5, 5, 5, 5}, {LI,     NULL}},
	{"li",    {2, 5, 5, 5, 5}, {LI,     NULL}},
	{"la",    {1, 5, 5, 5, 5}, {AMEM2_x, LA_m}},
	{"la",    {1, 5, 5, 5, 5}, {AMEM2_x, LA_m}},
	{"la",    {1, 5, 5, 5, 5}, {AMEM2_x, LA_m}},
	{"la",    {1, 5, 5, 5, 5}, {AMEM2_x, LA_m}},
	{"la",    {1, 5, 5, 5, 5}, {AMEM2_x, LA_m}},
	{"la",    {1, 5, 5, 5, 5}, {AMEM2_x, LA_m}},
	{"la",    {1, 5, 5, 5, 5}, {AMEM2_x, LA_m}},
	{"la",    {1, 5, 5, 5, 5}, {AMEM2_x, LA_m}},
	{"sa",    {1, 5, 5, 5, 5}, {AMEM2_x, SA_m}},
	{"sa",    {1, 5, 5, 5, 5}, {AMEM2_x, SA_m}},
	{"sa",    {1, 5, 5, 5, 5}, {AMEM2_x, SA_m}},
	{"sa",    {1, 5, 5, 5, 5}, {AMEM2_x, SA_m}},
	{"sa",    {1, 5, 5, 5, 5}, {AMEM2_x, SA_m}},
	{"sa",    {1, 5, 5, 5, 5}, {AMEM2_x, SA_m}},
	{"sa",    {1, 5, 5, 5, 5}, {AMEM2_x, SA_m}},
	{"sa",    {1, 5, 5, 5, 5}, {AMEM2_x, SA_m}},
};

#endif
