#pragma once
static inline void cpuid(int code, uint32_t *a, uint32_t *d);
static inline int cpuid_string(int code, uint32_t where[4]);