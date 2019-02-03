#pragma once

#include <QMap>
#include <QString>

namespace xlnt
{
	class worksheet;
}

enum LineType
{
	LT_NULL,
	LT_GROUP,
	LT_PEOPLE
};

struct ExcelItem
{
	LineType type = LT_NULL;
	QString gName;
	QString pName;
	double rateCount = 0;
};

class IFormatParser
{
public:
	virtual QMap<int, ExcelItem> parse(const xlnt::worksheet &ws) const = 0;
	virtual ~IFormatParser(){}
};
