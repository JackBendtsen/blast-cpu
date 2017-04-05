#ifndef OPCODES_H
#define OPCODES_H

#define N_OPS 48
#define N_IDX_OPS 32
#define HIGH_OPS_BASE 128

typedef unsigned char u8;
typedef unsigned short u16;

static char *op_str[] = {
	"nop", "r", "l", "ja",
	"shl", "shli", "shr", "shri",
	"eb", "et", "add", "sub",
	"or", "and", "xor", "m",
	"adda", "addia", "lia", "lias",
	"ll", "sl", "mtar", "mfar",
	"mfir", "mfov", "int", "b",
	"wr", "wi", "rr", "ri",
	"addi", "ori", "andi", "xori",
	"bn", "bp", "bz", "bq",
	"lw", "lb", "sw", "sb",
	"jr", "li", "la", "sa"
};

static int op_type[][5] = {
	{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 5, 5, 5, 5}, {0, 5, 5, 5, 5},
	{0, 1, 1, 2, 0}, {0, 4, 1, 2, 0}, {0, 1, 1, 2, 0}, {0, 4, 1, 2, 0},
	{0, 1, 2, 0, 0}, {0, 1, 2, 0, 0}, {0, 1, 1, 2, 0}, {0, 1, 1, 2, 0},
	{0, 1, 1, 2, 0}, {0, 1, 1, 2, 0}, {0, 1, 1, 2, 0}, {0, 1, 2, 0, 0},
	{0, 1, 0, 0, 0}, {0, 5, 5, 5, 5}, {0, 5, 5, 5, 5}, {0, 0, 0, 0, 4},
	{0, 5, 5, 5, 1}, {0, 5, 5, 5, 1}, {0, 5, 5, 1, 0}, {0, 5, 5, 2, 0},
	{0, 2, 0, 0, 0}, {0, 2, 0, 0, 0}, {0, 5, 5, 5, 5}, {0, 5, 5, 5, 5},
	{0, 1, 1, 0, 0}, {0, 5, 5, 5, 1}, {0, 1, 2, 0, 0}, {0, 5, 5, 5, 2},
	{3, 5, 5, 5, 5}, {3, 5, 5, 5, 5}, {3, 5, 5, 5, 5}, {3, 5, 5, 5, 5},
	{1, 5, 5, 5, 5}, {1, 5, 5, 5, 5}, {1, 5, 5, 5, 5}, {1, 5, 5, 5, 5},
	{2, 5, 5, 5, 5}, {2, 5, 5, 5, 5}, {1, 5, 5, 5, 5}, {1, 5, 5, 5, 5},
	{1, 5, 5, 5, 5}, {2, 5, 5, 5, 5}, {1, 5, 5, 5, 5}, {1, 5, 5, 5, 5}
};

int assemble(u8 **out, int *out_sz, u8 *in, int in_sz);
int disassemble(u8 **out, int *out_sz, u8 *in, int in_sz);

#endif