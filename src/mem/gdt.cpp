
#include <stdint.h>
#include <mem/gdt.h>

void set_gdt(gdtr_t gdt) {
  asm volatile("lgdt %0" : : "m"(gdt) );
}