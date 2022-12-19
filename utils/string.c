#include "string.h"

#define true	1
#define false 0

int strlen(char *string)
{
	int i;
	for (i=0;string[i] != '\0'; ++i);
	return i;
}

