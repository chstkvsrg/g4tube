#pragma once

#include "HistManager.hh"


class CorePhantomHist: public ITargetHist
{
public:
    CorePhantomHist();
    virtual ~CorePhantomHist();



protected:
	void initHistogram();
	void saveHisto();


private:


};




