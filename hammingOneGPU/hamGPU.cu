#include <cuda_runtime.h>

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

void usage();

void getArguments(int argc, char *argv[], std::string &outputFile, char *&vectors, unsigned int &vecCount, unsigned int &vecLen);

//#define VERBOSEPAIRS
__global__ void hammingOne2(char *vectors, int vecLenB, int vecCount, unsigned int *pairCount)
{

	int pairs = 0;
	int vecI = blockIdx.x * blockDim.x + threadIdx.x;
	if (vecI >= vecCount)
		return;

	char *v1 = new char[vecLenB];
	for (int i = 0; i < vecLenB; i++)
	{

		v1[i] = vectors[i * vecCount + vecI];
	}

	if (vecI < vecCount)
	{
		int v2 = vecI + 1;

		for (int c = 0; 2 * c <= vecCount; c++)
		{
			if (v2 >= vecCount)
			{
				v2 -= vecCount;
				c++;
				c++;
				continue;
			}

			int d = 0;
			for (int i = 0; i < vecLenB; i++)
			{
				char c = v1[i] ^ vectors[i * vecCount + v2];

				d += __popc(c & 255);
				if (d > 1)
					break;
			}
			if (d == 1)
			{
				pairs++;
			}

			v2++;
		}
	}
	delete v1;
	atomicAdd(pairCount, pairs);
	return;
}
void timeElapsedCout(float start, float event, std::string name)
{
	std::cout << std::left << std::setw(28) << name + " ended,"
			  << "time elapsed:" << event << " ms, \tsince start: " << start << "ms\n";
}

void eventStartMeasure(cudaEvent_t start, std::string name)
{
	std::cout << name << " started\n";
	cudaEventRecord(start, 0);
}
void eventStopMeasure(cudaEvent_t stop, cudaEvent_t start, float &startms, std::string name)
{
	float evms;
	cudaEventRecord(stop, 0);
	cudaDeviceSynchronize();

	cudaEventElapsedTime(&evms, start, stop);
	startms += evms;
	timeElapsedCout(startms, evms, name);
}

int main(int argc, char *argv[])
{
	std::string outputFile = "";
	unsigned int vecCount, vecLen;

	char *vectors;
	char *d_vectors;
	unsigned int *d_pairCount;

	getArguments(argc, argv, outputFile, vectors, vecCount, vecLen);

	unsigned int vecLenBytes = std::ceil(double(vecLen) / 8.0);

	char *vectorsCoales = new char[vecLenBytes * vecCount];
	for (int i = 0; i < vecCount; i++)
	{
		for (int j = 0; j < vecLenBytes; j++)
		{
			vectorsCoales[j * vecCount + i] = vectors[i * vecLenBytes + j];
		}
	}

	dim3 blockSize2(1024);
	dim3 blockCount2(ceil((double)vecCount / blockSize2.x));

	cudaEvent_t p1, p2;
	float startms = 0;
	std::cout << "tyutaj";
	cudaEventCreate(&p1);
	cudaEventCreate(&p2);

	eventStartMeasure(p1, "Memory allocation");
	cudaMalloc(&d_vectors, vecCount * vecLenBytes);
	cudaMalloc(&d_pairCount, sizeof(int));
	eventStopMeasure(p2, p1, startms, "Memory allocation");

	eventStartMeasure(p1, "Memory copy");
	cudaMemcpy(d_vectors, vectorsCoales, vecCount * vecLenBytes, cudaMemcpyHostToDevice);
	eventStopMeasure(p2, p1, startms, "Memory copy");

	eventStartMeasure(p1, "Kernel");
	hammingOne2<<<blockCount2, blockSize2>>>(d_vectors, vecLenBytes, vecCount, d_pairCount);
	eventStopMeasure(p2, p1, startms, "Kernel");

	eventStartMeasure(p1, "Free memory");
	cudaFree(d_vectors);
	eventStopMeasure(p2, p1, startms, "Free memory");

	unsigned int pairCount;
	cudaMemcpy(&pairCount, d_pairCount, 4, cudaMemcpyDeviceToHost);
	cudaFree(d_pairCount);
	std::cout << "Pair count: " << pairCount << "\n";
	if (outputFile != "")
	{
		writeDataFile(outputFile, startms, vecCount, vecLen);
	}

	delete[] vectors;
	delete[] vectorsCoales;

	return 0;
}

void usage()
{
	std::cout << "usage: hamGPU inputfile [outputfile]\n";
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