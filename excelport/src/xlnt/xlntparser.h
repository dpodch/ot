#pragma once

#include <core/i_excel_parser.h>
#include <xlnt/i_format_parser.h>

namespace xlnt
{
	class worksheet;
}

class XlntParser :public IExcelParser
{
public:
	XlntParser();
	virtual ~XlntParser();

	virtual QList<Group*> parse(const QByteArray &ba, bool *isOk, QString *errMsg) const;

private:
	People* createPeople(const QString &str) const;

private:
	QList<IFormatParser*> parserList;
};
