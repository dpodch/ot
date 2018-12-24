#pragma once

#include <QVariantList>

#include <core/subdivision.h>

class Firm
{
public:
	Firm();
	virtual ~Firm();

	QVariant toVariant() const;
	void setName(const QString &value);
	void setSList(const QList<Subdivision*> &value);
	void add(Subdivision* value);

	static QVariantList toVariantList(const QList<Firm*> &flist);

	QString getName() const;

private:
	QString name;
	QList<Subdivision*> sList;
};
