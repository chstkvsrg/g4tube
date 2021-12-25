/*
 * Hist.h
 *
 *  Created on: Sep 12, 2018
 *      Author: fna
 */


#pragma once

#include <map>
#include <vector>
#include <pthread.h>

#include <G4String.hh>
//#define G4String std::string

typedef unsigned long long longuint;


class Hist
{
public:
	Hist(G4String id, const G4String& storageFormat, const std::vector<int>& dimensions, double min, double max, int bins);
	~Hist();

	G4String path(const G4String& fmt, ...);

	void addContext(void *ctx);
	void addValue(void *ctx, const std::vector<int>& index, double value);
    void saveHisto(bool norm, const G4String& fmt, bool append);
    void saveHisto(char* inputBuffer, bool norm, const G4String& fmt, bool append);

	// for distributed
	void fillBufferWithGatheredResults(char *buffer, int *len);

	void clear();

private:

	G4String id_;
	G4String storageFormat_;
	std::vector<int> dimensions_;
	double min_;
	double max_;
	int bins_;

	double step_;
	int totalElements_;

	std::map<void*, uint*> mapCtxHistData_;

	pthread_spinlock_t spin_;

    void saveHistoRecursively(char* inputBuffer, bool norm, const G4String& fmt, bool append, int currentIndex, std::vector<int> index);
};



