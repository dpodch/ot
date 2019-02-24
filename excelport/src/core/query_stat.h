#pragma once

#include <QDateTime>
#include <QMutex>
#include <QList>

class QueryStat
{
public:

	void increment(bool iserr, uint64_t size);
	int getQueryCount() const;
	int getErrorCount() const;
	uint64_t getAvgSize() const;

	QDateTime getLastQueryTime() const;

private:
	mutable QMutex mutex;
	int count = 0;
	int errCount = 0;
	QDateTime lastQueryTime;

	QList<uint64_t> sizeList;
	int pos = 0;
};
