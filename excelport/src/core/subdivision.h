#pragma once

#include <core/vacancy.h>

class Subdivision
{
public:
	Subdivision();
	virtual ~Subdivision();

	QVariant toVariant() const;
	void setName(const QString &value);
	void setVacancyList(const QList<Vacancy> &value);

private:
	QString name;
	QList<Vacancy> vacancyList;
};
