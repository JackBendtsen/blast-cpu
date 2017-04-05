#include <stdio.h>
#include <stdlib.h>
#include "opcodes.h"

int main(int argc, char **argv) {
	if (argc != 4) {
		printf("Invalid arguments\n"
			"Usage: %s <option> <input file> <output file>\n"
			"Options:\n"
			" -a\n"
			"    Assemble the input assembly file and output a binary file\n"
			" -d\n"
			"    Disassemble the input binary file and output an assembly file\n", argv[0]);
		return 1;
	}
	FILE *f = fopen(argv[2], "r");
	if (!f) {
		printf("Could not open \"%s\"\n", argv[2]);
		return 2;
	}

	fseek(f, 0, SEEK_END);
	int in_sz = ftell(f);
	rewind(f);
	if (in_sz < 1) {
		printf("\"%s\" is too small to be opened\n", argv[2]);
		return 3;
	}

	u8 *in = malloc(in_sz);
	fread(in, 1, in_sz, f);
	fclose(f);

	u8 *out = NULL;
	int out_sz = 0;

	if (argv[1][1] == 'a') {
		if (assemble(&out, &out_sz, in, in_sz) < 0) {
			free(in);
			return 4;
		}
	}
	else if (argv[1][1] == 'd') {
		if (disassemble(&out, &out_sz, in, in_sz) < 0) {
			free(in);
			return 5;
		}
	}
	else {
		printf("Invalid option \"%s\"\n", argv[1]);
		return 6;
	}

	f = fopen(argv[3], "wb");
	fwrite(out, 1, out_sz, f);
	fclose(f);

	free(in);
	free(out);

	return 0;
}
