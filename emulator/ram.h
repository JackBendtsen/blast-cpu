#ifndef RAM_H
#define RAM_H

#include "blast.h"

#define N_RAM 1

u8 io_mem[2048];
u8 **RAM;

void init_ram();
void close_ram();
int read_triple(u8 t, int addr);
u16 read_word(u8 t, int addr);
u8 read_byte(u8 t, int addr);
void write_triple(u8 t, int addr, int triple);
void write_word(u8 t, int addr, u16 word);
void write_byte(u8 t, int addr, u8 byte);

#endif
