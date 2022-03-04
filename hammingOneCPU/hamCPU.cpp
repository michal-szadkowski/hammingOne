#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <stdint.h>
#include <chrono>
#include <iomanip>
#include "fileIO.h"

const __int8_t bitCount[256] = {
	0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};

void usage()
{
	std::cout << "usage: hamCPU inputfile [outputfile]\n";
	exit(0);
}

void getArguments(int argc, char *argv[], std::string &outputFile, char *&vectors, unsigned int &vecCount, unsigned int &vecLen)
{
	if (argc < 2 || argc > 3)
		usage();

	vectors = readFile(argv[1], vecCount, vecLen);
	if (vectors == NULL)
		usage();
	if (argc >= 3)
		outputFile = argv[2];
}

void printBinary(char c)
{
	for (int i = 7; i >= 0; --i)
	{
		std::cout << ((c & (1 << i)) ? '1' : '0');
	}
}
void printBytes(char *c, int l)
{
	for (int i = 0; i < l; i++)
	{
		printBinary(c[i]);
		std::cout << " ";
	}
	std::cout << "\n";
}

int vecCompareDst1(char *vec1, char *vec2, unsigned int vecLenBytes)
{
	int count = 0;
	for (unsigned int i = 0; i < vecLenBytes; i++)
	{
		unsigned char cmp = vec1[(int)i] ^ vec2[(int)i];
		count += bitCount[cmp];
	}
	return count;
}

void timeElapsedCout(float event, std::string name)
{
	std::cout << std::left << std::setw(28) << name + " ended,"
			  << "time elapsed:" << event << "ms\n";
}

int main(int argc, char *argv[])
{
	std::string outputFile = "";
	unsigned int vecCount, vecLen;
	char *vectors;

	getArguments(argc, argv, outputFile, vectors, vecCount, vecLen);

	unsigned int vecLenBytes = std::ceil(double(vecLen) / 8.0);

	char **vectors2D = new char *[vecCount];
	for (unsigned int i = 0; i < vecCount; i++)
	{
		vectors2D[i] = vectors + i * vecLenBytes;
	}

	Results res;

	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	start = std::chrono::high_resolution_clock::now();
	for (unsigned int i = 0; i < vecCount - 1; i++)
	{
		for (unsigned int j = i + 1; j < vecCount; j++)
		{
			if (vecCompareDst1(vectors2D[i], vectors2D[j], vecLenBytes) == 1)
			{
				res.add(i, j);
			}
		}
	}
	end = std::chrono::high_resolution_clock::now();

	timeElapsedCout(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(), "hamming distance");

	if (outputFile != "")
	{
		writeDataFile(outputFile, std::chrono::duration_cast<std::chrono::milliseconds>(end - start), vecCount, vecLen);
	}
	res.output();

	return 0;
}