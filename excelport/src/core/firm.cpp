
#include <QVariantList>
#include <QVariantMap>

#include "firm.h"

Firm::Firm()
{

}

Firm::~Firm()
{
	qDeleteAll(sList);
}

QVariant Firm::toVariant() const
{
	QVariantMap map;

	QVariantList list;
	Q_FOREACH (Subdivision *s, sList)
	{
		list.append(s->toVariant());
	}

	map.insert("name", name);
	map.insert("subdivision_list", list);
	return map;
}

void Firm::setName(const QString &value)
{
	name = value;
}

void Firm::setSList(const QList<Subdivision *> &value)
{
	qDeleteAll(sList);
	sList = value;
}

void Firm::add(Subdivision *value)
{
	sList.append(value);
}

QVariantList Firm::toVariantList(const QList<Firm *> &flist)
{
	QVariantList res;

	Q_FOREACH (Firm* f, flist)
	{
		res.append(f->toVariant());
	}

	return res;
}

QString Firm::getName() const
{
	return name;
}
