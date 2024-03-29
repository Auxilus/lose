#include "string.h"
#include "mem.h"
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>

#define true 1
#define false 0

int strlen(char *string)
{
	int i;
	for (i = 0; string[i] != '\0'; ++i)
		;
	return i;
}

void itoa(int n, char str[])
{
	int i, sign;
	if ((sign = n) < 0)
		n = -n;
	i = 0;
	do
	{
		str[i++] = n % 10 + '0';
	} while ((n /= 10) > 0);

	if (sign < 0)
		str[i++] = '-';
	str[i] = '\0';

	reverse(str);
}

long atoi(const char *s)
{
	long num = 0;

	int i = 0;

	// run till the end of the string is reached, or the
	// current character is non-numeric
	while (s[i] && (s[i] >= '0' && s[i] <= '9'))
	{
		num = num * 10 + (s[i] - '0');
		i++;
	}

	return num;
}

/* K&R */
void reverse(char s[])
{
	int c, i, j;
	for (i = 0, j = strlen(s) - 1; i < j; i++, j--)
	{
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

int strcmp(const char *str1, const char *str2)
{
	unsigned int len1 = strlen((char *)str1);
	unsigned int len2 = strlen((char *)str2);

	int cmpResult = memcmp(str1, str2, (len1 < len2) ? len1 : len2);
	if (cmpResult != 0)
		return cmpResult;

	if (len1 > len2)
		return 1;
	else if (len1 < len2)
		return -1;

	return 0;
}

char *strcat(char *destination, const char *source)
{
	// return if no memory is allocated to the destination
	if (destination == NULL)
	{
		return NULL;
	}

	// take a pointer pointing to the beginning of the destination string
	char *ptr = destination;
	destination += strlen(destination);

	// copy the C-string pointed by source into the array
	// pointed by destination
	while (*source != '\0')
	{
		*destination = *source;
		destination++;
		source++;
	}

	// include the terminating null character
	*destination = '\0';

	// the destination is returned by standard `strcpy()`
	return ptr;
}

/* TEST: string functions */

int snprintf(char *str, size_t size, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	int n = vsnprintf(str, size, format, ap);
	va_end(ap);
	return n;
}

int sprintf(char *str, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	int n = vsprintf(str, format, ap);
	va_end(ap);
	return n;
}

int sscanf(const char *s, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	int n = vsscanf(s, format, ap);
	va_end(ap);
	return n;
}

int vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
	// TODO: 'size' is not handled
	// TODO: 'e', 'f', 'g', 'n' specifiers to do
	// TODO: function doesn't return the number of printed characters

	if (size == 0)
		return 0;

	// we loop through each character of the format
	while (*format != '\0' && size > 1)
	{
		// first we handle the most common case: a normal character
		if (*format != '%')
		{
			*str++ = *format++;
			continue;
		}

		// then we check if format is "%%"
		format++;
		if (*format == '%')
		{
			*str++ = '%';
			format++;
			continue;
		}

		// now we are sure we are in a special case
		// what we do is that we store flags, width, precision, length in variables
		bool sharpFlag = false;
		bool alignLeft = false;
		bool alwaysSign = false;
		bool noSign = false;
		char padding = ' ';
		int minimumWidth = 0;
		int precision = 1;
		bool numberMustBeShort = false;
		bool numberMustBeLong = false;
		bool unsignedNumber = false;
		bool capitalLetters = false;
		bool octal = false;
		bool hexadecimal = false;
		bool pointer = false;
		bool tagFinished = false;

		// then we loop (and we modify variables) until we find a specifier
		do
		{

			switch (*format)
			{
			// flags
			case '-':
				alignLeft = true;
				format++;
				break;
			case '+':
				alwaysSign = true;
				format++;
				break;
			case ' ':
				noSign = true;
				format++;
				break;
			case '0':
				padding = '0';
				format++;
				break;
			case '#':
				sharpFlag = true;
				format++;
				break;

				// width
			case '1':
			case '2':
			case '3':
			case '4':
			case '5': // width cannot start with 0 or it would be a flag
			case '6':
			case '7':
			case '8':
			case '9':
				minimumWidth = atoi(format);
				while (*format >= '0' && *format <= '9')
					format++;
				break;
			case '*':
				minimumWidth = va_arg(ap, int);
				format++;
				break;

				// precision
			case '.':
				format++;
				if (*format == '*')
				{
					precision = va_arg(ap, int);
					format++;
				}
				else if (*format >= '0' && *format <= '9')
				{
					precision = atoi(format);
					while (*format >= '0' && *format <= '9')
						format++;
				}
				else
				{
					precision = 0; // this behavior is standardized
				}
				break;

				// length
			case 'h':
				numberMustBeShort = true;
				format++;
				break;
			case 'l':
			case 'L':
				numberMustBeLong = true;
				format++;
				break;

				// specifiers

				//	strings
			case 's':
			{
				char *nStr = va_arg(ap, char *);
				size_t len = strlen(nStr);

				if (!alignLeft && len < minimumWidth)
				{
					while (len++ < minimumWidth)
						*str++ = padding;
				}

				while (*nStr)
					*str++ = *nStr++;

				if (alignLeft && len < minimumWidth)
				{
					while (len++ < minimumWidth)
						*str++ = padding;
				}

				format++;
				tagFinished = true;
				break;
			}

				// 	characters
			case 'c':
			{
				char toWrite;
				/*if (numberMustBeLong)		toWrite = (char)va_arg(ap, wchar_t);
					else					*/
				toWrite = (char)va_arg(ap, int);

				if (!alignLeft)
				{
					for (; minimumWidth > 1; minimumWidth--)
						*str++ = padding;
				}

				*str++ = toWrite;

				if (alignLeft)
				{
					for (; minimumWidth > 1; minimumWidth--)
						*str++ = padding;
				}

				format++;
				tagFinished = true;
				break;
			}

				// 	numbers
			case 'o':
				octal = true;
			case 'p':
				pointer = true;
			case 'X':
				capitalLetters = true;
			case 'x':
				hexadecimal = true;
			case 'u':
				unsignedNumber = true;
			case 'd':
			case 'i':
			{
				// first we handle problems with our switch-case
				if (octal)
				{
					pointer = false;
					hexadecimal = false;
					unsignedNumber = false;
				}

				// then we retreive the value to write
				unsigned long int toWrite;
				if (numberMustBeLong)
					toWrite = va_arg(ap, long int);
				else if (numberMustBeShort)
					toWrite = (short int)va_arg(ap, int);
				else if (pointer)
					toWrite = (unsigned long int)va_arg(ap, void *);
				else
					toWrite = va_arg(ap, int);

				// handling sign
				if (!noSign)
				{
					bool positive = (unsignedNumber || (((signed)toWrite) > 0));
					if (alwaysSign || !positive)
						*str++ = (positive ? '+' : '-');
					if (!unsignedNumber && (((signed)toWrite) < 0))
						toWrite = -((signed)toWrite);
				}

				if (sharpFlag && toWrite != 0)
				{
					if (octal || hexadecimal)
						*str++ = '0';
					if (hexadecimal)
					{
						if (capitalLetters)
							*str++ = 'X';
						else
							*str++ = 'x';
					}
				}

				// writing number
				int digitSwitch = 10;
				if (hexadecimal)
					digitSwitch = 16;
				else if (octal)
					digitSwitch = 8;

				// this variable will be usefull
				char *baseStr = str;

				int numDigits = 0;
				do
				{
					if (numDigits)
						memmove(baseStr + 1, baseStr, numDigits * sizeof(char));
					int modResult = toWrite % digitSwitch;
					if (modResult < 10)
					{
						*baseStr = '0' + modResult;
						str++;
					}
					else if (capitalLetters)
					{
						*baseStr = 'A' + (modResult - 10);
						str++;
					}
					else
					{
						*baseStr = 'a' + (modResult - 10);
						str++;
					}
					toWrite /= digitSwitch;
					numDigits++;
				} while (toWrite != 0);

				if (numDigits < minimumWidth)
				{
					minimumWidth -= numDigits;
					if (alignLeft)
					{
						for (; minimumWidth > 0; minimumWidth--)
							*str++ = padding;
					}
					else
					{
						memmove(baseStr + minimumWidth * sizeof(char), baseStr, numDigits * sizeof(char));
						memset(baseStr, padding, minimumWidth * sizeof(char));
						str += minimumWidth;
					}
				}

				// finished
				format++;
				tagFinished = true;
				break;
			}

			default:
				format++;
				tagFinished = true;
				break;
			}
		} while (!tagFinished);
	}

	*str = '\0';

	return 1;
}

int vsprintf(char *str, const char *format, va_list ap)
{
	return vsnprintf(str, (size_t)-1, format, ap);
}

int vsscanf(const char *s, const char *format, va_list ap)
{
}

size_t trimwhitespace(char *out, size_t len, char *str)
{
	if (len == 0)
		return 0;

	const char *end;
	size_t out_size;

	// Trim leading space
	while ((unsigned char)*str == ' ')
		str++;

	if (*str == 0) // All spaces?
	{
		*out = 0;
		return 1;
	}

	// Trim trailing space
	end = str + strlen(str) - 1;
	while (end > str && (unsigned char)*end == ' ')
		end--;
	end++;

	// Set output size to minimum of trimmed string length and buffer size minus 1
	out_size = (end - str) < len - 1 ? (end - str) : len - 1;

	// Copy trimmed string and add null terminator
	memcpy((u8*)str, out, out_size);
	out[out_size] = 0;

	return out_size;
}
