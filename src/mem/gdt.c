
#include <stdint.h>

uint64_t gdt_entries[9] = {
	0x0000000000000000, // null
	/*
	note: should be able to get rid of {code,data}{16,32} once no longer using stivale terminal
	*/
	0x00009a000000ffff, // code16
	0x000093000000ffff, // data16
	0x00cf9a000000ffff, // code32
	0x00cf93000000ffff, // data32
	0x00af9b000000ffff, // code64
	0x00af93000000ffff, // data64
	0x00affb000000ffff, // usrc64
	0x00aff3000000ffff  // usrd64
};

/*
	using 0x58 because 9 gdt entries + 2 tss entries times 8 bytes
	11*8 = 0x58
*/
struct __attribute__((__packed__)) { uint16_t num_entries; uint64_t* entries; } gdt = { 0x58, gdt_entries };

static inline void init_gdt(uint64_t gdt) {
  asm volatile("lgdt": "=a" (gdt) );
}