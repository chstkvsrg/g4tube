
#include "TemperatureDispatcherChannel.hh"

#include "DispatcherTransport.hh"
#include "constants.hh"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

TemperatureDispatcherChannel::TemperatureDispatcherChannel(DispatcherTransport *trans, int fd) :
IDispatcherSideChannel(TEMPERATURE_CHANNEL_ID, trans, fd), temperature_(0)
{

}

unsigned short TemperatureDispatcherChannel::getTemperature()
{
	char *buffer = trans_->buffer();
	sprintf(buffer, "%u|%hhu", id_, TEMPERATURE_CHANNEL_GET_TEMPERATURE_REQ);

	int len = strlen(buffer);
	printf("dispatch sends %d bytes\n", len);
	trans_->sendARecv(fd_, buffer, &len, BUFFER_LEN);

	buffer[len] = 0;
	printf("where 3 %s\n", buffer);

	process(buffer, len);

	return temperature_;
}

bool TemperatureDispatcherChannel::process(char *buffer, int len)
{
	unsigned char req;

	int id;
	sscanf(buffer, "%u|%hhu", &id, &req);

	switch (req)
	{
		case TEMPERATURE_CHANNEL_GET_TEMPERATURE_REQ:
		{
			sscanf(buffer, "%u|%hhu|%hu", &id, &req, &temperature_);
		} break;
	}

	return true;
}



