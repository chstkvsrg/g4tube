
#include "Geant4CalcDispatcherChannel.hh"

#include "DispatcherTransport.hh"
#include "Exception.hh"
#include "constants.hh"
#include "RemoteUtils.hh"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Geant4CalcDispatcherChannel::Geant4CalcDispatcherChannel(const std::string& srcArchivePath, DispatcherTransport *trans, int fd) :
IDispatcherSideChannel(GEANT4CALC_CHANNEL_ID, trans, fd), srcArchivePath_(srcArchivePath)
{

}

void Geant4CalcDispatcherChannel::update(const char* szParams)
{
	char *buffer = trans_->buffer();
	sprintf(buffer, "%u|%hhu|%s|", id_, GEANT4CALC_CHANNEL_UPDATE_REQ, szParams);
	int index = strlen(buffer);

	FILE *f = fopen(srcArchivePath_.c_str(), "r+");
	fseek(f, 0 , SEEK_END);
	int fileSize = ftell(f);
	fseek(f, 0 , SEEK_SET);
	int readSize = fread(&buffer[index], 1, fileSize, f);
	index += readSize;
	fclose(f);
	if (fileSize != readSize)
		throw Exception(fd_, "Failed to read fullsize of src archive");

	int len = index;
	trans_->sendARecv(fd_, buffer, &len, BUFFER_LEN);

	process(buffer, len);
}

void Geant4CalcDispatcherChannel::restart(const char* szParams)
{
	char *buffer = trans_->buffer();
	sprintf(buffer, "%u|%hhu|%s|", id_, GEANT4CALC_CHANNEL_RESTART_REQ, szParams);

	int len = strlen(buffer);
	trans_->sendARecv(fd_, buffer, &len, BUFFER_LEN);

	process(buffer, len);
}


char* Geant4CalcDispatcherChannel::getUnitInfo()
{
	char *buffer = trans_->buffer();
	sprintf(buffer, "%u|%hhu", id_, GEANT4CALC_CHANNEL_GET_UNIT_INFO);
	int index = strlen(buffer);

	int len = strlen(buffer);
	trans_->sendARecv(fd_, buffer, &len, BUFFER_LEN);

	process(buffer, len);

	return szUnitInfo;
}

void Geant4CalcDispatcherChannel::run(int upToEvents)
{
	char *buffer = trans_->buffer();
	sprintf(buffer, "%u|%hhu|%d", id_, GEANT4CALC_CHANNEL_RUN_REQ, upToEvents);
	int index = strlen(buffer);

	int len = strlen(buffer);
	trans_->sendARecv(fd_, buffer, &len, BUFFER_LEN);

	process(buffer, len);
}

void Geant4CalcDispatcherChannel::stop()
{
	char *buffer = trans_->buffer();
	sprintf(buffer, "%u|%hhu", id_, GEANT4CALC_CHANNEL_STOP_REQ);
	int index = strlen(buffer);

	int len = strlen(buffer);
	trans_->sendARecv(fd_, buffer, &len, BUFFER_LEN);

	process(buffer, len);
}


unsigned long long Geant4CalcDispatcherChannel::eventProcessed()
{
	char *buffer = trans_->buffer();
	sprintf(buffer, "%u|%hhu", id_, GEANT4CALC_CHANNEL_GET_PROCESSED_EVENTS_REQ);

	int len = strlen(buffer);
	trans_->sendARecv(fd_, buffer, &len, BUFFER_LEN);

	process(buffer, len);

	return eventProcessed_;
}

void Geant4CalcDispatcherChannel::downloadStats(char* buffer, int *len)
{
	sprintf(buffer, "%u|%hhu", id_, GEANT4CALC_CHANNEL_DOWNLOAD_STATS_REQ);

	*len = strlen(buffer);
	trans_->sendARecv(fd_, buffer, len, BUFFER_LEN);

	int index = 0;
	int c = 0;
	while (c != 2) if (buffer[index++] == '|') c++;
	*len -= index;
	memcpy(buffer, &buffer[index], *len);
	endianConversation(buffer, sizeof(uint), *len / sizeof(uint));
}

unsigned Geant4CalcDispatcherChannel::temperature()
{
	char *buffer = trans_->buffer();
	sprintf(buffer, "%u|%hhu", id_, GEANT4CALC_CHANNEL_GET_TEMP);

	int len = strlen(buffer);
	trans_->sendARecv(fd_, buffer, &len, BUFFER_LEN);

	process(buffer, len);
	return temp_;
}


bool Geant4CalcDispatcherChannel::process(char *buffer, int len)
{
	unsigned char req;

	//buffer[len] = 0; // it has already done in readMesssage

	int id;
	sscanf(buffer, "%u|%hhu", &id, &req);

	switch (req)
	{
		case GEANT4CALC_CHANNEL_UPDATE_REQ:
		case GEANT4CALC_CHANNEL_RESTART_REQ:
		case GEANT4CALC_CHANNEL_RUN_REQ:
		case GEANT4CALC_CHANNEL_STOP_REQ:
		{
			unsigned char ok;
			sscanf(buffer, "%u|%hhu|%d", &id, &req, &ok);
			if (!ok)
				throw Exception(fd_, "Failed to process request");
		} break;
		case GEANT4CALC_CHANNEL_GET_PROCESSED_EVENTS_REQ:
		{
			sscanf(buffer, "%u|%hhu|%llu", &id, &req, &eventProcessed_);
		} break;
		case GEANT4CALC_CHANNEL_GET_UNIT_INFO:
		{
			sscanf(buffer, "%u|%hhu|%1024[^\n]", &id, &req, szUnitInfo);
		} break;
		case GEANT4CALC_CHANNEL_GET_TEMP:
		{
			sscanf(buffer, "%u|%hhu|%d", &id, &req, &temp_);
		} break;
	}

	return true;
}


void Geant4CalcDispatcherChannel::disconnect()
{
	trans_->disconnect(fd_);
}
