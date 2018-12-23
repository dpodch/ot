#pragma once

#include <rest/http_handler.h>

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
};
