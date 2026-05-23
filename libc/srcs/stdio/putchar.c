#include <stdio.h>
#include "terminal.h"


int putchar(int ic) {
	char c = (char) ic;
	terminal_write(&c, sizeof(c));
	return ic;
}
