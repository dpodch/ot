#include <xlnt/xlnt.hpp>
#include <utils/exceptions.hpp>
#include <iostream>
#include <sstream>
#include <QFile>
#include <QDebug>

#include "xlntparser.h"

XlntParser::XlntParser()
{

}

XlntParser::~XlntParser()
{

}

QList<Firm> XlntParser::parse(const QByteArray &ba, bool *isOk, QString *errMsg) const
{
	try
	{
		std::istringstream in(std::string(ba.data(), ba.size()));
		xlnt::workbook excelFile;
		excelFile.load(in);

		xlnt::worksheet ws = excelFile.active_sheet();
		std::string cellValue = ws.cell("A1").value<std::string>();
		qDebug() <<"First cell = " << QString(cellValue.c_str());
	}
	catch(xlnt::exception &e)
	{
		*isOk = false;
		errMsg->append(QString(e.what()));
	}
	catch(...)
	{
		*isOk = false;
		errMsg->append("Unknown error");
	}

	if (*isOk == false)
	{
		qCritical() << Q_FUNC_INFO << ":" << *errMsg;
	}

	return QList<Firm>();
}
