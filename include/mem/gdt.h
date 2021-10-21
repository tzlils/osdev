#pragma once

#include <stdint.h>

/*
	using 0x58 because 9 gdt entries + 2 tss entries times 8 bytes
	11*8 = 0x58
*/
typedef struct __attribute__((__packed__))
{
	uint16_t num_entries;
	uint64_t *entries;
} gdtr_t;

void set_gdt(gdtr_t gdt);