#pragma once
#include <stdarg.h>
#include <stddef.h>

template<class ...Ts> 
size_t snprintf(char out[], size_t max_size, const char* format, Ts... args);
// template <size_t max_size, typename... Args>
// int snprintf(char *out, const char *format, Args &&...args);
// int vsnprintf(char *s, size_t n, const char *format, ...);
