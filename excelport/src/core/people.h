#pragma once

#include <QString>
#include <QDateTime>

class People
{
public:
	People();
	virtual ~People();

	QVariant toVariant() const;

	void setName(const QString &value);
	void setPatronymic(const QString &value);
	void setSurname(const QString &value);
	void setBirthday(const QDate &value);
	void setEmployment_date(const QDate &value);
	void setRateCount(int value);

private:
	int rateCount = 0;
	QString name;
	QString patronymic;
	QString surname;
	QDate birthday;
	QDate employment_date;

};
