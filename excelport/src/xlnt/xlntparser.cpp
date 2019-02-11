#include <xlnt/xlnt.hpp>
#include <utils/exceptions.hpp>
#include <iostream>
#include <sstream>
#include <QFile>
#include <QDebug>
#include <QStringList>

#include <xlnt/format1_parser.h>
#include <xlnt/format2_parser.h>

#include "xlntparser.h"


XlntParser::XlntParser()
{
	parserList.append(new Format1Parser);
	parserList.append(new Format2Parser);
}

XlntParser::~XlntParser()
{
	qDeleteAll(parserList);
}

bool decrement(QList< QPair<Group*, double> > *groups, double value)
{
	for (int i = 0; i < groups->size(); i++)
	{
		auto g = groups->at(i);
		g.second = g.second - value;
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

	QStringList fio = QString(str).split(" ");
	if (fio.size() > 1)
	{
		p = new People;
		p->setSurname(fio.value(0));
		p->setName(fio.value(1));
		p->setPatronymic(fio.value(2));
	}

	QStringList fields = QString(str).split(",");
	if (fields.size() >= 2)
	{
		p->setBirthday(QDate::fromString(fields.value(1).simplified(),
										 "dd.MM.yyyy"));
	}

	if (fields.size() >= 3)
	{
		p->setEmployment_date(QDate::fromString(fields.value(2).simplified(),
												"dd.MM.yyyy"));
	}

	return p;
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
			IFormatParser *parser = parserList.at(k);

			bool parserOk = true;
			QString parserErrMessage;

			QMap<int, ExcelItem> itemMap = parser->parse(ws);
			QList<int> rows = itemMap.keys();
			qSort(rows);

			QList<QPair<Group*, double> > tmpGroup;
			for (int i = 0; i < rows.size(); i++)
			{
				int rowNum = rows.at(i);
				ExcelItem item = itemMap.value(rowNum);

				if (tmpGroup.isEmpty())
				{
					if (item.gName.isEmpty() == true)
					{
						parserOk = false;
						parserErrMessage.append(
									"The first line should contain the name of the firm");
						break;
					}

					Group* g = new Group;
					g->setName(item.gName);
					tmpGroup.append(QPair<Group*, double>(g, item.rateCount));
					res.append(g);
					continue;
				}

				if (tmpGroup.isEmpty())
				{
					parserOk = false;
					parserErrMessage.append("Bad excel format");
					break;
				}

				const auto &cGroup = tmpGroup.last();

				double rateCount = 0.;

				if (item.gName.isEmpty() == false)
				{
					Group *g = cGroup.first->findGroup(item.gName);
					if (g == NULL)
					{
						g = new Group;
						g->setName(item.gName);
						cGroup.first->addGroup(g);
						tmpGroup.append(QPair<Group*, double>(g, item.rateCount));
					}

					if (item.pName.isEmpty() == false)
					{
						People *p = createPeople(item.pName);
						if (p == NULL)
						{
							parserOk = false;
							parserErrMessage.append(
										QString("Error format FIO:") + item.pName);
							break;
						}
						else
						{
							g->addPeople(p);
							rateCount = item.rateCount;
						}
					}
				}
				else if (item.pName.isEmpty() == false)
				{

					People *p = createPeople(item.pName);
					if (p == NULL)
					{
						parserOk = false;
						parserErrMessage.append(
									QString("Error format FIO:") + item.pName);
						break;
					}
					else
					{
						cGroup.first->addPeople(p);
						rateCount = item.rateCount;
					}
				}

				if (decrement(&tmpGroup, rateCount) == false)
				{
					parserErrMessage.append("Error excel format");
					parserOk = false;
					break;
				}
			}

			if (tmpGroup.isEmpty() == false)
			{
				parserErrMessage.append("Error excel format");
				parserOk = false;
			}

			if (rows.size() == 0)
			{
				parserErrMessage.append("Rows size is empty");
				parserOk = false;
			}
			*isOk = parserOk;

			if (parserOk == true)
			{
				break;
			}
			else
			{
				qDeleteAll(res);
				res.clear();
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
		qCritical() << Q_FUNC_INFO << ":" << *errMsg;
	}

	return res;
}
