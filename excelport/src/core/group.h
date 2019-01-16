#pragma once

#include <QVariantList>

#include <core/people.h>

class Group
{
public:
	Group();
	virtual ~Group();

	QVariant toVariant() const;
	void setName(const QString &value);
	QString getName() const;

	QList<People *> getPeopleList() const;
	void setPeopleList(const QList<People *> &value);

	QList<Group *> getGroupList() const;
	void setGroupList(const QList<Group *> &value);

	Group* findGroup(const QString &name) const;

	void addPeople(People *p);
	void addGroup(Group *g);

	void clear();

	static QVariantList toVariantList(const QList<Group*> &groups);

private:
	QString name;
	QList<People*> peopleList;
	QList<Group*> groupList;
};
