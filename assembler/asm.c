#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opcodes.h"

int assemble(u8 **out, int *out_sz, u8 *in, int in_sz) {
	if (!out || !out_sz || !in || in_sz < 1) return -1;

	u8 *buf = NULL;
	int i = 0, j, k, t, sz = 0;
	int comment = 0, line = 0;
	int mn = 1, op = -1, opcode = -1;

	/*
	   args contains encoded arguments for the current instruction,
	   arg_count keeps track of which argument is currently being referred to,
	   arg_len contains the amount of arguments in the current instruction,
	   cur_arg is the current argument,
	   arg_digit keeps track of the current digit of the current argument,
	   arg_sign is the sign of the current argument,
	   base is the base the next number will be read in,
	   bits_used contains the bit offset that the current argument will be shifted to when placed in args, and
	   arg_limits is an array of integer limits for each type of argument.
	*/
	int args = 0, arg_count = 0, arg_len = 0;
	int cur_arg = 0, arg_digit = 0, arg_sign = 1, base = 10, bits_used = 0;
	int arg_limits[] = {8, 16, 16, 256, 4096, 65536};

	/*
	   arg_format contains the list of argument types for an instruction, in reverse-order.
	   each argument type is given its own 4 bits inside of arg_format. the argument types are:
	   1 = reg, 2 = small number, 3 = big number (1 nybble), ..., 6 = big number (4 nybbles)	
	*/
	int arg_format = 0;

	while (i < in_sz) {
		if ((in[i] < 0x20 || in[i] > 0x7e) && in[i] != '\n' && in[i] != '\r' && in[i] != '\t') {
			printf("Unrecognised character byte '%#02x'\n", in[i]);
			if (buf) free(buf);
			return -2;
		}
		if ((comment || mn) && in[i] == '\n') {
			comment = 0;
			line++;
			i++;
			continue;
		}
		if (!comment && (in[i] == ';' || in[i] == '#')) {
			comment = 1;
			i++;
			continue;
		}
		if (comment || in[i] == ' ' || in[i] == '\t' || in[i] == '\r') {
			i++;
			continue;
		}

		if (mn) {
			for (j = 0; j < N_OPS; j++) {
				k = t = 0;
				while (!t && k < 6 && i+k <= in_sz &&
				  ((in[i+k] >= 'A' && in[i+k] <= 'Z') || (in[i+k] >= 'a' && in[i+k] <= 'z'))) {
					if (in[i+k] != op_str[j][k]) t = 1;
					else k++;
				}
				if (!t && k == strlen(op_str[j])) break;
			}
			if (j == N_OPS) {
				printf("Unknown instruction at line %d\n", line+1);
				if (buf) free(buf);
				return -3;
			}
			op = opcode = j;
			
			if (op >= N_IDX_OPS) {
				opcode = HIGH_OPS_BASE + (opcode - N_IDX_OPS) * 8;
				arg_len = 2;
				arg_format = 0x61; // 1 = reg, followed by 6 = 16-bit (4 nybble) number
			}
			else {
				int p = -1, nl = 0;
				for (j = 4; j >= 0; j--) {
					t = op_type[op][j];
					if (t < 1 || t > 5) {
						if (!arg_len) bits_used += 4;
						continue;
					}
					if (t == 5) {
						if (!nl) {
							arg_format |= 3 << (++p * 4);
							arg_len++;
						}
						else arg_format += 1 << (p * 4);
						nl = 1;
						continue;
					}
					if (t <= 3) {
						arg_format |= 1 << (++p * 4);
						arg_len++;
					}
					else {
						arg_format |= 2 << (++p * 4);
						arg_len++;
					}
					nl = 0;
				}
			}

			mn = 0;
			i += k;
			continue;
		}

		int arg_type = (arg_format >> (arg_count * 4)) & 0xf;

		int valid = 0, digit = 0;
		if (in[i] == '-' && arg_digit == 0 && arg_type > 1) {
			arg_sign = -1;
			valid = 1;
		}
		if (in[i] == 'r' || in[i] == 'R' && (arg_digit == 0 && arg_type == 1)) {
			valid = 1;
		}
		if ((in[i] == 'x' || in[i] == 'X') && arg_digit == 1 && in[i-1] == '0') {
			base = 16;
			valid = 1;
		}
		if ((in[i] >= '0' && in[i] <= '9') || (base == 16 && ((in[i] >= 'A' && in[i] <= 'F') || (in[i] >= 'a' && in[i] <= 'f')))) {
			digit = in[i] - '0';
			if (digit > 9) {
				digit -= in[i] >= 'a' ? 0x27 : 7;
			}
			cur_arg *= base;
			cur_arg += digit;
			arg_digit++;
			valid = 1;
		}
		
		int eol = (in[i] == '\n') | (i >= in_sz-1);
		if (in[i] == ',' || eol) {
			if (arg_digit) arg_count++;

			if (arg_count < arg_len && eol) {
				printf("Error: %d - %s: not enough arguments (got: %d, needed %d)\n", line+1, op_str[op], arg_count, arg_len);
				if (buf) free(buf);
				return -4;
			}

			if (arg_digit < 1 && in[i] == ',') {
				printf("Error: %d - %s: argument %d is empty\n", line+1, op_str[op], arg_count+1);
				if (buf) free(buf);
				return -5;
			}

			if (arg_count > arg_len) {
				printf("Error: %d - %s: too many arguments (got %d, needed %d)\n", line+1, op_str[op], arg_count, arg_len);
				if (buf) free(buf);
				return -6;
			}

			int arg_size = arg_type < 3 ? 4 : (arg_type - 2) * 4;

			if (cur_arg > arg_limits[arg_type-1]) {
				printf("Error: %d - %s: argument %d (%d) is too large (limit: %d)\n", line+1, op_str[op], arg_count+1, cur_arg, arg_limits[arg_type-1]);
				if (buf) free(buf);
				return -7;
			}

			cur_arg *= arg_sign;
			if (arg_format == 0x61 && arg_count == 1) {
				opcode |= cur_arg & 7;
			}
			else {
				int mask = arg_limits[arg_type-1] - 1;
				args |= (cur_arg & mask) << bits_used;
				bits_used += arg_size;
			}

			if (eol) {
				if (!opcode) {
					buf = realloc(buf, ++sz);
					buf[sz-1] = 0;
				}
				else {
					buf = realloc(buf, sz + 3);
					buf[sz] = opcode;
					buf[sz+1] = (u8)(args >> 8);
					buf[sz+2] = (u8)args;
					sz += 3;
				}

				arg_count = arg_format = arg_len = bits_used = args = 0;
				line++;
				mn = 1;
			}

			arg_digit = cur_arg = 0;
			arg_sign = 1;
			base = 10;
			valid = 1;
		}

		if (!valid) {
			printf("Error: %d - %s: Invalid character '%c'\n", line+1, op_str[op], in[i]);
			if (buf) free(buf);
			return -8;
		}

		i++;
	}

	*out = buf;
	*out_sz = sz;
	return 0;
}

