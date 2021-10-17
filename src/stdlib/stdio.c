#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib/stdio.h>

int int_to_str(char * s, size_t n, uint8_t base, int64_t num) {
	size_t length = 0;
	char number[70] = { 0 };
	while (num != 0) {
		number[++length] =  "0123456789abcdef"[num % base];
		num /= base;
	}

	size_t i = 0;
	size_t j = 0;
	for(j = length; j > 0; j--, i++) {
		s[i] = number[j];
		if(i > n) break;   
	}

	return length;
}

int vsnprintf(char * s, size_t n, const char* format, ...) {
	int format_count = 0;
	for(int i = 0; s[i] != 0; i++) {
		if(s[i] == '%' && s[i+1] != '%') format_count++;
	}
	
	va_list list;
	va_start(list, format_count);
	
	char c;
	size_t idx = 0;
	while((c = *format++) != 0) {
		if(idx > n) break;
		if(c != '%') {
			s[++idx] = c;
			continue;
		}

		c = *format++;
		if(c == '%') {
			s[++idx] = c;
		} else if(c == 's') {
			char* arg = va_arg(list, char*);
			char cur;
			while((cur = *arg++) != 0) {
				s[++idx] = cur;
				if(idx > n) break;
			}
		} else if(c == 'd') {
			int64_t arg = va_arg(list, int64_t);
			if(arg < 0) {
				s[++idx] = '-';
			}
			idx += int_to_str(s + idx + 1, n - idx, 10, arg);
		} else if(c == 'x') {
			int64_t arg = va_arg(list, int64_t);
			idx += int_to_str(s + idx + 1, n - idx, 16, arg);
		} else if(c == 'b') {
			int64_t arg = va_arg(list, int64_t);
			idx += int_to_str(s + idx + 1, n - idx, 2, arg);
		}
	}
	s[++idx] = '\0';

	va_end(list);
	return idx;
}