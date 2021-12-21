#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib/stdio.h>

int int_to_str(char *s, size_t n, uint8_t base, int64_t num)
{
	size_t length = 0;
	char number[70] = {0};
	while (num != 0)
	{
		number[++length] = "0123456789abcdef"[num % base];
		num /= base;
	}

	size_t i = 0;
	size_t j = 0;
	for (j = length; j > 0; j--, i++)
	{
		s[i] = number[j];
		if (i > n)
			break;
	}

	return length;
}

template<class T> size_t specialized_printer(char* out, size_t max_size, T in)
{
	return specialized_printer(out, max_size, "{I DON'T KNOW THIS TYPE}");
};

template<> size_t specialized_printer<const char*>(char* out, size_t max_size, const char* in) {
	size_t i = 0;
	for(; i < max_size && in[i] != '\0'; i++) {
		out[i] = in[i];
	}
	return i;
}

template<> size_t specialized_printer<char*>(char* out, size_t max_size, char* in) {
	return specialized_printer(out, max_size, (const char*)in);
}

template<> size_t specialized_printer<int>(char* out, size_t max_size, int in)  {
	return int_to_str(out, max_size, 10, in);
}

template<> size_t specialized_printer<bool>(char* out, size_t max_size, bool in) {
	return specialized_printer(out, max_size, in ? "true" : "false");
}

template<class ...Ts> 
size_t snprintf(char out[], size_t max_size, const char* format, Ts... args) {
	size_t index = 0;
	size_t fmt_index = 0;
	auto printer = [&](auto arg) {
		if(index >= max_size) return;
		for(; format[fmt_index] != '\0'; fmt_index++) {
			if(format[fmt_index] == '%') {
				fmt_index++;
				break;
			};
			out[++index] = format[fmt_index];
		}

		size_t size = specialized_printer(out+index, max_size-index, arg);
		index += size;
		return;
	};

	((printer(args)), ...);
	for(; format[fmt_index] != '\0'; fmt_index++) {
		out[index++] = format[fmt_index];
	}
	out[index] = '\0';
	return index;
}