#include "CorePhantomHist.hh"
#include "TargetsCalculator.hh"

#include "corect_ggo.h"

extern args_info_corect args_info;

CorePhantomHist::CorePhantomHist()
{
}

CorePhantomHist::~CorePhantomHist()
{
}


void CorePhantomHist::initHistogram()
{
	std::vector<int> dim;
    dim.push_back(TargetsCalculator::instance()->GetDetYNumber());
	char sz[512];
	sprintf(sz, "%.1f/%%d.txt", args_info.angle_arg);
	//HistManager::instance()->initHistogram("my", sz, dim, 0, 560, 560);
    HistManager::instance()->initHistogram("my", sz, dim, 0, TargetsCalculator::instance()->GetDetXNumber(), TargetsCalculator::instance()->GetDetXNumber());

	/*std::vector<int> dim;
		dim.push_back(1);
        //histMan->initHistogram("my", "tube-spectrum.txt", di m, 0, 200, 200);*/
}

void CorePhantomHist::saveHisto()
{
	HistManager::instance()->saveHisto("my");
}



