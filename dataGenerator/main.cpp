#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <cstdlib>

void usage() {
	std::cout << "usage: dataGen vectorLength vectorCount dist1VecMinCount outputFile\n";
	exit(0);
}

void getArguments(int argc, char* argv[], unsigned int& vecLen, unsigned int& vecCount, unsigned int& dst1Count, std::string& outputFile) {
	if (argc != 5)
		usage(); 
	vecLen = atoi(argv[1]);
	vecCount = atoi(argv[2]);
	dst1Count = atoi(argv[3]);
	outputFile = argv[4];
	if (vecCount == 0 || vecLen == 0)
		usage();
}

char* generateRandomVector(unsigned int length) {
	unsigned int bitsLeft = length;
	int lenInBytes = std::ceil(double(length) / 8.0);
	char* vector = new char[lenInBytes];
	for (int i = lenInBytes - 1; i >= 0; i--)
	{
		vector[i] = rand() % 256;
		if (bitsLeft > 8)
			bitsLeft -= 8;
	}
	char mask = 255 >> (8 - bitsLeft);
	vector[0] = vector[0] & mask;
	return vector;
}

//Swaps one bit in vector and returns a new copy
char* generateDst1Vector(char* vector, unsigned int length) {
	int bitToSwap = rand() % length;
	int byteWithBitToSwap = bitToSwap / 8;
	int lenInBytes = std::ceil(double(length) / 8.0);
	char* newVector = new char[lenInBytes];
	bitToSwap = bitToSwap % 8;
	vector[lenInBytes - 1 - byteWithBitToSwap] = vector[lenInBytes - 1 - byteWithBitToSwap] ^ 1 << bitToSwap;
	memcpy(newVector, vector, lenInBytes);
	return newVector;
}

char* int2bytes(unsigned int n) {
	char* bytes = new char[4];
	bytes[0] = (n >> 24) & 0xFF;
	bytes[1] = (n >> 16) & 0xFF;
	bytes[2] = (n >> 8) & 0xFF;
	bytes[3] = n & 0xFF;
	return bytes;
}
void printBinary(char c) {
	for (int i = 7; i >= 0; --i)
	{
		std::cout << ((c & (1 << i)) ? '1' : '0');
	}
}
void printBytes(char* c, int l) {
	for (int i = 0; i < l; i++)
	{
		printBinary(c[i]);
		std::cout << " ";
	}
	std::cout << "\n";
}

int main(int argc, char* argv[]) {
	unsigned int vectorLen = 0, vectorCount = 0, dst1Count = 0;
	std::string outputFile = "output";
	srand(time(NULL));

	getArguments(argc, argv, vectorLen, vectorCount, dst1Count, outputFile);

	std::ofstream output;
	output.open(outputFile, std::ios::out | std::ios::trunc | std::ios::binary);

	int lenInBytes = std::ceil(double(vectorLen) / 8.0);

	int dst1Stride = ceil(vectorCount / double(dst1Count));

	if (dst1Stride == 0)
		dst1Stride = vectorCount + 1;

	char* lastDst1Vector = generateRandomVector(vectorLen);

	output.write(int2bytes((int)vectorCount), 4);
	output.write(int2bytes((int)vectorLen), 4);

	for (unsigned int i = 0; i < vectorCount; i++)
	{
		char* vec;
		if (i % dst1Stride == 0)
		{
			vec = generateDst1Vector(lastDst1Vector, vectorLen);
		}
		else
			vec = generateRandomVector(vectorLen);

		output.write(vec, lenInBytes);
		delete[] vec;
	}
	delete[] lastDst1Vector;
	output.close();
	return 0;
}