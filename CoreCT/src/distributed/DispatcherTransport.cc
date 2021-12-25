/*
 * DispatcherTransport.cxx
 *
 *  Created on: Jun 22, 2019
 *      Author: fna
 */

#include "DispatcherTransport.hh"
#include "RemoteUtils.hh"
#include "Exception.hh"
#include "constants.hh"

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>



DispatcherTransport::DispatcherTransport() : keepAcceptThreadRunning_(false)
{
	buffer_ = new char[BUFFER_LEN];

	startAcception();

	pthread_mutex_init(&mutex_, nullptr);
}

DispatcherTransport::~DispatcherTransport()
{
	delete []buffer_;

	pthread_mutex_destroy(&mutex_);
}

void* DispatcherTransport::startRoutine(void *p)
{
	DispatcherTransport *thiz = (DispatcherTransport *) p;

	int fd = up("", DEFAULT_PORT);
	if (fd < 0)
	{
		printf("fail to up server\n");
		return nullptr;
	}

    int fd_client;
    sockaddr_in client_addr;
    int client_addr_size = sizeof(client_addr);

    //printf("waiting connections...\n");

    int count = 0;

    while (thiz->keepAcceptThreadRunning_)
    {
    	if (!checkOnNewData(fd, 1))
    		continue;

		fd_client = accept(fd, (sockaddr *) &client_addr, (socklen_t*) &client_addr_size);
		/*unsigned char cmd[2];
		int received = recv(fd_client, (char*)cmd, 1, 0);
		cmd[1] = 0;

		printf("received - %s, count: %d\n", cmd, ++count);*/

		pthread_mutex_lock(&thiz->mutex_);
		thiz->lstFDs_.push_back(fd_client);
		printf("accepted! current units count: %d\n", (int)thiz->lstFDs_.size());
		pthread_mutex_unlock(&thiz->mutex_);
    }


}


void DispatcherTransport::sendBroadCastMessage(const char *host)
{
	//lstFDs_.clear();

	int sd = socket(AF_INET, SOCK_DGRAM, 0);

	if ( sd < 0 )
		//handle_error("socket failed");
		return;

	const int on=1;
	if ( setsockopt(sd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)) != 0 )
		//handle_error("set broadcast failed");
		return;

	struct sockaddr_in addr;

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(DEFAULT_PORT);
	if ( inet_aton(host, &addr.sin_addr) == 0 )
        //handle_error("inet_aton failed (%s)", argv[1]);
		return;

	char szBuffer[50];
    sprintf(szBuffer, "192.168.0.108 %d", DEFAULT_PORT);
	if ( sendto(sd, szBuffer, strlen(szBuffer)+1, 0, (struct sockaddr*)&addr, sizeof(addr)) < 0 )
		//handle_error("sendto");
		return;

	closesock(sd);

	sleep(2);

}

void DispatcherTransport::sendARecv(int fd, char *data, int *len, int bufferLen)
{
	int uni = *len;
	endianConversation((char*)&uni, sizeof(uni), 1);
	unsigned long sent = send(fd, (const char*)&uni, sizeof(uni), 0);
	if (sent != sizeof(uni))
		throw Exception(fd, "Failed to send the message length field");

	/*char sz[15];
	strncpy(sz, data, 14);
	sz[*len > 14 ? 14 : *len] = 0;
	printf("send %s\n", sz);*/
	sent = send(fd, (const char*)data, *len, 0);
	if (sent != *len)
		throw Exception(fd, "Failed to send the message");

	if (!readMessage(fd, data, bufferLen, len))
		throw Exception(fd, "Failed to read the message");
}

void DispatcherTransport::disconnect(int fd)
{
	pthread_mutex_lock(&mutex_);
	closesock(fd);
	lstFDs_.remove(fd);
	pthread_mutex_unlock(&mutex_);
}

std::list<int> DispatcherTransport::unitConnections()
{
	std::list<int> lst;
	pthread_mutex_lock(&mutex_);
	lst = lstFDs_;
	pthread_mutex_unlock(&mutex_);
	return lst;
}

void DispatcherTransport::startAcception()
{
	if (keepAcceptThreadRunning_) return;

	keepAcceptThreadRunning_ = true;

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_create(&thread_, &attr, startRoutine, this);
	//pthread_join(thread[i], NULL);
	pthread_attr_destroy(&attr);
}


void DispatcherTransport::stopAcception()
{
	if (!keepAcceptThreadRunning_) return;
	keepAcceptThreadRunning_ = false;

	pthread_join(thread_, nullptr);
}


