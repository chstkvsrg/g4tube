/*
 * DispatcherTransport.h
 *
 *  Created on: Jun 22, 2019
 *      Author: fna
 */

#pragma once

#include <list>
#include <pthread.h>

class DispatcherTransport
{
public:
	DispatcherTransport();
	~DispatcherTransport();

	void sendBroadCastMessage(const char *host);


	void sendARecv(int fd, char *data, int *len, int bufferLen);

	char* buffer() { return buffer_; }

	std::list<int> unitConnections();

	void disconnect(int fd);

	void startAcception();
	void stopAcception();

private:
	std::list<int> lstFDs_;
	char *buffer_;

	pthread_mutex_t mutex_;

	pthread_t thread_;
	bool keepAcceptThreadRunning_;

	static void* startRoutine(void *p);


};

