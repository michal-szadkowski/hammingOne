#include <iostream>
#include <string>
#include <chrono>

char *readFile(std::string inputFile, unsigned int &vectorCount, unsigned int &vectorLen);

void writeDataFile(std::string outputFile, std::chrono::milliseconds time, unsigned int vecCount, unsigned int vecLen);

void writeDataEntry(std::ostream &output, std::chrono::milliseconds time, unsigned int vecCount, unsigned int vecLen);

void writeDataConsole(std::chrono::milliseconds time, unsigned int vecCount, unsigned int vecLen);

void writeDataFile(std::string outputFile, float time, unsigned int vecCount, unsigned int vecLen);

void writeDataConsole(float time, unsigned int vecCount, unsigned int vecLen);

void writeDataEntry(std::ostream &output, float time, unsigned int vecCount, unsigned int vecLen);

void writeRestults();

class Results
{
public:
	struct hamPairList
	{
		hamPairList *next;
		int p1;
		int p2;
	};
	hamPairList *head = nullptr;
	int count = 0;

	void add(int a, int b);
	bool exportToFile(std::string fileName);
	bool importFromFile(std::string fileName);
	void output();
};
