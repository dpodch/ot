
#include <QVariant>
#include <QVariantMap>

#include "people.h"

People::People()
{

}

People::~People()
{

}

QVariant People::toVariant() const
{
	QVariantMap map;
	map.insert("name", name);
	map.insert("patronymic", patronymic);
	map.insert("surname", surname);
	map.insert("birthday", birthday.toString("yyyy-MM-dd"));
	map.insert("employment_date", employment_date.toString("yyyy-MM-dd"));

	if (rateCount > 0)
	{
		map.insert("rate_count", (double)rateCount / 100);
	}

	return map;
}

void People::setName(const QString &value)
{
	name = value;
}

void People::setPatronymic(const QString &value)
{
	patronymic = value;
}

void People::setSurname(const QString &value)
{
	surname = value;
}

void People::setBirthday(const QDate &value)
{
	birthday = value;
}

void People::setEmployment_date(const QDate &value)
{
	employment_date = value;
}

void People::setRateCount(int value)
{
	rateCount = value;
}
