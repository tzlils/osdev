#include <stdarg.h>
#include <stddef.h>
#include <stdlib/stdio.h>

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
			int arg = va_arg(list, int);
			char numbers[10] = "0123456789";
			while (arg != 0) {
				s[++idx] = numbers[arg % 10];
				arg /= 10;
				if(idx > n) break;
			}
		}
	}
	s[++idx] = '\0';

	va_end(list);
	return idx;
}