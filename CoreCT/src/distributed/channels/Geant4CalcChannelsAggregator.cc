/*
 * Geant4CalcChannelsAggregator.cxx
 *
 *  Created on: Jun 27, 2019
 *      Author: fna
 */

#include "Geant4CalcChannelsAggregator.hh"
#include "Geant4CalcDispatcherChannel.hh"
#include "Exception.hh"
#include "constants.hh"

#include <string.h>


Geant4CalcChannelsAggregator::Geant4CalcChannelsAggregator(DispatcherTransport *trans) : trans_(trans)
{

}

Geant4CalcChannelsAggregator::~Geant4CalcChannelsAggregator()
{
	clean();
}

void Geant4CalcChannelsAggregator::clean()
{
	for (std::map<int, Geant4CalcDispatcherChannel*>::iterator itrCh = map_.begin(); itrCh != map_.end(); itrCh++)
		delete itrCh->second;
	map_.clear();
}

void Geant4CalcChannelsAggregator::disconnect()
{
	for (std::map<int, Geant4CalcDispatcherChannel*>::iterator itrCh = map_.begin(); itrCh != map_.end(); itrCh++)
		itrCh->second->disconnect();

	clean();
}

void Geant4CalcChannelsAggregator::setChannels(const std::list<int> &fds)
{
	clean();

	for (std::list<int>::const_iterator itrFD = fds.begin(); itrFD != fds.end(); itrFD++)
		map_[*itrFD] = new Geant4CalcDispatcherChannel("1.tar.bz2", trans_, *itrFD);
}

void Geant4CalcChannelsAggregator::update(const char *szParams)
{
	for (std::map<int, Geant4CalcDispatcherChannel*>::iterator itrCh = map_.begin(); itrCh != map_.end(); )
	{
		try
		{
			itrCh->second->update(szParams);
		}
		catch (const Exception& exception)
		{
			printf("An error has occured for fd %d: %s\n", exception.fd(), exception.errorStr().c_str());
			itrCh->second->disconnect();
			delete itrCh->second;
			map_.erase(itrCh++);
			continue;
		}
		itrCh++;
	}
}

void Geant4CalcChannelsAggregator::restart(const char *szParams)
{
	for (std::map<int, Geant4CalcDispatcherChannel*>::iterator itrCh = map_.begin(); itrCh != map_.end(); )
	{
		try
		{
			itrCh->second->restart(szParams);
		}
		catch (const Exception& exception)
		{
			printf("An error has occured for fd %d: %s\n", exception.fd(), exception.errorStr().c_str());
			itrCh->second->disconnect();
			delete itrCh->second;
			map_.erase(itrCh++);
			continue;
		}
		itrCh++;
	}
}

void Geant4CalcChannelsAggregator::run(int upToEvents)
{
	for (std::map<int, Geant4CalcDispatcherChannel*>::iterator itrCh = map_.begin(); itrCh != map_.end(); )
	{
		try
		{
			itrCh->second->run(upToEvents);
		}
		catch (const Exception& exception)
		{
			printf("An error has occured for fd %d: %s\n", exception.fd(), exception.errorStr().c_str());
			itrCh->second->disconnect();
			delete itrCh->second;
			map_.erase(itrCh++);
			continue;
		}
		itrCh++;
	}
}

void Geant4CalcChannelsAggregator::stop()
{
	for (std::map<int, Geant4CalcDispatcherChannel*>::iterator itrCh = map_.begin(); itrCh != map_.end(); )
	{
		try
		{
			itrCh->second->stop();
		}
		catch (const Exception& exception)
		{
			printf("An error has occured for fd %d: %s\n", exception.fd(), exception.errorStr().c_str());
			itrCh->second->disconnect();
			delete itrCh->second;
			map_.erase(itrCh++);
			continue;
		}
		itrCh++;
	}
}

unsigned long long Geant4CalcChannelsAggregator::eventProcessed()
{
	unsigned long long e = 0;
	for (std::map<int, Geant4CalcDispatcherChannel*>::iterator itrCh = map_.begin(); itrCh != map_.end(); )
	{
		try
		{
			e += itrCh->second->eventProcessed();
		}
		catch (const Exception& exception)
		{
			printf("An error has occured for fd %d: %s\n", exception.fd(), exception.errorStr().c_str());
			itrCh->second->disconnect();
			delete itrCh->second;
			map_.erase(itrCh++);
			continue;
		}
		itrCh++;
	}

	return e;
}

unsigned long long Geant4CalcChannelsAggregator::eventProcessed(int fd)
{
	unsigned long long e = 0;
	Geant4CalcDispatcherChannel *channel = map_[fd];

	try
	{
		e = channel->eventProcessed();
	}
	catch (const Exception& exception)
	{
		printf("An error has occured for fd %d: %s\n", exception.fd(), exception.errorStr().c_str());
		channel->disconnect();
		delete channel;
		map_.erase(fd);
	}

	return e;
}

char* Geant4CalcChannelsAggregator::getInfo(int fd)
{
	char *p = nullptr;
	Geant4CalcDispatcherChannel *channel = map_[fd];

	try
	{
		p = channel->getUnitInfo();
	}
	catch (const Exception& exception)
	{
		printf("An error has occured for fd %d: %s\n", exception.fd(), exception.errorStr().c_str());
		channel->disconnect();
		delete channel;
		map_.erase(fd);
	}

	return p;
}


void Geant4CalcChannelsAggregator::downloadStats(char* buffer, int *len)
{
	char* p = nullptr;

	bool first = true;
	for (std::map<int, Geant4CalcDispatcherChannel*>::iterator itrCh = map_.begin(); itrCh != map_.end(); )
	{
		try
		{
			itrCh->second->downloadStats(first ? buffer : p, len);

			if (first)
				p = new char[*len];
			else
			{
				uint *buffer_ = (uint *)buffer;
				uint *p_ = (uint *)p;
				for (int i = 0; i < (*len / sizeof(uint)); i++)
					buffer_[i] += p_[i];
			}
			first = false;

			uint *buffer_ = (uint *)buffer;
			for (int i = 0; i < (*len / sizeof(uint)); i++)
				std::cout << buffer_[i] << " " << std::endl;

		}
		catch (const Exception& exception)
		{
			printf("An error has occured for fd %d: %s\n", exception.fd(), exception.errorStr().c_str());
			itrCh->second->disconnect();
			map_.erase(itrCh++);
			continue;
		}
		itrCh++;
	}

	if (p) delete []p;
}

unsigned Geant4CalcChannelsAggregator::temperature(int fd)
{
	unsigned temp = 0;
	Geant4CalcDispatcherChannel *channel = map_[fd];

	try
	{
		temp = channel->temperature();
	}
	catch (const Exception& exception)
	{
		printf("An error has occured for fd %d: %s\n", exception.fd(), exception.errorStr().c_str());
		channel->disconnect();
		delete channel;
		map_.erase(fd);
	}

	return temp;
}

