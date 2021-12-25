/*
 * TemperatureChannel.h
 *
 *  Created on: Jun 24, 2019
 *      Author: fna
 */

#pragma once

#include "IChannel.hh"

class TemperatureDispatcherChannel : public IDispatcherSideChannel
{
public:
	TemperatureDispatcherChannel(DispatcherTransport *trans, int fd);
	virtual ~TemperatureDispatcherChannel() {}

	unsigned short getTemperature();

	bool process(char *buffer, int len) override;

private:
	unsigned short temperature_;
};

