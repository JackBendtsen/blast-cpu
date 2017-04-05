#include "ram.h"

extern u8 **RAM;

void init_ram() {
	int i;
	RAM = calloc(N_RAM, sizeof(u8*));
	for (i = 0; i < N_RAM; i++) RAM[i] = calloc(0x100000, 1);
}

void close_ram() {
	int i;
	for (i = 0; i < N_RAM; i++) free(RAM[i]);
	free(RAM);
}

inline int read_triple(u8 t, int addr) {
	if (addr > 0xffffd) return 0;
	return (RAM[t][addr] << 16) | (RAM[t][addr+1] << 8) | RAM[t][addr+2];
}

inline u16 read_word(u8 t, int addr) {
	if (addr > 0xffffe) return 0;
	u16 word = RAM[t][addr] << 8;
	return word | RAM[t][addr+1];
}

inline u8 read_byte(u8 t, int addr) {
	if (addr > 0xfffff) return 0;
	return RAM[t][addr];
}

inline void write_triple(u8 t, int addr, int triple) {
	if (addr > 0xffffd) return;
	RAM[t][addr] = (u8)(triple >> 16);
	RAM[t][addr+1] = (u8)(triple >> 8);
	RAM[t][addr+2] = (u8)triple;
}

inline void write_word(u8 t, int addr, u16 word) {
	if (addr > 0xffffe) return;
	RAM[t][addr] = (u8)(word >> 8);
	RAM[t][addr+1] = (u8)word;
}

inline void write_byte(u8 t, int addr, u8 byte) {
	if (addr > 0xfffff) return;
	RAM[t][addr] = byte;
}