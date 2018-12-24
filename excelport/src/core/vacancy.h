#pragma once

#include <QList>

#include <core/people.h>

class Vacancy
{
public:
	Vacancy();
	virtual ~Vacancy();

	QVariant toVariant() const;
	void setName(const QString &value);
	void setPeopleList(const QList<People> &value);

private:
	QList<People> peopleList;
	QString name;
};
