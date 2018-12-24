#pragma once

#include <core/vacancy.h>

class Subdivision
{
public:
	Subdivision();
	virtual ~Subdivision();

	QVariant toVariant() const;
	void setName(const QString &value);
	void setVacancyList(const QList<Vacancy*> &value);

	Vacancy *getVacancy(const QString &value) const;

	void add(Vacancy *value);

	QString getName() const;

private:
	QString name;
	QList<Vacancy*> vacancyList;
};
