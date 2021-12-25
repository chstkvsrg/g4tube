
#include "Geant4CalcUnitChannel.hh"

#include "UnitTransport.hh"
#include "RemoteUtils.hh"
#include "constants.hh"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

unsigned getMacHash();

Geant4CalcUnitChannel::Geant4CalcUnitChannel(IApp *app, UnitTransport *trans) :
	IUnitSideChannel(GEANT4CALC_CHANNEL_ID, trans), currentReq_(0), app_(app), eventsProcessed_(0), eventsDesired_(0), keepThreadsRunning_(true), active_(false)
{
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_create(&thread_, &attr, startRoutine, this);
	//pthread_join(thread[i], NULL);
	pthread_attr_destroy(&attr);

	pthread_mutex_init(&mutex_, nullptr);
	pthread_cond_init(&cond_, nullptr);
}

Geant4CalcUnitChannel::~Geant4CalcUnitChannel()
{
	stopThreads();
	pthread_mutex_destroy(&mutex_);
	pthread_cond_destroy(&cond_);
}

void* Geant4CalcUnitChannel::startRoutine(void *p)
{
	//while (thiz->eventsDesired_ && (thiz->eventsProcessed_ >= thiz->eventsDesired_) && thiz->keepThreadsRunning_)
	//while ((!thiz->eventsDesired_ || (thiz->eventsProcessed_ < thiz->eventsDesired_)) && thiz->keepThreadsRunning_)

	Geant4CalcUnitChannel *thiz = (Geant4CalcUnitChannel *) p;

	thiz->app_->threadInitialization();

	while (thiz->keepThreadsRunning_)
	{
		pthread_mutex_lock(&thiz->mutex_);

		while (!thiz->active_ && thiz->keepThreadsRunning_)
			pthread_cond_wait(&thiz->cond_, &thiz->mutex_);
		pthread_mutex_unlock(&thiz->mutex_);

		while (thiz->active_ && thiz->keepThreadsRunning_)
		{
			//sleep(1);
			//thiz->eventsProcessed_ += 10;
			thiz->app_->run(100000);
			thiz->eventsProcessed_ += 100000;

			//printf("%d events processed\n", thiz->eventsProcessed_);
		}
	}

	thiz->app_->threadDeinitialization();

	return nullptr;
}

