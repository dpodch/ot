#pragma  once

#include <QList>
#include <QByteArray>

#include <core/firm.h>

class IExcelParser
{
public:
	virtual QList<Firm*> parse(const QByteArray &ba, bool *isOk, QString *errMsg) const = 0;
};
