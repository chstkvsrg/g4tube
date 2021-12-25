/*
 * Hist.cpp
 *
 *  Created on: Sep 12, 2018
 *      Author: fna
 */

#include <fstream>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <cmath>

#include "Hist.hh"

//#include "globals.hh"



Hist::Hist(G4String id, const G4String& storageFormat, const std::vector<int>& dimensions, double min, double max, int bins) :
		id_(id), storageFormat_(storageFormat), dimensions_(dimensions), min_(min), max_(max), bins_(bins)
{
	pthread_spin_init(&spin_, 0);
	step_ = (max_ - min_) / bins;

	totalElements_ = 1;
	for (int i = 0, n = dimensions_.size(); i < n; i++)
		totalElements_ = totalElements_ * dimensions_[i];
	totalElements_ = totalElements_ * bins_;
}

Hist::~Hist()
{
	for (std::map<void*, uint*>::iterator itr = mapCtxHistData_.begin(); itr != mapCtxHistData_.end(); itr++)
		delete []itr->second;
	mapCtxHistData_.clear();

	pthread_spin_destroy(&spin_);
}

G4String Hist::path(const G4String& fmt, ...) // TODO: move to another class
{
    va_list args;
    va_start(args, fmt);

    char buff[100];
    vsprintf(buff, fmt.c_str(), args);
    va_end(args);

    return buff;
}


void Hist::addContext(void *ctx)
{
	uint *p = new uint[totalElements_];
	memset(p, 0, sizeof(uint) * totalElements_);

	pthread_spin_lock(&spin_);
	mapCtxHistData_[ctx] = p;
	pthread_spin_unlock(&spin_);
}

void Hist::addValue(void *ctx, const std::vector<int>& index, double value)
{
	if (!mapCtxHistData_.count(ctx))
	{
		char msg[200];
		sprintf(msg, "no context has found! histo id - %s ctx - %p", id_.c_str(), ctx);
		//G4Exception("Hist::addValue", "", RunMustBeAborted, msg);
		return;
	}

	int offset = 0;
	int n = index.size();
	for (int i = 0; i < n; i++)
	{
		int l = 1;
		for (int j = i + 1; j < n; j++)
			l = l * dimensions_[j];
		offset += index[i] * l * bins_;
	}

	//int bin = (value - min_) / step_;
	int bin = round((value - min_) / step_);
	if (bin < 0) bin = 0;
	if (bin >= bins_) bin = bins_ - 1;

	mapCtxHistData_[ctx][offset + bin]++;

}


void Hist::saveHistoRecursively(char* inputBuffer, bool norm, const G4String& fmt, bool append, int currentDimension, std::vector<int> index)
{
	for (int ind = 0; ind < dimensions_[currentDimension]; ind++)
	{
		index[currentDimension] = ind;
		if (currentDimension != int(index.size() - 1))
            saveHistoRecursively(inputBuffer, norm, fmt, append, currentDimension + 1, index);
		else
		{
			if (!strcmp(fmt.c_str(), "matlab-compability"))
			{
				int offset = 0;
				int n = index.size();
				for (int i = 0; i < n; i++)
				{
					int l = 1;
					for (int j = i + 1; j < n; j++)
						l = l * dimensions_[j];
					offset += index[i] * l * bins_;
				}

				uint* gather;
				double summ = 1.;

				if (inputBuffer)
					gather = &((uint*)inputBuffer)[offset];
				else
				{
					gather = new uint[bins_]; // TODO: smart
					memset(gather, 0, sizeof(uint) * bins_);

					for (std::map<void*, uint*>::iterator itr = mapCtxHistData_.begin(); itr != mapCtxHistData_.end(); itr++)
                        for (int b = 0; b < bins_; b++)
						{
							//printf("%d \n", i);
                            gather[b] += itr->second[offset + b];
						}

					if (norm)
					{
						summ = 0;
                        for (int b = 0; b < bins_; b++)
                            summ += gather[b];
					}
				}

                G4String fileName = path(storageFormat_.c_str(), (n >= 1 ? index[0]: 0), (n >= 2 ? index[1]: 0), (n >= 3 ? index[2]: 0), (n >= 4 ? index[3]: 0), (n >= 5 ? index[4]: 0));

                if (append)
                {
                    FILE *f = fopen(fileName.c_str(), "r");
                    if (f)
                    {
                        char str[256];
                        char *p = fgets(str, 126, f);
                        int value1, value2;
                        int *gather2 = new int[bins_];
                        int i = -1;
                        while (!feof (f))
                        {
                            p = fgets(str, 126, f);
                            if (!p) break;
                            i++;
                            if (i == bins_) break;
                            sscanf(str, "%d\t%d", &value1, &value2);
                            gather2[i] = value2;
                        }

                        fclose(f);

                        if (i == bins_ - 1)
                            for (int b = 0; b < bins_; b++)
                                gather[b] += gather2[b];

                        delete [] gather2;
                    }
                }

                FILE *f = fopen(fileName.c_str(), "w");
                fprintf(f, "bin\tcount\n");
                for (int b = 0; b < bins_; b++)
                    fprintf(f, "%d\t%d\n", b, gather[b]);
                fclose(f);

                /*std::ofstream f(p.c_str());
				f << "bin\tcount\n";
				for (int i=0; i < bins_; i++)
				{
					if (norm)
						f << i << "\t" << gather[i] / summ << "\n";
					else
						f << i << "\t" << gather[i] << "\n";
				}
                f.close();*/

				if (!inputBuffer) delete []gather;
			}
			else
			{
				char msg[200];
				sprintf(msg, "format %s is not supported! skip this saveHisto() call... ", fmt.c_str());
				//G4Exception("Hist::saveHisto", "", JustWarning, msg);
			}
		}

	}
}

void Hist::saveHisto(bool norm, const G4String& fmt, bool append)
{
	std::vector<int> index;
	for (int i = 0, n = dimensions_.size(); i < n; i++)
		index.push_back(0);
    saveHistoRecursively(nullptr, norm, fmt, append, 0, index);
}

void Hist::saveHisto(char* inputBuffer, bool norm, const G4String& fmt, bool append)
{
	std::vector<int> index;
	for (int i = 0, n = dimensions_.size(); i < n; i++)
		index.push_back(0);
    saveHistoRecursively(inputBuffer, norm, fmt, append, 0, index);
}

void Hist::fillBufferWithGatheredResults(char *buffer, int *len)
{
	*len = totalElements_ * sizeof(uint);
	printf("Attention! Required size for to put stats into the buffer : %d. With new configuration, check whether it is enough or not.\n", *len);

	if (buffer)
	{
		uint* gather = (uint*) buffer;
		memset(gather, 0, sizeof(uint) * totalElements_);

		int n = mapCtxHistData_.size();
		uint** pp = new uint*[mapCtxHistData_.size()];
		std::map<void*, uint*>::iterator itr = mapCtxHistData_.begin();
		for (int i = 0; i < n; i++, itr++)
			pp[i] = itr->second;

		for (int i = 0; i < totalElements_; i++)
			for (int ctx = 0; ctx < n; ctx++)
				gather[i] += pp[ctx][i];

		delete []pp;

		for (int i = 0; i < totalElements_; i++)
			std::cout << gather[i] << " " << std::endl;
	}

}

void Hist::clear()
{
	for (std::map<void*, uint*>::iterator itr = mapCtxHistData_.begin(); itr != mapCtxHistData_.end(); itr++)
		memset(itr->second, 0, totalElements_ * sizeof(uint));
}



