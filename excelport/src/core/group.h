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

	bool addPeople(People *p);
	bool addGroup(Group *g);

	void clear();

	static QVariantList toVariantList(const QList<Group*> &groups);

	void setRateCount(int value);
	void addRateCount(int value);

private:
	int rateCount = 0;
	QString gTagName;
	QString name;
	QList<People*> peopleList;
	QList<Group*> groupList;
};
