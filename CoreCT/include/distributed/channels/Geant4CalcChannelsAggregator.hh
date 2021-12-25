/*
 * Geant4CalcChannelsAggregator.h
 *
 *  Created on: Jun 27, 2019
 *      Author: fna
 */

#pragma once

class Geant4CalcDispatcherChannel;
class DispatcherTransport;

#include <list>
#include <map>

class Geant4CalcChannelsAggregator
{
public:
	Geant4CalcChannelsAggregator(DispatcherTransport *trans);
	~Geant4CalcChannelsAggregator();

	void disconnect();
	void clean();

	void setChannels(const std::list<int> &fds);

	DispatcherTransport* trans() { return trans_; }
	int unitsNumber() { return map_.size(); }



	// channel specific functions
	void update(const char *szParams);
	void restart(const char *szParams);
	void run(int upToEvents);
	void stop();
	unsigned long long eventProcessed();
	void downloadStats(char* buffer, int *len);

	unsigned long long eventProcessed(int fd);
	char* getInfo(int fd);
	unsigned temperature(int fd);


private:
	std::map<int, Geant4CalcDispatcherChannel*> map_;
	DispatcherTransport *trans_;


};
