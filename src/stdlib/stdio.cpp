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

template <uint64_t n>
int format(char *out, size_t max_size)
{
	return int_to_str(out, max_size, 10, n);
}

template <typename T>
int format_arg(const T &arg, size_t max_size)
{
	return format<T>(arg, max_size);
}

template <typename... Args>
int snprintf(char *out, int max_size, const char *format, Args &&...args)
{
	size_t idx = 0;
	const auto handle_arg = [&](auto &arg)
	{
		while (*format)
		{
			if (*format == '\\')
			{
				out[++idx] = '\\';
				format += 2;
				continue;
			}
			if (*format != '{')
			{
				out[++idx] = *format++;
				continue;
			}

			while (*format != '}')
			{
				idx += format_arg(arg, max_size);
			}
		}
	}

	(handle_arg(args), ...);
	while (*format)
	{
		out[++idx] = *format++;
	}
	return idx;
}

/*
int vsnprintf(char *s, size_t max_size, const char *format, ...)
{
	int format_count = 0;
	for (int i = 0; s[i] != 0; i++)
	{
		if (s[i] == '%' && s[i + 1] != '%')
			format_count++;
	}

	va_list list;
	va_start(list, format_count);

	char c;
	size_t idx = 0;
	while ((c = *format++) != 0)
	{
		if (idx > max_size)
			break;
		if (c != '%')
		{
			s[++idx] = c;
			continue;
		}

		c = *format++;

		// handle length formats like %.16s
		size_t format_length = 0;
		if (c == '.')
		{
			c = *format++;
			while ((c >= 48) && (c <= 57))
			{
				format_length = format_length * 10 + (c - '0');
				c = *format++;
			}
			format_length += idx - 1;
		}
		else
		{
			format_length = max_size;
		}

		if (c == '%')
		{
			s[++idx] = c;
		}
		else if (c == 's')
		{
			char *arg = va_arg(list, char *);
			char cur;
			while ((cur = *arg++) != 0)
			{
				s[++idx] = cur;
				if (idx > format_length)
					break;
			}
		}
		else if (c == 'd')
		{
			int64_t arg = va_arg(list, int64_t);
			if (arg < 0)
			{
				s[++idx] = '-';
			}
			idx += int_to_str(s + idx + 1, format_length - idx, 10, arg);
		}
		else if (c == 'x')
		{
			int64_t arg = va_arg(list, int64_t);
			idx += int_to_str(s + idx + 1, format_length - idx, 16, arg);
		}
		else if (c == 'b')
		{
			int64_t arg = va_arg(list, int64_t);
			idx += int_to_str(s + idx + 1, format_length - idx, 2, arg);
		}
	}
	s[++idx] = '\0';

	va_end(list);
	return idx;
}
*/