#include "fileIO.h"
#include <iostream>
#include <fstream>
#include <cmath>

int bytes2int(char *bytes)
{
	int n = 0;
	n = bytes[3] & 255;
	n += (bytes[2] << 8) & (255 << 8);
	n += (bytes[1] << 16) & (255 << 16);
	n += (bytes[0] << 24) & (255 << 24);
	return n;
}
char *int2bytes(unsigned int n)
{
	char *bytes = new char[4];
	bytes[0] = (n >> 24) & 0xFF;
	bytes[1] = (n >> 16) & 0xFF;
	bytes[2] = (n >> 8) & 0xFF;
	bytes[3] = n & 0xFF;
	return bytes;
}

char *readFile(std::string inputFile, unsigned int &vectorCount, unsigned int &vectorLen)
{
	std::ifstream input;
	input.open(inputFile, std::ios::in | std::ios::binary);

	if (!input.is_open())
		return NULL;

	char *buf = new char[4];

	input.read(buf, 4);
	vectorCount = bytes2int(buf);
	input.read(buf, 4);
	vectorLen = bytes2int(buf);
	unsigned int vecLenBytes = ceil(double(vectorLen) / 8.0);
	char *vectors = new char[vectorCount * vecLenBytes];
	input.read(vectors, vecLenBytes * vectorCount);
	input.close();
	delete[] buf;
	return vectors;
}

std::ofstream openFileStream(std::string outputFile)
{
	std::ofstream output;
	output.open(outputFile, std::ios::out | std::ios::app);
	return output;
}

void writeDataFile(std::string outputFile, std::chrono::milliseconds time, unsigned int vecCount, unsigned int vecLen)
{
	std::ofstream output = openFileStream(outputFile);
	if (!output.is_open())
		return;
	writeDataEntry(output, time, vecCount, vecLen);
	output.close();
}

void writeDataConsole(std::chrono::milliseconds time, unsigned int vecCount, unsigned int vecLen)
{
	writeDataEntry(std::cout, time, vecCount, vecLen);
}

void writeDataEntry(std::ostream &output, std::chrono::milliseconds time, unsigned int vecCount, unsigned int vecLen)
{
	output << vecCount << "," << vecLen << "," << (time).count() << "\n";
}

void writeDataFile(std::string outputFile, float time, unsigned int vecCount, unsigned int vecLen)
{
	std::ofstream output = openFileStream(outputFile);
	if (!output.is_open())
		return;
	writeDataEntry(output, time, vecCount, vecLen);
	output.close();
}

void writeDataConsole(float time, unsigned int vecCount, unsigned int vecLen)
{
	writeDataEntry(std::cout, time, vecCount, vecLen);
}

void writeDataEntry(std::ostream &output, float time, unsigned int vecCount, unsigned int vecLen)
{
	output << vecCount << "," << vecLen << "," << time << "\n";
}

void Results::add(int a, int b)
{
	hamPairList *node = new hamPairList{.next = head, .p1 = a, .p2 = b};
	head = node;
	count++;
};

bool Results::exportToFile(std::string fileName)
{
	std::ofstream output;
	output.open(fileName, std::ios::out | std::ios::binary);

	if (!output.is_open())
		return false;

	char *buf = int2bytes(this->count);
	output.write(buf, 4);

	Results::hamPairList *nd = head;
	while (nd != nullptr)
	{
		output.write(int2bytes(nd->p1), 4);
		output.write(int2bytes(nd->p2), 4);
		nd = nd->next;
	}
	return true;
}

bool Results::importFromFile(std::string fileName)
{
	std::ifstream input;
	input.open(fileName, std::ios::in | std::ios::binary);

	if (!input.is_open())
		return false;

	char buf[4];
	input.read(buf, 4);
	int pairCount = bytes2int(buf);
	head = nullptr;
	for (int i = 0; i < pairCount; i++)
	{
		input.read(buf, 4);
		int a = bytes2int(buf);
		input.read(buf, 4);
		int b = bytes2int(buf);
		this->add(a, b);
	}
	return true;
}

void Results::output()
{
	std::cout << this->count << "\n";
	Results::hamPairList *nd = this->head;
	while (nd != nullptr)
	{
		// std::cout << nd->p1 << " " << nd->p2 << "\n";
		nd = nd->next;
	}
}
