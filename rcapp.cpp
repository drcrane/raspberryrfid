#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "readcard.h"
#include "utilityfn.hpp"

/*
 * Simple programme that will return the ID read from the card to stdout
 * Probably useful in a shell script ;-)
 */

int main(int argc, char *argv[]) {
	int i;
	int res;
	unsigned char buf[50];
	char* ascii;
	int sz;

	sz = sizeof(buf);
	for (i = 0; i < 10; i++) {
		res = ReadCard(buf, (size_t*)&sz);
		if (res == 0) {
			ascii = Utility::BinaryToHex(buf, sz);
			fprintf(stdout, "%s\n", ascii);
			free(ascii);
			break;
		}
	}
	return res;
}
