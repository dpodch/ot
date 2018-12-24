
#include <QVariantMap>
#include <QVariantList>

#include "subdivision.h"

Subdivision::Subdivision()
{

}

Subdivision::~Subdivision()
{

}

QVariant Subdivision::toVariant() const
{
	QVariantList vacancyListVar;
	Q_FOREACH (Vacancy v, vacancyList)
	{
		vacancyListVar.append(v.toVariant());
	}

	QVariantMap map;
	map.insert("name", name);
	map.insert("vacancy_list", vacancyListVar);
	return map;
}

void Subdivision::setName(const QString &value)
{
	name = value;
}

void Subdivision::setVacancyList(const QList<Vacancy> &value)
{
	vacancyList = value;
}
