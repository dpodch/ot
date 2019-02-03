
#include <QVariantMap>
#include <QVariantList>
#include <QDebug>

#include "group.h"

Group::Group()
{

}

Group::~Group()
{
	clear();
}

QVariant Group::toVariant() const
{
	if (peopleList.size() > 0 && groupList.size() > 0)
	{
		qCritical() << Q_FUNC_INFO << "ERROR Group contains people and group";
	}

	QVariantMap map;
	map.insert("name", name);

	QVariantList plistVar;
	QVariantList glistVar;

	Q_FOREACH (People *p, peopleList)
	{
		plistVar.append(p->toVariant());
	}

	if (plistVar.isEmpty())
	{
		Q_FOREACH (Group *g, groupList)
		{
			glistVar.append(g->toVariant());
		}
	}

	map.insert("name", name);
	if (!plistVar.isEmpty())
	{
		map.insert("position_list", plistVar);
	}
	else if (!glistVar.isEmpty())
	{
		map.insert("division_list", glistVar);
	}

	return map;
}

void Group::setName(const QString &value)
{
	name = value;
}

QString Group::getName() const
{
	return name;
}

QList<People *> Group::getPeopleList() const
{
	return peopleList;
}

void Group::setPeopleList(const QList<People *> &value)
{
	peopleList = value;
}

QList<Group *> Group::getGroupList() const
{
	return groupList;
}

void Group::setGroupList(const QList<Group *> &value)
{
	groupList = value;
}

Group *Group::findGroup(const QString &name) const
{
	Group* res = NULL;
	Q_FOREACH (Group *g, groupList)
	{
		if (g->getName() == name)
		{
			res = g;
			break;
		}
	}
	return res;
}

void Group::addPeople(People *p)
{
	Q_ASSERT (p != NULL);
	peopleList.append(p);
}

void Group::addGroup(Group *g)
{
	Q_ASSERT (g != NULL);
	groupList.append(g);
}

void Group::clear()
{
	qDeleteAll(peopleList);
	qDeleteAll(groupList);
	peopleList.clear();
	groupList.clear();
	name.clear();
}

QVariantList Group::toVariantList(const QList<Group *> &groups)
{
	QVariantList res;

	Q_FOREACH (Group* g, groups)
	{
		res.append(g->toVariant());
	}

	return res;
}
