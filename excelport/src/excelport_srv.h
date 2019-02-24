#pragma once

#include <core/query_stat.h>
#include <QObject>

#include <served/served.hpp>

class ExcelportSrv :public QObject
{
	Q_OBJECT
public:
	ExcelportSrv(int port, int threadCount);
	virtual ~ExcelportSrv();

	void start();

private Q_SLOTS:
	void run();

private:
	void getStatQuery(served::response &resp, const served::request &req);
	void postXls(served::response &resp, const served::request &req);

	void setText(served::response &resp,
				 const QByteArray &message,
				 int httpStatus) const;

	void setJson(served::response &resp,
				 const QByteArray &message,
				 int httpStatus) const;

private:
	QueryStat stat;
	int port = 12100;
	int maxThreadCount = 10;
};
