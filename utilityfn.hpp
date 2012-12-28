#include <stdio.h>

class Utility {
	public:
		static int IsPowerOf2(unsigned long long int number);
		static long int LoadFileIntoBuffer(void** buffer, char* filename);
		static char* BinaryToHex(void* ptr, size_t size);
		static int HexToBinary(char* string, void** result, int* len);
};
