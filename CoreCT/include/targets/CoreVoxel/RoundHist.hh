#pragma once

#include "HistManager.hh"


class RoundHist: public ITargetHist
{
public:
	RoundHist();
    virtual ~RoundHist();



protected:
	void initHistogram();
	void saveHisto();


private:


};




