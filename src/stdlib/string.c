#include <stdlib/string.h>

void memset(void *ptr, int value, size_t num)
{
	for (size_t i = (size_t)ptr; i < (size_t)ptr + num; i++)
		*(int *)i = value;
}