// Copyright (c) 2018 Forward Systems. All rights reserved

#include <QDebug>
#include <QByteArray>
#include <QDomDocument>
#include <QDomElement>

#include <QJson/Serializer>
#include <xlnt/xlntparser.h>

#include "excel2xml_handler.h"

QueryStat* Excel2XmlHandler::stat = new QueryStat;

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
			.arg(trUtf8("Выполняет преобразование формата excel в json"));
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
		QString msg("Total count: %1\nError count: %2\nlast query(utc): %3\navg size: %4");
		msg = msg.arg(stat->getQueryCount())
				.arg(stat->getErrorCount())
				.arg(stat->getLastQueryTime().toString(Qt::ISODate))
				.arg(stat->getAvgSize());
		sendReceipt(*response, QHttpResponse::STATUS_OK, msg.toUtf8(), "text/plain");
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

void Excel2XmlHandler::storeBody()
{
	QByteArray ba = request->body();

	if (ba.size() < 100)
	{
		sendReceipt(*response, QHttpResponse::STATUS_BAD_REQUEST,
					QByteArray("post body is empty"), "text/plain");
		stat->increment(true, ba.size());
		return;
	}
	else if (ba.size() > 100 * 1000 * 1000)
	{
		stat->increment(true, ba.size());
		sendReceipt(*response, QHttpResponse::STATUS_BAD_REQUEST,
					QByteArray("file is very large!! Max < 100MB"), "text/plain");
		return;
	}


	bool isOk = true;
	QString errmsg = "";

	QList<Group *> groups = XlntParser().parse(ba, &isOk, &errmsg);
	QVariantList fList =  Group::toVariantList(groups);
	qDeleteAll(groups);
	groups.clear();

	if (isOk == true)
	{
		QVariantMap map;
		map.insert("version", "1");
		map.insert("firms_list", fList);

		QJson::Serializer s;
		s.setFullEscapeEnabled(false);

		sendReceipt(*response, QHttpResponse::STATUS_OK, s.serialize(map), "text/json");
		stat->increment(false, ba.size());
	}
	else
	{
		stat->increment(true, ba.size());
		QString msg = "Error parse xls format " + errmsg;
		sendReceipt(*response, QHttpResponse::STATUS_BAD_REQUEST,
					msg.toUtf8(), "text/plain");
	}
}

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
