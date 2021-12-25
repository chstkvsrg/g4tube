/*
 * HistManager.h
 *
 *  Created on: Sep 12, 2018
 *      Author: fna
 */


#pragma once

#include <map>
#include <vector>

#include <G4String.hh>
//#define G4String std::string

class Hist;



class ITargetHist
{
public:
	virtual ~ITargetHist() {}

	virtual void initHistogram() = 0;
	virtual void saveHisto() = 0;
};

class HistManager
{
public:
	static HistManager* instance();
	static void free();

	void setOutDir(const G4String& outDir);

	void initHistogram(const G4String& id, const G4String& storageFormat, const std::vector<int>& dimensions, double min, double max, int bins);
	void addContext(const G4String& id, void *ctx);
	void addValue(const G4String& id, void *ctx, const std::vector<int>& index, double value);
    void saveHisto(const G4String& id, bool norm = false, const G4String& fmt = "matlab-compability", bool append = false);
    void saveHisto(const G4String& id, char *inputBuffer, bool norm = false, const G4String& fmt = "matlab-compability", bool append = false);

	// for distributed
	void fillBufferWithGatheredResults(const G4String& id, char* buffer, int *len);


	void clear(const G4String& id);


private:
	std::map<G4String, Hist* > mapHist_;

	static HistManager* p_;

	G4String outDir_;

	HistManager();
	virtual ~HistManager();

	G4String path(const G4String& fmt, ...);
	void _mkdir(const G4String& dir);
	void mkdirRecursively(const G4String& formatDir, int currentDepth, int maxDepth, const std::vector<int>& dim, std::vector<int> index);

};
