#include "cpu.h"

void strset(char *str, char c) {
	int i;
	for (i = 0; str[i]; i++) str[i] = c;
}

int iacmp(int *arr, int n, int len) {
	if (!arr) return -1;
	int i;
	for (i = 0; i < len; i++) if (arr[i] != n) return 1;
	return 0;
}

void disasm(regs *r, char *args) {
	strset(args, 0);
	char temp[10] = {0};
	int num = 0;
	if (ops[r->cmd].dec[0] && !iacmp(&ops[r->cmd].dec[1], 5, 4)) {
		num = (u16)r->insr;
		sprintf(args, "r%d,%s%x", r->cmd & 7, num < 10 ? "" : "0x", num);
		return;
	}
	int i, s = 0, n = 0, shift = 0, type;
	for (i = 4; i >= 0; i--, shift += 4) {
		type = ops[r->cmd].dec[i];
		if (n && (type != 5 || i == 0)) {
			sprintf(temp, "%s0x%x", s ? "," : "", num);
			strcat(args, temp);
		}
		strset(temp, 0);
		switch (type) {
			case 1:
			case 2:
			case 3:
				if (i == 0) sprintf(temp, "r%d", r->cmd & 7);
				else sprintf(temp, "%sr%d", s ? "," : "", (r->insr >> shift) & 7);
				strcat(args, temp);
				s++;
				break;
			case 4:
				if (i == 0) sprintf(temp, "%d", r->cmd & 0xf);
				else sprintf(temp, "%s%d", s ? "," : "", (r->insr >> shift) & 0xf);
				strcat(args, temp);
				s++;
				break;
			case 5:
				num |= ((r->insr >> shift) & 0xf) << (n++ * 4);
				break;
		}
	}
}

void read_line(char *str) {
	while (1) {
		char c = getchar();
		if (c == '\n' || c == EOF) break;
		*str++ = c;
	}
}

void pause(regs *r) {
	int i;
	printf("\n ***BREAK***\n\n");
	for (i = 0; i < 4; i++) printf("r%d: %04X  - r%d: %04X\n", i, r->r[i], i+4, r->r[i+4]);
	printf("\nPC: 0x%06X, AR: 0x%06X, LR: 0x%06X,\n"
		"TAR: 0x%06X, ITR: 0x%04X, OV: %X\n"
		"DW0: 0x%04X, DW1: 0x%04X, DW2: 0x%04X, DW3: 0x%04X\n\n", r->pc, r->ar, r->lr, r->ta, r->itr, r->ov, r->dw[0], r->dw[1], r->dw[2], r->dw[3]);
	char str[20] = {0};
	disasm(r, str);
	printf("%06X -> %s %s | %02X %04X\n\nType Q to quit, S to step to the next instruction or Enter to continue ", r->pc-3, ops[r->cmd].name, str, r->cmd, (u16)r->insr);
	memset(str, 0, 20);
	read_line(str);
	if (str[0] == 's') r->step = 1;
	else if (str[0] == 'q') r->step = 2;
}