bool Geant4CalcUnitChannel::process(char *buffer, int len)
{
	bool res = true;

	unsigned char req;

	unsigned id;

	sscanf(buffer, "%u|%hhu", &id, &req);

	currentReq_ = req;

	char sz[1024];
	sz[0] = 0;

	switch (req)
	{
		case GEANT4CALC_CHANNEL_UPDATE_REQ:
		{
			sscanf(buffer, "%u|%hhu|%1023[^|]", &id, &req, sz);

			FILE *f = fopen("params", "w");
			size_t writtenSize = fprintf(f, "%s", sz);
			fclose(f);

			bool ok = writtenSize == strlen(sz);

			if (ok)
			{
				int index = 0;
				int c = 0;
				while (c != 3) if (buffer[index++] == '|') c++;

				f = fopen("1.tar.bz2", "w");
				len -= index;
				writtenSize = fwrite(&buffer[index], 1, len, f);
				fclose(f);

				ok = writtenSize == len;
			}

			sprintf(buffer, "%u|%hhu|%d", id_, req, ok ? 1 : 0 );
			trans_->sendData(buffer, strlen(buffer));

			if (!ok) break;

			f = fopen("to_be_executed.sh", "w");
			writtenSize = fprintf(f, "src/update.sh");
			fclose(f);

			stopThreads();
			trans_->saveDispatcherHost();
			trans_->disconnect();

			res = false;
			//int r = system("src/update.sh"); (void)r;
			//exit(0);

		} break;

		case GEANT4CALC_CHANNEL_RESTART_REQ:
		{
			sscanf(buffer, "%u|%hhu|%1023[^|]", &id, &req, sz);
			FILE *f = fopen("params", "w");
			size_t writtenSize = fprintf(f, "%s", sz);
			fclose(f);

			bool ok = writtenSize == strlen(sz);

			sprintf(buffer, "%u|%hhu|%d", id_, req, ok  ? 1 : 0 );
			trans_->sendData(buffer, strlen(buffer));

			if (!ok) break;

			f = fopen("to_be_executed.sh", "w");
			writtenSize = fprintf(f, "src/restart.sh");
			fclose(f);

			stopThreads();
			trans_->saveDispatcherHost();
			trans_->disconnect();
			res = false;
			//int r = system("src/restart.sh"); (void)r;
			//exit(0);
		}
		break;

		case GEANT4CALC_CHANNEL_GET_UNIT_INFO:
		{
			/*char hostname[HOST_NAME_MAX];
			char username[LOGIN_NAME_MAX];
			gethostname(hostname, HOST_NAME_MAX);
			getlogin_r(username, LOGIN_NAME_MAX);*/

			FILE *cpuinfo = fopen("/proc/cpuinfo", "rb");
			char *arg = 0;
			size_t size = 0;
			char szProcName[51];
			char szProcName2[51];
			while(getdelim(&arg, &size, '\n', cpuinfo) != -1)
			{
				if (strstr(arg, "model name"))
				{
					sscanf(arg, "%50[^:]: %50[^ ] %50[^ ] %50[^ ]", szProcName, szProcName, szProcName, szProcName2);
					break;
				}
			}
			free(arg);
			fclose(cpuinfo);

			sprintf(buffer, "%u|%hhu|%s_%s_%u", id_, req, szProcName, szProcName2, getMacHash());
			trans_->sendData(buffer, strlen(buffer));
		} break;

		case GEANT4CALC_CHANNEL_RUN_REQ:
		{
			//sscanf(buffer, "%u|%hhu|%llu", &id, &req, &eventsDesired_);

			pthread_mutex_lock(&mutex_);
			active_ = true;
			pthread_cond_signal(&cond_);
			pthread_mutex_unlock(&mutex_);

			sprintf(buffer, "%u|%hhu|1", id, req);
			trans_->sendData(buffer, strlen(buffer));

		} break;

		case GEANT4CALC_CHANNEL_STOP_REQ:
		{
			pthread_mutex_lock(&mutex_);
			active_ = false;
			pthread_cond_signal(&cond_);
			pthread_mutex_unlock(&mutex_);

			sprintf(buffer, "%u|%hhu|1", id, req);
			trans_->sendData(buffer, strlen(buffer));
		} break;

		case GEANT4CALC_CHANNEL_GET_PROCESSED_EVENTS_REQ:
		{
			sprintf(buffer, "%u|%hhu|%llu", id, req, eventsProcessed_);
			trans_->sendData(buffer, strlen(buffer));

		} break;

		case GEANT4CALC_CHANNEL_DOWNLOAD_STATS_REQ:
		{
			sprintf(buffer, "%u|%hhu|", id_, req);
			int index = strlen(buffer);

			// TODO: check wheither buffer capacity is enough to load data using (nullptr) call

			app_->fillBufferWithGatheredResults(buffer + index, &len);
			endianConversation(buffer + index, sizeof(uint), len / sizeof(uint));
			trans_->sendData(buffer, index + len);
		} break;

		case GEANT4CALC_CHANNEL_GET_TEMP:
		{
			int temp = 0;
			for (int i = 0; i < 6; i++)
			{
				sprintf(sz, "/sys/class/hwmon/hwmon%d/temp1_input", i);
				if (!access(sz, F_OK))
				{
					FILE *f = fopen(sz, "r");
					size_t r = fread(sz, 1, sizeof(sz)-1, f); (void)r;
					fclose(f);
					temp = atoi(sz) / 1000;
					break;
				}
			}

			sprintf(buffer, "%u|%hhu|%u", id_, req, temp);
			trans_->sendData(buffer, strlen(buffer));

		} break;
	}

	return res;
}

void Geant4CalcUnitChannel::stopThreads()
{
	pthread_mutex_lock(&mutex_);
	keepThreadsRunning_ = false;
	pthread_cond_signal(&cond_);
	pthread_mutex_unlock(&mutex_);

	pthread_join(thread_, NULL);
}


