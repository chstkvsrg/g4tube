/*
 * RemoteUtils.h
 *
 *  Created on: Jul 2, 2015
 *      Author: koza
 */

#include "RemoteUtils.hh"

#include <iostream>
#include <string.h>

#ifdef _WIN32
	#include <winsock.h>
	#include <windows.h>
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <dlfcn.h>
	#include <unistd.h>
#endif


void msleep(int msec)
{
#ifdef _WIN32
	Sleep(msec);
#else
	usleep(msec * 1000);
#endif
}

void closesock(int fd)
{
	if (fd > 0)
	{
#ifdef _WIN32
		closesocket(fd);
#else
		close(fd);
#endif
	}
}

bool setUnblockSocket(int fd, int sec)
{
	// SO_RCVTIMEO, SO_SNDTIMEO

#ifdef _WIN32
	int tval = sec;
	if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tval, sizeof(tval)) == -1)
#else
	struct timeval tval;
	tval.tv_sec = sec;
	tval.tv_usec = 0;
	if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tval, sizeof(tval)) == -1)
#endif
	{
		//log_msg("ERROR: Socket unblocked error (SO_RCVTIMEO). errno: %u\n", errno);
		return false;
	}

#ifdef _WIN32
	tval = sec;
	if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&tval, sizeof(tval)) == -1)
#else
	tval.tv_sec = sec;
	tval.tv_usec = 0;
	if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tval, sizeof(tval)) == -1)
#endif
	{
		//log_msg("ERROR: Socket unblocked error (SO_SNDTIMEO). errno: %u\n", errno);
		return false;
	}

	return true;
}


bool checkOnNewData(int fd, int timeout)
{
	fd_set rfds;
	struct timeval tv;
	int retval;

	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);

	tv.tv_sec = 0;
	tv.tv_usec = timeout * 1000;

	retval = select(fd + 1, &rfds, NULL, NULL, &tv);

	return retval > 0;
}

bool readMessage(int fd, char *buffer, int bufferLen, int *messageLen)
{
	int index = 0;
	*messageLen = 0;

	while (1)
	{
		int recv_bytes = recv(fd, &buffer[index], bufferLen-index, 0);
		//printf("recv_bytes: %d errno: %d\n", recv_bytes, errno);
		index += recv_bytes;
		if (recv_bytes <= 0)
		{
			printf("readMessage: dispatcher's connection has broken. recv_bytes - %d\n", recv_bytes);
			return false;
		}

		if (!(*messageLen) && index >= 3)
		{
			*messageLen = *(int*)buffer;
			endianConversation((char*)messageLen, 4, 1);
			if ((*messageLen + 4) > (bufferLen-1))
			{
				printf("readMessage: bufferLen is not enough to read the message. messageLen - %d, buffer capacity - %d\n", *messageLen, bufferLen-1);
				return false;
			}
		}

		if (*messageLen && (index >= (*messageLen + 4)))
		{
			if (index > (*messageLen + 4))
				printf("readMessage: warning: got more bytes than necessary\n");

			memcpy(buffer, &buffer[4], *messageLen);
			buffer[*messageLen] = 0;

			char sz[15];
			strncpy(sz, buffer, 14);
			sz[*messageLen > 14 ? 14 : *messageLen] = 0;
			printf("recv_bytes %d messageLen %d %s\n", recv_bytes, *messageLen, sz);

			break;
		}


	}

	return true;
}


int up(const char * host, int port)
{
    // create socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd == -1)
    {
    	printf("can't create server socket\n");
        return -1;
    }

    // set options

    int yes = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes)) == -1)
    {
        printf("can't set options for server socket\n");

        closesock(fd);
        return -1;
    }

    // bind

    sockaddr_in localaddr;
    memset(&localaddr, 0, sizeof(localaddr));
    localaddr.sin_family = AF_INET;
    localaddr.sin_port = htons(port);
    if (!strcmp(host, ""))
        localaddr.sin_addr.s_addr = 0;
    else
        localaddr.sin_addr.s_addr = inet_addr(host);

    int res = bind(fd, (sockaddr *) &localaddr,  sizeof(localaddr));

    if (res == -1)
    {
        printf("bind error %d\n", errno);
        closesock(fd);
        return -1;
    }

    // listen

    res = listen(fd, 0x100);
    if (res)
    {
    	printf("Error of start listen\n");

        closesock(fd);
        return -1;
    }

    //setUnblockSocket(fd, 1);
    //for (int)

    return fd;
}


void endianConversation(char* buffer, int elementSize, int bufferSize)
{
	if (elementSize <= 1 || (elementSize % 2))
	{
		printf("Invalid size for endian convertion\n");
		return;
	}

	unsigned short a = 1;
	char *p = (char*)&a;
	if (p[0]) return;

	for (int i = 0; i < bufferSize; i++ )
	{
		char *p = &buffer[i * elementSize];
		for (int b = 0; b < elementSize / 2; b++)
		{
			char tmp = p[b];
			buffer[i] = buffer[elementSize];
			buffer[elementSize - 1 - b] = tmp;
		}
	}


}






