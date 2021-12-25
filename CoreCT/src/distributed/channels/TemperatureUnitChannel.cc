
#include "TemperatureUnitChannel.hh"

#include "UnitTransport.hh"
#include "constants.hh"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

TemperatureUnitChannel::TemperatureUnitChannel(UnitTransport *trans) :
	IUnitSideChannel(TEMPERATURE_CHANNEL_ID, trans)
{

}

bool TemperatureUnitChannel::process(char *buffer, int len)
{
	unsigned char req;

	unsigned id;
	sscanf(buffer, "%u|%hhu", &id, &req);

	switch (req)
	{
		case TEMPERATURE_CHANNEL_GET_TEMPERATURE_REQ:
		{
			unsigned short temperature = 10 + rand() % 10;

			sprintf(buffer, "%u|%hhu|%hu", id_, req, temperature);
			trans_->sendData(buffer, strlen(buffer));
		} break;
	}

	return true;
}
