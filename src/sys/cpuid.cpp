module;
#include <stdint.h>

export module cpuid;
namespace cpuid {
	export inline void cpuid(int code, uint32_t *a, uint32_t *d) {
		asm volatile("cpuid":"=a"(*a),"=d"(*d):"a"(code):"ecx","ebx");
	}

	export inline char* cpuid_string(int code, char where[4]) {
	asm volatile("cpuid":"=a"(*where),"=b"(*(where+1)),
				"=c"(*(where+2)),"=d"(*(where+3)):"a"(code));
	return &where[0];
	}
}