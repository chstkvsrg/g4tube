/*
 * RemoteUtils.h
 *
 *  Created on: Jul 2, 2015
 *      Author: koza
 */

#pragma once


void msleep(int msec);
void closesock(int fd);
bool setUnblockSocket(int fd, int sec);
bool checkOnNewData(int fd, int timeout); // timeout in ms
bool readMessage(int fd, char *buffer, int bufferLen, int *messageLen);
int up(const char * host, int port);

void endianConversation(char* buffer, int elementSize, int bufferSize);

