#pragma  once

#include <QList>
#include <QByteArray>

#include <core/group.h>

class IExcelParser
{
public:
	virtual QList<Group*> parse(const QByteArray &ba, bool *isOk, QString *errMsg) const = 0;
};
