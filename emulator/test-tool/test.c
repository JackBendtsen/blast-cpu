#include "../cpu.h"
#include "../ram.h"

void print_buf(u8 *buf, int sz) {
	if (!buf || sz < 1) return;
	int i;
	printf("00 | ");
	for (i = 0; i < sz; i++) {
		if (i && i % 8 == 0) printf("\n%02x | ", i);
		printf("%02x ", buf[i]);
	}
}

int main(int argc, char **argv) {
	init_ram();
	regs *r = calloc(1, sizeof(regs));
	char cmd[16], *p;
	while (1) {
		putchar('\n');
		memset(cmd, 0, 16);
		p = NULL;

		fgets(cmd, 16, stdin);
		if (cmd[0] == 'q') break;
		else if (cmd[0] == 'p') print_buf((u8*)r, sizeof(regs));
		else if (cmd[0] == 'r') decode(r);
		else if (cmd[0] == 'x') execute(r);

		else if (cmd[0] == 'm') {
			int addr = strtol(cmd+2, NULL, 16), sz = 16;
			if (addr < 0 || addr >= 0x100000) continue;
			if (addr > 0xffff0) sz = 0x100000 - addr;
			print_buf(RAM[0]+addr, sz);
		}
		else {
			u8 idx = (u8)strtol(cmd, &p, 16);
			u8 byte = (u8)strtol(p, NULL, 16);
			if (idx >= sizeof(regs)) continue;
			((u8*)r)[idx] = byte;
		}
	}
	free(r);
	close_ram();
	return 0;
}