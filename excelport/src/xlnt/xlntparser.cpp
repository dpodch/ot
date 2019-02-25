#include <xlnt/xlnt.hpp>
#include <utils/exceptions.hpp>
#include <iostream>
#include <sstream>
#include <QFile>
#include <QDebug>
#include <QStringList>

#include <xlnt/format1_parser.h>
#include <xlnt/format2_parser.h>
#include <xlnt/format3_parser.h>

#include "xlntparser.h"


XlntParser::XlntParser()
{
	parserList.append(new Format3Parser);
	parserList.append(new Format2Parser);
	parserList.append(new Format1Parser);
}

XlntParser::~XlntParser()
{
	qDeleteAll(parserList);
}

bool decrement(QList< QPair<Group*, int> > *groups, int value)
{
	for (int i = 0; i < groups->size(); i++)
	{
		auto g = groups->value(i);

//		qDebug() << "before" << g.first->getName() << g.second << value;
		g.second = g.second - value;
//		qDebug() << "after" << g.first->getName() << g.second << value;
		groups->replace(i, g);
	}

	while ( (groups->size() > 0) && (groups->last().second <= 0) )
	{
		if (groups->last().second < 0)
		{
			return false;
		}
		groups->takeLast();
	}

	return true;
}

People* XlntParser::createPeople(const QString &str) const
{
	People* p = NULL;
	if (str.size() < 5)
	{
		p = new People;
		return p;
	}

	QStringList fields = QString(str).split(",");

	if (fields.size() >= 1)
	{
		QStringList fio = QString(fields.first()).split(" ");
		if (fio.size() > 1)
		{
			p = new People;
			p->setSurname(fio.value(0));
			p->setName(fio.value(1));
			p->setPatronymic(fio.value(2));
		}
	}

	QDate dt1;
	QDate dt2;

	if (fields.size() >= 2)
	{
		dt1 = QDate::fromString(fields.value(1).simplified(), "dd.MM.yyyy");
	}

	if (fields.size() >= 3)
	{
		dt2 = QDate::fromString(fields.value(2).simplified(), "dd.MM.yyyy");
	}

	if (!dt1.isNull() && !dt2.isNull())
	{
		p->setBirthday(dt1 < dt2 ? dt1 : dt2);
		p->setEmployment_date(dt1 < dt2 ? dt2 : dt1);
	}

	return p;
}

void XlntParser::addPeopleToGroup(const QString &str, int rateCount,
		Group *g, bool &isOk) const
{
	People *p = createPeople(str);
	if (p == NULL)
	{
		qWarning() << "Error parse FIO" + str;
		isOk = false;
	}
	else
	{
		p->setRateCount(rateCount);
		if (g->addPeople(p) == false)
		{
			isOk = false;
			qWarning() << QString("Error parse people to group:") + str;
			delete p;
			p = NULL;
		}
	}
}

QList<Group*> XlntParser::parse(IFormatParser *parser, const xlnt::worksheet &ws) const
{
	QList<Group*> res;

	bool isOk = true;

	QMap<int, ExcelItem> itemMap = parser->parse(ws);
	QList<int> rows = itemMap.keys();
	qSort(rows);

	QList<QPair<Group*, int> > tmpGroup;
	for (int i = 0; i < rows.size(); i++)
	{
		int rowNum = rows.at(i);
		ExcelItem item = itemMap.value(rowNum);

		if (tmpGroup.isEmpty())
		{
			if (item.gName.isEmpty() == true)
			{
				isOk = false;
				qWarning() << "The first line should contain the name of the firm";
				break;
			}

			Group* g = new Group;
			g->setName(item.gName);
			g->setRateCount(item.rateCount);
			tmpGroup.append(QPair<Group*, int>(g, item.rateCount));
			res.append(g);
			continue;
		}

		if (tmpGroup.isEmpty())
		{
			isOk = false;
			qWarning() << "Bad excel format";
			break;
		}

		const auto &cGroup = tmpGroup.last();

		int rateCount = 0.;

		if (item.gName.isEmpty() == false)
		{
			Group *g = cGroup.first->findGroup(item.gName);
			if (g == NULL)
			{
				g = new Group;
				g->setRateCount(item.rateCount);
				g->setName(item.gName);
				cGroup.first->addGroup(g);
				tmpGroup.append(QPair<Group*, int>(g, item.rateCount));
			}
			else
			{
				tmpGroup.append(QPair<Group*, int>(g, item.rateCount));
				g->addRateCount(item.rateCount);
			}

			if (item.pName.isEmpty() == false)
			{
				addPeopleToGroup(item.pName, item.rateCount,
								 g, isOk);
				if (isOk == true)
				{
					rateCount = item.rateCount;
				}
			}
		}
		else if (item.pName.isEmpty() == false)
		{
			addPeopleToGroup(item.pName, item.rateCount,
							 cGroup.first, isOk);
			if (isOk == true)
			{
				rateCount = item.rateCount;
			}
		}
		else
		{
			isOk = false;
			qWarning() << "pepope and group name is empty";
			break;
		}

		if (decrement(&tmpGroup, rateCount) == false)
		{
			qWarning() << "Error decrement"
						<< "gName" << item.gName
						<< "pName" << item.pName
						<< "rateCount" << rateCount;
			isOk = false;
			break;
		}
	}

	if (tmpGroup.isEmpty() == false)
	{
		qCritical() << "Group is empty";
		isOk = false;
	}

	if (rows.size() == 0)
	{
		qWarning() << "Rows is empty Use next parser";
		isOk = false;
	}

	if (isOk == false)
	{
		qDeleteAll(res);
		res.clear();
	}
	return res;
}

QList<Group*> XlntParser::parse(const QByteArray &ba, bool *isOk, QString *errMsg) const
{
	QList<Group*> res;
	try
	{
		std::istringstream in(std::string(ba.data(), ba.size()));
		xlnt::workbook excelFile;
		excelFile.load(in);
		const xlnt::worksheet ws = excelFile.sheet_by_index(0);

		for (int k = 0; k < parserList.size(); k++)
		{
			res = parse(parserList.at(k), ws);
			*isOk = !res.isEmpty();
			if (*isOk == true)
			{
				break;
			}
		}
	}
	catch(xlnt::exception &e)
	{
		*isOk = false;
		errMsg->append(QString(e.what()));
	}
	catch(...)
	{
		*isOk = false;
		errMsg->append("Unknown error");
	}

	if (*isOk == false)
	{
		errMsg->append("Error parse file");
		qCritical() << Q_FUNC_INFO << ":" << *errMsg;
	}

	return res;
}
