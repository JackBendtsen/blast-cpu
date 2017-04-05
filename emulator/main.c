#include "cpu.h"
#include "ram.h"
#include <time.h>

#define DEBUG 1
#define TIMER 2
//typedef unsigned long long int u64;

char *opt[] = {"debug", "timer", "bp"};

extern u8 **RAM;
//extern u8 **ROM;
extern const char *op_str[];

void debug(regs *r, u8 testflags, int bp) {
	if (r->step || ((testflags & 4) && bp == r->pc-3)) {
		r->step = 0;
		pause(r);
	}
	else if (r->dbg) {
		char args[20] = {0};
		disasm(r, args);
		printf("PC: 0x%06X, Op: %02X %04X - %s %s\n", r->pc, r->cmd, (u16)r->insr, ops[r->cmd].name, args);
	}
}

int main(int argc, char **argv) {
	printf("Blast CPU Emulator v0.1\n");
	if (argc < 2) {
		printf("Invalid arguments\n"
			"Usage: %s <Name of Blast binary> [Optional Extras]\n"
			"Optional Args:\n"
			"\t\"debug\"       - Prints debug information\n"
			"\t\"timer\"       - Prints time elapsed, number of cycles executed,\n"
			"\t                    and average clock speed at termination\n"
			"\t\"bp <offset>\" - Sets an execute breakpoint at <offset>\n", argv[0]);
		return 1;
	}

	FILE *f = fopen(argv[1], "rb");
	if (!f) {
		printf("\"%s\" doesn't exist\n", argv[1]);
		return 2;
	}

	int i, j = 0;
	u8 testflags = 0;
	int bp = 0;
	if (argc > 2) {
		for (i = 2; i < argc; i++) {
			for (j = 0; j < 3; j++) {
				if (!strcmp(argv[i], opt[j])) {
					testflags |= 1 << j;
					if (j == 2) {
						if (i > argc-2) testflags &= ~4;
						else bp = strtol(argv[++i], NULL, 16);
					}
				}
			}
		}
	}

	init_ram();

#if 0
	ROM_HEADER *header = open_rom(f);
	if (!header) return 3;
#else
	fseek(f, 0, SEEK_END);
	int sz = ftell(f);
	rewind(f);
	if (sz < 1 || sz > 0x100000) return 3;

	u8 *buf = malloc(sz);
	fread(buf, 1, sz, f);
	fclose(f);

	memcpy(RAM[0], buf, sz);
	free(buf);
#endif

	regs *r = calloc(1, sizeof(regs));

	unsigned long long int cycles = 0;
	int on = 1;
	clock_t t1;
	if (testflags & 1) r->dbg = 1;
	if (testflags & 2) t1 = clock();

	while (on) {
		while (1) {
			cpu_tick(r);
			cycles++;
			if (r->state == 2) debug(r, testflags, bp);
			if ((r->itr & 7) == 7 || r->step == 2) {
				on = 0;
				break;
			}
		}
	}

	if (testflags & TIMER) {
		clock_t t2 = clock();
		printf("%lld cycles in %g seconds\n", cycles, (float)(t2-t1) / 1000000.0);
	}

	f = fopen("mem.bin", "wb");
	fwrite(RAM[0], 1, MEM_SIZE, f);
	fclose(f);
	f = fopen("regs.bin", "wb");
	fwrite(r, 1, sizeof(regs), f);
	fclose(f);

	free(r);
//	free(header);
	close_ram();
//	close_rom();
	return 0;
}
