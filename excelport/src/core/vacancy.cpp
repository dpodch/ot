
#include <QVariant>
#include <QVariantMap>
#include <QVariantList>

#include "vacancy.h"

Vacancy::Vacancy()
{

}

Vacancy::~Vacancy()
{
}

QVariant Vacancy::toVariant() const
{
	QVariantMap map;

	QVariantList pList;
	Q_FOREACH (People p, peopleList)
	{
		pList.append(p.toVariant());
	}

	map.insert("people_list", pList);
	map.insert("name", name);

	return map;
}

void Vacancy::setName(const QString &value)
{
	name = value;
}

void Vacancy::setPeopleList(const QList<People> &value)
{
	peopleList = value;
}
