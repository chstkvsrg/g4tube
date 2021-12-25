/*
 * HistManager.cpp
 *
 *  Created on: Sep 12, 2018
 *      Author: fna
 */

#include <unistd.h>
#include <list>
#include <stdarg.h>
#include <sys/stat.h>
#include <string.h>

#include "HistManager.hh"
#include "Hist.hh"

HistManager* HistManager::p_ = nullptr;


HistManager::HistManager() :
    outDir_("OUTPUT")
{

}

HistManager::~HistManager()
{
	for (std::map<G4String, Hist* >::iterator itr = mapHist_.begin(); itr != mapHist_.end(); itr++) // TODO: use smarts instead and remove this code
		delete itr->second;
	mapHist_.clear();

}

HistManager* HistManager::instance()
{
	if (!p_)
		p_ = new HistManager();
	return p_;
}

void HistManager::free()
{
	if (p_) delete p_;
	p_ = nullptr;
}

void HistManager::setOutDir(const G4String& outDir)
{
	outDir_ = outDir;
}


G4String HistManager::path(const G4String& fmt, ...) // TODO: move to another class
{
    va_list args;
    va_start(args, fmt);

    char buff[100];
    vsprintf(buff, fmt.c_str(), args);
    va_end(args);

    return buff;
}


void HistManager::_mkdir(const G4String& dir)
{
	char tmp[256];
	char *p = NULL;
	size_t len;

	snprintf(tmp, sizeof(tmp),"%s",dir.c_str());
	len = strlen(tmp);
	if(tmp[len - 1] == '/')
		tmp[len - 1] = 0;
	for (p = tmp + 1; *p; p++)
		if(*p == '/')
		{
			*p = 0;

			if (access(tmp, F_OK))
				mkdir(tmp, S_IRWXU);
			*p = '/';
		}
	if (access(tmp, F_OK))
		mkdir(tmp, S_IRWXU);
}


void HistManager::mkdirRecursively(const G4String& storageFormatDir, int currentDimension, int maxDepth, const std::vector<int>& dim, std::vector<int> index)
{
	for (int ind = 0; ind < dim[currentDimension]; ind++)
	{
		index[currentDimension] = ind;
		if (currentDimension != maxDepth)
			mkdirRecursively(storageFormatDir, currentDimension + 1, maxDepth, dim, index);
		else
		{
			int n = index.size();
			G4String p = path(storageFormatDir, (n >= 1 ? index[0]: 0), (n >= 2 ? index[1]: 0), (n >= 3 ? index[2]: 0), (n >= 4 ? index[3]: 0), (n >= 5 ? index[4]: 0));
			_mkdir(p);
		}
	}
}


void HistManager::initHistogram(const G4String& id, const G4String& storageFormat, const std::vector<int>& dimensions, double min, double max, int bins)
{
	G4String storageFormatFull = outDir_ + ((outDir_[outDir_.length() - 1] == '/') ? "" : "/") + storageFormat;

	mapHist_[id] = new Hist(id, storageFormatFull, dimensions, min, max, bins);

	// create storage directories
	int lastSlash = storageFormatFull.rfind('/', storageFormatFull.length() - 1);
	int depth = 0;

	if (lastSlash != -1)
	{
		G4String storageFormatDir = storageFormatFull.substr(0, lastSlash);
		G4String prev = storageFormatDir;

		int n = dimensions.size();

		int i = 0;
		for (i = 0; i < n; i++)
		{
			G4String p = path(storageFormatDir.c_str(), (i >= 0 ? dimensions[0]: 0), (i >= 1 ? dimensions[1]: 0), (i >= 2 ? dimensions[2]: 0), (i >= 3 ? dimensions[3]: 0), (i >= 4 ? dimensions[4]: 0));
			if (p == prev)
				break;
			prev = p;
		}
		depth = i;

		if (depth)
		{
			std::vector<int> index;
			for (i = 0; i < depth; i++)
				index.push_back(0);
			if (depth) mkdirRecursively(storageFormatDir, 0, depth - 1, dimensions, index);
		}
		else
		{
			_mkdir(storageFormatDir);
		}
	}
}

void HistManager::addContext(const G4String& id, void *ctx)
{
	mapHist_[id]->addContext(ctx);
}

void HistManager::addValue(const G4String& id, void *ctx, const std::vector<int>& index, double value)
{
	mapHist_[id]->addValue(ctx, index, value);
}

void HistManager::saveHisto(const G4String& id, bool norm, const G4String& fmt, bool append)
{
	if (access(outDir_.c_str(), F_OK))
		mkdir(outDir_.c_str(), S_IRWXU);

    mapHist_[id]->saveHisto(norm, fmt, append);
}

void HistManager::saveHisto(const G4String& id, char *inputBuffer, bool norm, const G4String& fmt, bool append)
{
	if (access(outDir_.c_str(), F_OK))
		mkdir(outDir_.c_str(), S_IRWXU);

    mapHist_[id]->saveHisto(inputBuffer, norm, fmt, append);
}

void HistManager::fillBufferWithGatheredResults(const G4String& id, char* buffer, int *len)
{
	mapHist_[id]->fillBufferWithGatheredResults(buffer, len);
}


void HistManager::clear(const G4String& id)
{
	mapHist_[id]->clear();
}



