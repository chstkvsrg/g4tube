/*
 * IChannel.h
 *
 *  Created on: Jun 24, 2019
 *      Author: fna
 */

#pragma once

class IChannel
{
public:
	IChannel(unsigned id) : id_(id) {}
	virtual ~IChannel() {}

	unsigned id() { return id_; }

	virtual bool process(char *buffer, int len) = 0;

protected:

	unsigned id_;
};

class UnitTransport;

class IUnitSideChannel : public IChannel
{
public:
	IUnitSideChannel(unsigned id, UnitTransport *trans) : IChannel(id), trans_(trans) {}
	virtual ~IUnitSideChannel() {}

protected:

	UnitTransport *trans_;
};

class DispatcherTransport;

class IDispatcherSideChannel : public IChannel
{
public:
	IDispatcherSideChannel(unsigned id, DispatcherTransport *trans, int fd) : IChannel(id), trans_(trans), fd_(fd) {}
	virtual ~IDispatcherSideChannel() {}

	int fd() { return fd_; }

protected:

	DispatcherTransport *trans_;
	int fd_;
};
