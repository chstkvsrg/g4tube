/*
 * UnitTransport.cxx
 *
 *  Created on: Jun 22, 2019
 *      Author: fna
 */

#include "UnitTransport.hh"
#include "RemoteUtils.hh"
#include "constants.hh"

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>


UnitTransport::UnitTransport() : fd_(-1)
{
	buffer_ = new char[BUFFER_LEN];
}

UnitTransport::~UnitTransport()
{
	if (fd_ != -1) closesock(fd_);

	delete []buffer_;
}

void UnitTransport::waitForDispatcherConnection()
{
	int sd = socket(AF_INET, SOCK_DGRAM, 0);

	if ( sd < 0 )
		//handle_error("socket failed");
		return;

	const int on=1;
	struct sockaddr_in addr;

	if ( setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) != 0 )
		//handle_error("set broadcast failed");
		return;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(DEFAULT_PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//if ( inet_aton(argv[2], &addr.sin_addr) == 0 )
		//panic("inet_aton failed (%s)", argv[2]);

	if ( bind(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 )
		//handle_error("bind failed");
		return;

	char buffer[1024];

	do
	{
		printf("wait for broadcast message\n");

		socklen_t len = sizeof(addr);

		ssize_t r;
		if ( (r = recvfrom(sd, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, &len)) > 0 )
		{
			printf("Got %s:%d \"%s\"\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), buffer);

			sscanf(buffer, "%s %d", szDispatcherHost_, &dispatcherPort_);

			if (connectToDispatcher(szDispatcherHost_, dispatcherPort_))
			{
				printf("success! this unit has been linked to dispatcher %s:%d and waits for orders...\n", szDispatcherHost_, dispatcherPort_);
				break;
			}
		}
		else
			//perror("recvfrom");
			printf("recvfrom errorn r - %d len - %d\n", (int)r, len);
	}
	while ( true );
	//while ( strcmp(buffer, "bye\n") != 0 );

	closesock(sd);
}

bool UnitTransport::connectToDispatcher(const char* file)
{
	FILE *f = fopen("dispatcher_host", "r+");
	fscanf(f, "%s %d", szDispatcherHost_, &dispatcherPort_);
	fclose(f);

	return connectToDispatcher(szDispatcherHost_, dispatcherPort_);
}

bool UnitTransport::connectToDispatcher(const char* szHost, int port)
{
	if (fd_ != -1)
	{
		printf("Connection already exists!\n");
		return false;
	}
	fd_ = socket(AF_INET, SOCK_STREAM, 0);

	//setUnblockSocket(fd, 0);
	/*struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
	setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));*/

	sockaddr_in dest_addr;
	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(port);

	((unsigned long *)&dest_addr.sin_addr)[0] = inet_addr(szHost);

	printf("why mother fucker... \n");
	if (connect(fd_, (sockaddr *)&dest_addr, sizeof(dest_addr)))
	{
		printf("connect() error %d\n", errno);
		closesock(fd_);
		fd_ = -1;
		return false;
	}
	else
	{
		printf("connect ok ! \n");
	}

	setUnblockSocket(fd_, 5);

	return true;
	//return sendData("o", 1);
}

bool UnitTransport::listenARead(char **buffer, int *messageLen)
{
	*buffer = buffer_;
	while (1)
		if (checkOnNewData(fd_, 1000))
			return readMessage(fd_, buffer_, BUFFER_LEN, messageLen);
}


bool UnitTransport::sendData(const char *data, int len)
{
	int uni = len;
	endianConversation((char*)&uni, sizeof(uni), 1);
	unsigned long sent = send(fd_, (const char*)&uni, sizeof(uni), 0);
	if (sent != sizeof(len)) return false;

	sent = send(fd_, (const char*)data, len, 0);
	return sent == len;
}

void UnitTransport::saveDispatcherHost()
{
	FILE *f = fopen("dispatcher_host", "w");
	fprintf(f, "%s %d", szDispatcherHost_, dispatcherPort_);
	fclose(f);
}


void UnitTransport::disconnect()
{
	closesock(fd_);
	fd_ = -1;
}

