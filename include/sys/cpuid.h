#pragma once
void cpuid(int code, uint32_t *a, uint32_t *d);
int cpuid_string(int code, uint32_t where[4]);