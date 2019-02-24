#include "query_stat.h"

void QueryStat::increment(bool iserr, uint64_t size)
{
	QMutexLocker locker(&mutex);
	lastQueryTime = QDateTime::currentDateTimeUtc();

	if (pos == 100)
	{
		pos = 0;
	}

	if (sizeList.size() > pos)
	{
		sizeList.replace(pos, size);
	}
	else
	{
		sizeList.append(size);
	}
	pos++;

	if (iserr == true)
	{
		errCount++;
	}
	else
	{
		count++;
	}
}

int QueryStat::getQueryCount() const
{
	QMutexLocker locker(&mutex);
	return count;
}

int QueryStat::getErrorCount() const
{
	QMutexLocker locker(&mutex);
	return errCount;
}

uint64_t QueryStat::getAvgSize() const
{
	uint64_t res = 0;
	Q_FOREACH (uint64_t i, sizeList)
	{
		res += i;
	}

	if (sizeList.size() > 0)
	{
		res /= sizeList.size();
	}

	return res;
}

QDateTime QueryStat::getLastQueryTime() const
{
	return lastQueryTime;
}
