/*
 * TemperatureChannel.h
 *
 *  Created on: Jun 24, 2019
 *      Author: fna
 */

#pragma once

#include "IChannel.hh"

class TemperatureUnitChannel : public IUnitSideChannel
{
public:
	TemperatureUnitChannel(UnitTransport *trans);
	virtual ~TemperatureUnitChannel() {}

	bool process(char *buffer, int len) override;
};

