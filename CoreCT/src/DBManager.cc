

#include "DBManager.hh"

#include <sqlite3.h>


#define handleError() \
	if (rc_) {\
		const char *err = sqlite3_errmsg(db_); \
		printf("Ошибка SQL: %s\nerror: %s\n", sql_, err ? err : ""); \
	}


DBManager::DBManager(): db_(nullptr), lastId_(0)
{
}

DBManager::~DBManager()
{

}

bool DBManager::open()
{
	if (sqlite3_open("g4_distributed_state.sqlite", &db_))
	{
		printf("Fail to open db: %s\n", sqlite3_errmsg(db_));
		sqlite3_close(db_);
		db_ = nullptr;
	}

	return db_ != nullptr;
}


void DBManager::close()
{
	if (db_)
	{
		sqlite3_close(db_);
		db_ = nullptr;
	}
}

void DBManager::beginTransaction()
{
	if (!db_) return;
	rc_ = sqlite3_exec(db_, "BEGIN IMMEDIATE TRANSACTION;", nullptr, nullptr, nullptr); handleError();
}

void DBManager::endTransaction()
{
	if (!db_) return;
	rc_ = sqlite3_exec(db_, "COMMIT TRANSACTION;", nullptr, nullptr, nullptr); handleError();
}


std::string DBManager::datetime()
{
    time_t curtime = time(NULL);
    struct tm *tm_ = localtime(&curtime);
    sprintf(tmp_, "%04d-%02d-%02d_%02d:%02d:%02d", 1900 + tm_->tm_year, tm_->tm_mon + 1, tm_->tm_mday, tm_->tm_hour, tm_->tm_min, tm_->tm_sec);
    return tmp_;
}

int DBManager::progress()
{
	if (!db_) return 0;

	sqlite3_stmt* stmt = 0;
	rc_ = sqlite3_prepare_v2( db_, "SELECT progress FROM progress", -1, &stmt, 0 ); handleError();

	if (sqlite3_step( stmt ) != SQLITE_ROW)
	{
		printf("Statement error: expected SQLITE_ROW, but got another rc!\n");
		sqlite3_finalize( stmt );
		return 0;
	}

	int res = sqlite3_column_int( stmt, 0 );
	sqlite3_finalize( stmt );

	return res;

}

void DBManager::setProgress(int value)
{
	if (!db_) return;

	sprintf(sql_, "UPDATE progress SET progress='%d'", value);
	rc_ = sqlite3_exec(db_, sql_, nullptr, nullptr, nullptr); handleError();
}

void DBManager::addTempValue(const std::string &unit, int temp)
{
	if (!db_) return;

	sprintf(sql_, "INSERT INTO 'temps' (name, temp, datetime) VALUES('%s', '%d', '%s')", unit.c_str(), temp, datetime().c_str());
    rc_ = sqlite3_exec(db_, sql_, nullptr, nullptr, nullptr); handleError();
}

void DBManager::updateEventProcessedValue(const std::string &unit, unsigned long long events)
{
	if (!db_) return;

	sqlite3_stmt* stmt = 0;
	sprintf(sql_, "SELECT count(*) FROM events WHERE name='%s'", unit.c_str());
	rc_ = sqlite3_prepare_v2( db_, sql_, -1, &stmt, 0 ); handleError();

	if (sqlite3_step( stmt ) != SQLITE_ROW)
	{
		printf("Statement error: expected SQLITE_ROW, but got another rc!\n");
		sqlite3_finalize( stmt );
		return;
	}

	bool exists = sqlite3_column_int( stmt, 0 );
	sqlite3_finalize( stmt );

	if (exists)
		sprintf(sql_, "UPDATE events SET events='%llu' WHERE name='%s'", events, unit.c_str());
	else
		sprintf(sql_, "INSERT INTO 'events' VALUES('%s', '%llu')", unit.c_str(), events);

	rc_ = sqlite3_exec(db_, sql_, 0, 0, 0); handleError();
}


bool DBManager::hasNewData()
{
	if (!db_) return false;

	sqlite3_stmt* stmt = 0;
	sprintf(sql_, "SELECT max(id) FROM temps");
	rc_ = sqlite3_prepare_v2( db_, sql_, -1, &stmt, 0 ); handleError();

	if (sqlite3_step( stmt ) != SQLITE_ROW)
	{
		printf("Statement error: expected SQLITE_ROW, but got another rc!\n");
		sqlite3_finalize( stmt );
		return false;
	}

	unsigned long long maxId = sqlite3_column_int64( stmt, 0 );
	sqlite3_finalize( stmt );

	bool res = (lastId_ < maxId);

	return res;
}


std::map<std::string, int> DBManager::lastTemperatures()
{
	if (!db_) return mapTemps_;

	sqlite3_stmt* stmt = 0;
	sprintf(sql_, "SELECT id, name, temp, max(datetime) FROM temps\
					WHERE id>%llu\
					GROUP BY name",
			lastId_);

	rc_ = sqlite3_prepare_v2( db_, sql_, -1, &stmt, 0 ); handleError();

	while (sqlite3_step( stmt ) == SQLITE_ROW)
	{
		unsigned long long id = sqlite3_column_int64( stmt, 0 );
		if (id > lastId_) lastId_ = id;

		mapTemps_[(const char*)sqlite3_column_text( stmt, 1 )] = sqlite3_column_int( stmt, 2 );
	}
	sqlite3_finalize( stmt );

	return mapTemps_;
}

std::map<std::string, unsigned long long> DBManager::eventsProcessed()
{
	std::map<std::string, unsigned long long> mapEvents;

	if (!db_) return mapEvents;

	sqlite3_stmt* stmt = 0;
	sprintf(sql_, "SELECT name, events FROM events");

	rc_ = sqlite3_prepare_v2( db_, sql_, -1, &stmt, 0 ); handleError();

	while (sqlite3_step( stmt ) == SQLITE_ROW)
		mapEvents[(const char*)sqlite3_column_text( stmt, 0 )] = sqlite3_column_int64( stmt, 1 );
	sqlite3_finalize( stmt );

	return mapEvents;
}


void DBManager::cleanDB()
{
	if (!db_) return;

	time_t curtime = time(NULL)-86400;
    struct tm *tm_ = localtime(&curtime);
    sprintf(tmp_, "%04d-%02d-%02d_%02d:%02d:%02d", 1900 + tm_->tm_year, tm_->tm_mon + 1, tm_->tm_mday, tm_->tm_hour, tm_->tm_min, tm_->tm_sec);
	sprintf(sql_, "DELETE FROM 'temps' WHERE datetime < '%s'", tmp_);
	rc_ = sqlite3_exec(db_, sql_, 0, 0, 0); handleError();
}







