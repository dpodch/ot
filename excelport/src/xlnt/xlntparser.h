#pragma once

#include <core/i_excel_parser.h>

class XlntParser :public IExcelParser
{
public:
	XlntParser();
	virtual ~XlntParser();

	virtual QList<Firm> parse(const QByteArray &ba, bool *isOk, QString *errMsg) const;
};
