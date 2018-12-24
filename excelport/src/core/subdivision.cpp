
#include <QVariantMap>
#include <QVariantList>

#include "subdivision.h"

Subdivision::Subdivision()
{

}

Subdivision::~Subdivision()
{
	qDeleteAll(vacancyList);
}

QVariant Subdivision::toVariant() const
{
	QVariantList vacancyListVar;
	Q_FOREACH (Vacancy* v, vacancyList)
	{
		vacancyListVar.append(v->toVariant());
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

void Subdivision::setVacancyList(const QList<Vacancy*> &value)
{
	qDeleteAll(vacancyList);
	vacancyList = value;
}

Vacancy* Subdivision::getVacancy(const QString &value) const
{
	Vacancy* res = NULL;
	Q_FOREACH (Vacancy*v , vacancyList)
	{
		if (v->getName() == value)
		{
			res = v;
			break;
		}
	}
	return res;
}

void Subdivision::add(Vacancy *value)
{
	vacancyList.append(value);
}

QString Subdivision::getName() const
{
	return name;
}
