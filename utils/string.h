#ifndef _UTILS_STRING
#define _UTILS_STRING

#include <stdarg.h>
#include <stddef.h>

int		strlen(char *string);
char*	strcat(char* destination, const char* source);
void	itoa(int n, char str[]);
long	atoi(const char *s);
void	reverse(char s[]);

int snprintf(char* str, size_t size, const char* format, ...);
int sprintf(char* str, const char* format, ...);
int sscanf(const char* s, const char* format, ...);
int vsnprintf(char* str, size_t size, const char* format, va_list ap);
int vsprintf(char* str, const char* format, va_list ap);
int vsscanf(const char* s, const char* format, va_list ap);
#endif
