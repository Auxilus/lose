#include "string.h"
#include <stddef.h>

#define true	1
#define false 0

int strlen(char *string)
{
	int i;
	for (i=0;string[i] != '\0'; ++i);
	return i;
}

void itoa(int n, char str[]) {
	int i, sign;
	if ((sign = n) < 0) n = -n;
	i = 0;
	do {
		str[i++] = n % 10 + '0';
	} while ((n /= 10) > 0);

	if (sign < 0) str[i++] = '-';
	str[i] = '\0';

	reverse(str);
}

/* K&R */
void reverse(char s[]) {
	int c, i, j;
	for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

char*  strcat(char* destination, const char* source)
{
       // return if no memory is allocated to the destination
       if (destination == NULL) {
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
