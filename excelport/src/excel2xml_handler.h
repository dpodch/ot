#pragma once

#include <QMutex>
#include <QDateTime>

#include <rest/http_handler.h>

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

class Excel2XmlHandler : public HttpHandler
{
	Q_OBJECT
public:
	Excel2XmlHandler();

	virtual HttpHandler* clone();

	virtual QString helpText() const;
	virtual void handleRequest(QHttpRequest *request, QHttpResponse *response);

private:
	void sendReceipt(QHttpResponse &response,
					 QHttpResponse::StatusCode code,
					 const QByteArray &data,
					 const QString &contentType);

private Q_SLOTS:
	void storeBody();

private:
	QHttpRequest *request;
	QHttpResponse* response;
	static QueryStat *stat;
};
