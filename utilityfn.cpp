#include "utilityfn.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int Utility::IsPowerOf2(unsigned long long int number) {
	unsigned long long int power;
	for (power = 1; power > 0; power = power << 1) {
		if (power == number) {
			return 1;
		}
		if (power > number) {
			return 0;
		}
	}
	return 0;
}

long int Utility::LoadFileIntoBuffer(void ** buffer, char * filename) {
	size_t filesize;
	size_t readsize;
	FILE * file;
	file = fopen(filename, "rb");
	if (file == NULL) {
		return -1;
	}
	fseek(file, 0, SEEK_END);
	filesize = (size_t)ftell(file);
	if (filesize == 0) {
		fclose(file);
		return -1;
	}
	*buffer = malloc(filesize);
	fseek(file, 0, SEEK_SET);
	readsize = fread(*buffer, filesize, 1, file);
	fclose(file);
	if (readsize != 1) {
		free(*buffer);
		*buffer = NULL;
		return -1;
	}
	return (long int)filesize;
}

char* Utility::BinaryToHex(void* ptr, size_t size) {
	char* outp;
	outp = (char*)malloc(size * 2 + 1);
	unsigned char* inp;
	inp = (unsigned char*)ptr;
	int currpos = 0;
	char hexchars[] = "0123456789abcdef";
	for (size_t i = 0; i < size; i++) {
		outp[currpos++] = hexchars[inp[i] >> 4 & 0xf];
		outp[currpos++] = hexchars[inp[i] & 0xf];
	}
	outp[currpos] = '\0';
	return outp;
}

static int hex2bin(const char* s) {
	int ret = 0;
	int i;
	for(i = 0; i < 2; i++) {
		char c = *s++;
		int n = 0;
		if('0' <= c && c <= '9') { n = c-'0'; }
		else if('a' <= c && c <= 'f') { n = 10 + c - 'a'; }
		else if('A' <= c && c <= 'F') { n = 10 + c - 'A'; }
		ret = n + (ret << 4);
	}
	return ret;
}

int Utility::HexToBinary(char* string, void** result, int* olen) {
	int len = strlen(string);
	if (len % 2 != 0) {
		return -1;
	}
	unsigned char* outp;
	int pos = 0;
	outp = (unsigned char*)malloc(len/2);
	for (int i = 0; i < len; i += 2) {
		outp[pos++] = hex2bin(string + i);
	}
	*result = (void*)outp;
	*olen = pos;
	return 0;
}
