/*
 * UnitTransport.h
 *
 *  Created on: Jun 22, 2019
 *      Author: fna
 */

#pragma once

class UnitTransport
{
public:
	UnitTransport();
	~UnitTransport();

	void waitForDispatcherConnection();

	bool connectToDispatcher(const char* szHost, int port);
	bool connectToDispatcher(const char* file);
	void disconnect();

	bool listenARead(char **buffer, int *messageLen);
	bool sendData(const char *data, int len);

	void saveDispatcherHost();


private:
	int fd_;
	char *buffer_;

	char szDispatcherHost_[30];
	int dispatcherPort_;




};

