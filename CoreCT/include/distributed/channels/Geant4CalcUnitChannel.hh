/*
 * Geant4CalcUnitChannel.h
 *
 *  Created on: Jun 24, 2019
 *      Author: fna
 */

#pragma once

#include "IChannel.hh"

#include <pthread.h>
#include <iostream>
#include <list>

class IApp
{
public:
	virtual ~IApp() {}
	virtual void threadInitialization() = 0;
	virtual void threadDeinitialization() = 0;
	virtual void run(int events) = 0;
	virtual void fillBufferWithGatheredResults(char* buffer, int *len) = 0;
};

class Geant4CalcUnitChannel : public IUnitSideChannel
{
public:
	Geant4CalcUnitChannel(IApp *app, UnitTransport *trans);
	virtual ~Geant4CalcUnitChannel();

	bool process(char *buffer, int len) override;

	void stopThreads();

private:
    pthread_mutex_t mutex_;
    pthread_cond_t cond_;

    std::list<int> lstReqs_;

    int currentReq_;
    const std::string workDir_;

    unsigned long long eventsProcessed_;
    unsigned long long eventsDesired_;

    bool active_;
    bool keepThreadsRunning_;

    pthread_t thread_;

    IApp *app_;

	static void* startRoutine(void *p);
};

