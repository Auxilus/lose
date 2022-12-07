#include "string.h"

int strlen(char *string)
{
	int i;
	for (i=0;string[i] != '\0'; ++i);
	return i;
}
