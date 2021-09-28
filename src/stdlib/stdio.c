#include <stdarg.h>
#include <stddef.h>
#include <stdlib/stdio.h>

void int_to_str(char * s, size_t n, int base, int64_t num) {
	int length = 0;
	char number[70] = { 0 };
	while (arg != 0) {
		number[++length] =  "0123456789abcdef"[arg % base];
		arg /= base;
	}

	while(int i = length; length > 0; length--) {
		number[i] =  "0123456789abcdef"[arg % base];
		if(idx > n) break;
	}
}

int vsnprintf(char * s, size_t n, const char* format, ...) {
	int format_count = 0;
	for(int i = 0; s[i] != 0; i++) {
		if(s[i] == '%') format_count++;
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
		};
		c = *format++;
		if(c == 's') {
			char* arg = va_arg(list, char*);
			char cur;
			while((cur = *arg++) != 0) {
				s[++idx] = cur;
				if(idx > n) break;
			}
		} else if(c == 'd') {
			int64_t arg = va_arg(list, int64_t);
			int_to_str(s + idx, n - idx, 10, arg);
		} else if(c == 'x') {
			int64_t arg = va_arg(list, int64_t);
			int_to_str(s + idx, n - idx, 16, arg);
		}
	}
	s[++idx] = '\0';

	va_end(list);
	return idx;
}