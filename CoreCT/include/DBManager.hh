#pragma once


struct sqlite3;

#include <iostream>
#include <map>

class DBManager
{
public:
	DBManager();
	~DBManager();

	bool open();
	void close();

	void beginTransaction();
	void endTransaction();

	int progress();
	void setProgress(int value);

	// fill
	void addTempValue(const std::string &unit, int temp);
	void updateEventProcessedValue(const std::string &unit, unsigned long long events);

	// read
	bool hasNewData();
	std::map<std::string, int> lastTemperatures();
	std::map<std::string, unsigned long long> eventsProcessed();


	void cleanDB();

private:
	sqlite3 *db_;

	int rc_;
	char sql_[2 * 1024];
	char tmp_[100];
	char *err_;

	unsigned long long lastId_;
	std::map<std::string, int> mapTemps_;


	std::string datetime();

};




