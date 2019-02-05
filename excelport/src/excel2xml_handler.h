#pragma once

#include <QMutex>

#include <rest/http_handler.h>

class QueryStat
{
public:

	void increment(bool iserr);
	int getQueryCount() const;
	int getErrorCount() const;

private:
	mutable QMutex mutex;
	int count = 0;
	int errCount = 0;
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
