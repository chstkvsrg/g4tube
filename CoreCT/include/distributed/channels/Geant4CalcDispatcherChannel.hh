/*
 * Geant4CalcDispatcherChannel.h
 *
 *  Created on: Jun 24, 2019
 *      Author: fna
 */

#pragma once

#include "IChannel.hh"

#include <iostream>

class Geant4CalcDispatcherChannel : public IDispatcherSideChannel
{
public:
	Geant4CalcDispatcherChannel(const std::string& srcArchivePath, DispatcherTransport *trans, int fd);
	virtual ~Geant4CalcDispatcherChannel() {}

	void init(const std::string& srcArchivePath, DispatcherTransport *trans, int fd);

	void update(const char* szParams);
	void restart(const char* szParams);
	char* getUnitInfo(); // should be in service channel
	void run(int upToEvents);
	void stop();
	unsigned long long eventProcessed();
	void downloadStats(char* buffer, int *len);
	unsigned temperature(); // should be in service channel

	bool process(char *buffer, int len) override;

	void disconnect();

private:

	std::string srcArchivePath_;
	unsigned long long eventProcessed_;
	unsigned temp_;

	char szUnitInfo[1025];

	//unsigned short Geant4CalcDispatcherChannel_;
};

