/*
 * Exception.h
 *
 *  Created on: Jun 25, 2019
 *      Author: fna
 */

#pragma once

#include <iostream>
#include <string.h>

class Exception
{
public:
	Exception(int fd, const std::string& str) : fd_(fd), str_(str) { }
	~Exception() {}

	int fd() const { return fd_; }
	std::string errorStr() const { return str_; }

private:
	int fd_;
	std::string str_;
};
