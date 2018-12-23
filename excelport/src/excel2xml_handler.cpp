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

#include <xlnt/xlnt.hpp>
#include <iostream>
#include <sstream>
#include <QFile>
void Excel2XmlHandler::storeBody()
{
	QByteArray ba = request->body();

	if (ba.isEmpty())
	{
		sendReceipt(*response, QHttpResponse::STATUS_BAD_REQUEST,
					QByteArray("post body is empty"), "text/plain");
		return;
	}

	std::istringstream in(std::string(ba.data(), ba.size()));
	xlnt::workbook excelFile;
	excelFile.load(in);

	xlnt::worksheet ws = excelFile.active_sheet();
	std::string cellValue = ws.cell("A1").value<std::string>();

	QDomDocument doc;
		doc.appendChild(
			doc.createProcessingInstruction(
				"xml", "version=\"1.0\" encoding=\"utf-8\""));

	QDomElement root = doc.createElement("first_cell_value");
	root.setAttribute("A1", QString(cellValue.c_str()));
	doc.appendChild(root);

	sendReceipt(*response, QHttpResponse::STATUS_OK, doc.toByteArray(), "text/xml");
}
