#ifndef BLAST_H
#define BLAST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define be16(x, y) (((x) << 8) | (y))
#define be24(x, y, z) (((x) << 16) | ((y) << 8) | (z))

#define MEM_SIZE 0x100000
#define SMALL_MEM 0x10000

typedef unsigned char u8;
typedef unsigned short u16;

#endif
