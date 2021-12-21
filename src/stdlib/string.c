#include <stdlib/string.h>
#include <stdint.h>

void memset(void *ptr, int value, size_t num)
{
	for (size_t i = (size_t)ptr; i < (size_t)ptr + num; i++)
		*(int *)i = value;
}

void memcpy(void *dest, void* src, size_t size) {
	while(size--) {
		*(uint64_t*)(dest++) = *(uint64_t*)(src++); 
	}
}