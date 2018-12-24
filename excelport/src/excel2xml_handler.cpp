// Copyright (c) 2018 Forward Systems. All rights reserved

#include <QDebug>
#include <QByteArray>
#include <QDomDocument>
#include <QDomElement>

#include "excel2xml_handler.h"

Excel2XmlHandler::Excel2XmlHandler()
{
}

HttpHandler *Excel2XmlHandler::clone()
{
	Excel2XmlHandler* res = new Excel2XmlHandler();
	return res;
}

QString Excel2XmlHandler::helpText() const
{
	QString str = QString("%1")
			.arg(trUtf8("Выполняет преобразоание формата excel в xml"));
	return str;
}

void Excel2XmlHandler::handleRequest(QHttpRequest *request, QHttpResponse *response)
{
	this->request = request;
	this->response = response;

	if (request->method() == QHttpRequest::HTTP_POST)
	{
		request->storeBody();
		connect(request, SIGNAL(end()), this, SLOT(storeBody()));
	}
	else if (request->method() == QHttpRequest::HTTP_GET)
	{
		sendReceipt(*response, QHttpResponse::STATUS_OK, "Echo OK", "text/plain");
	}
	else
	{
		sendReceipt(*response, QHttpResponse::STATUS_NOT_IMPLEMENTED,
					"Error method not implemented", "text/plain");
	}
}

void Excel2XmlHandler::sendReceipt(QHttpResponse &response,
											QHttpResponse::StatusCode code,
											const QByteArray &data,
											const QString &contentType)
{
	if (!m_responeDestroyed)
	{
		response.setHeader("Content-Type", contentType);
		response.setHeader("Content-Length", QString::number(data.size()));
		response.writeHead(code);
		response.write(data);
		response.end();
	}

	if (code != QHttpResponse::STATUS_OK)
	{
		qCritical() << Q_FUNC_INFO << "Error processing query";
	}

	Q_EMIT finished();
}

#include <QJson/Serializer>
#include <xlnt/xlntparser.h>
void Excel2XmlHandler::storeBody()
{
	QByteArray ba = request->body();

	if (ba.size() < 100)
	{
		sendReceipt(*response, QHttpResponse::STATUS_BAD_REQUEST,
					QByteArray("post body is empty"), "text/plain");
		return;
	}

	bool isOk = true;
	QString errmsg = "";
	QVariantList fList =  Firm::toVariantList(XlntParser().parse(ba, &isOk, &errmsg));

	if (isOk == true)
	{
		QVariantMap map;
		map.insert("version", "1");
		map.insert("firm_list", fList);

		QJson::Serializer s;
		s.setFullEscapeEnabled(false);

		qDebug() << s.serialize(map);
		sendReceipt(*response, QHttpResponse::STATUS_OK, s.serialize(map), "text/json");
	}
	else
	{
		QString msg = "Error parse xls format:" + errmsg;
		sendReceipt(*response, QHttpResponse::STATUS_BAD_REQUEST,
					msg.toUtf8(), "text/plain");
	}
}