int disassemble(u8 **out, int *out_sz, u8 *in, int in_sz) {
	if (!out || !out_sz || !in || in_sz < 1) return -1;

	u8 *buf = NULL;
	char args[32], *ap = args;
	int i = 0, j, b, t, opcode = 0, op = 0, sz = 0;
	while (i < in_sz) {
		opcode = in[i];
		if (!opcode) {
			buf = realloc(buf, sz+4);
			memcpy(buf+sz, "nop\n", 4);
			sz += 4;
			i++;
			continue;
		}
		if (i+3 > in_sz) break;

		op = opcode;
		if (op >= N_IDX_OPS) {
			if (op < HIGH_OPS_BASE) {
				char unk[16] = {0};
				int unk_sz = sprintf(unk, "?? (%02x %02x%02x)\n", op, in[i+1], in[i+2]);
				buf = realloc(buf, sz + unk_sz);
				strcpy(buf+sz, unk);
				sz += unk_sz;
				i += 3;
				continue;
			}
			op = (op - HIGH_OPS_BASE) / 8 + N_IDX_OPS;
		}

		int mn_sz = strlen(op_str[op]);
		buf = realloc(buf, sz + mn_sz + 1);
		memcpy(buf+sz, op_str[op], mn_sz);
		sz += mn_sz + 1;
		buf[sz-1] = ' ';

		int n = 0, args_sz = 0;
		memset(args, 0, 32);
		if (op_type[op][0]) {
			n = (in[i+1] << 8) | in[i+2];
			args_sz = sprintf(args, "r%d,%s%x\n", opcode & 7, n < 10 ? "" : "0x", n);
			buf = realloc(buf, sz + args_sz);
			memcpy(buf+sz, args, args_sz);
			sz += args_sz;
			i += 3;
			continue;
		}

		int nl = 0, first = 1;
		for (j = 4; j >= 0; j--) {
			b = (j+1) / 2;
			t = op_type[op][j];
			if (j < 4 && nl && (t != 5 || !j)) {
				ap += sprintf(ap, "%s%s%x", first ? "" : ",", n < 10 ? "" : "0x", n);
				first = 0;
			}
			if (!t) continue;

			int nyb = in[i+b];
			if (j%2) nyb >>= 4;
			else nyb &= 0xf;

			if (t <= 3 || !j) {
				ap += sprintf(ap, "%sr%d", first ? "" : ",", nyb & 7);
				first = 0;
			}
			if (t == 4) {
				ap += sprintf(ap, "%s%d", first ? "" : ",", nyb);
				first = 0;
			}
			if (t == 5) {
				n |= (nyb << (nl * 4));
				nl++;
			}
		}
		*ap++ = '\n';
		args_sz = ap - args;
		ap = args;

		buf = realloc(buf, sz + args_sz);
		memcpy(buf+sz, args, args_sz);
		sz += args_sz;

		i += 3;
	}

	*out = buf;
	*out_sz = sz;
	return 0;
}