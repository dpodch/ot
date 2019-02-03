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

				Group *g = NULL;
				People *p = NULL;
				double rateCount = 0.;

				if (item.gName.isEmpty() == false)
				{
					g = new Group;
					g->setName(item.gName);
					cGroup.first->addGroup(g);
					tmpGroup.append(QPair<Group*, double>(g, item.rateCount));

					if (item.pName.isEmpty() == false)
					{
						p = createPeople(item.pName);
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

					p = createPeople(item.pName);
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
				parserErrMessage.append("Error excel format");
				parserOk = false;
			}

			if (parserOk == true)
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
//	catch(...)
//	{
//		*isOk = false;
//		errMsg->append("Unknown error");
//	}

	if (*isOk == false)
	{
		qCritical() << Q_FUNC_INFO << ":" << *errMsg;
	}

	return res;
}

//std::vector<xlnt::range_reference> thinOut(const std::vector<xlnt::range_reference> &vector)
//{
//	QSet<int> rowProc;

//	std::vector<xlnt::range_reference> res;

//	for (uint i = 0; i < vector.size(); i++)
//	{
//		xlnt::range_reference r = vector.at(i);
//		int bot = r.bottom_left().row();
//		int top = r.top_right().row();

//		int right = r.bottom_right().column().index;
//		int left = r.bottom_left().column().index;

//		int prevRight = -1;;
//		int prevLeft = -1;

//		if (!res.empty())
//		{
//			prevRight = res.back().bottom_right().column().index;
//			prevLeft = res.back().bottom_left().column().index;
//		}

//		if (top != bot)
//		{
//			res.clear();
//			for (int i = top; i <= bot; i++)
//			{
//				rowProc.insert(i);
//			}
//		}
//		else if (rowProc.contains(top))
//		{
//			res.clear();
//		}
//		else if ( ( (prevLeft == 1 && prevRight == 7)
//					|| (prevLeft == 5 && prevRight == 7) )
//				 && (left == 1 && right == 4) )
//		{
//			res.push_back(r);
//		}
//		else if (prevLeft == 1 && prevRight == 4 && left == 5 && right == 7)
//		{
//			res.push_back(r);
//		}
//		else if (left == 1 && right == 7)
//		{
//			res.push_back(r);
//		}
//		else
//		{
//			res.clear();
//		}
//	}

//	return res;
//}

//QList<Group *> XlntParser::parse(const QByteArray &ba, bool *isOk, QString *errMsg) const
//{
//	QList<Group*> res;
//	try
//	{
//		std::istringstream in(std::string(ba.data(), ba.size()));
//		xlnt::workbook excelFile;
//		excelFile.load(in);

//		const xlnt::worksheet ws = excelFile.sheet_by_index(0);

//		std::vector<xlnt::range_reference> range = thinOut(ws.merged_ranges());

//		Group* cfirm = NULL;;
//		Group* cSub = NULL;

//		Q_FOREACH (xlnt::range_reference r, range)
//		{
//			xlnt::cell cell = ws.cell(r.bottom_left().column(), r.bottom_left().row());
//			bool isSubvision = (cell.font().bold()) && (r.width() == 7);
//			if (isSubvision == true)
//			{
//				if (cSub != NULL)
//				{
//					cfirm->addGroup(cSub);
//				}
//				cSub = new Group();
//				cSub->setName(QString(cell.value<std::string>().c_str()));
//			}
//			else if (r.width() == 7)
//			{
//				if (cfirm != NULL)
//				{
//					res.append(cfirm);
//				}
//				cfirm = new Group();
//				cfirm->setName(QString(cell.value<std::string>().c_str()));
//			}
//			else if (r.width() == 4)
//			{
//				QString vacancy = QString(ws.cell(xlnt::column_t(r.bottom_left().column().index + 4),
//												  r.bottom_left().row())
//										  .value<std::string>().c_str());

//				Group* v = cSub->findGroup(vacancy);
//				if (v == NULL)
//				{
//					v = new Group;
//					v->setName(vacancy);
//					cSub->addGroup(v);
//				}

//				QStringList fio = QString(cell.value<std::string>().c_str()).split(" ");
//				if (fio.size() >= 2)
//				{
//					People* p = new People;
//					p->setName(fio.value(1));
//					p->setSurname(fio.value(0));
//					p->setPatronymic(fio.value(2));
//					v->addPeople(p);
//				}
//			}
//		}

//		if (cSub != NULL)
//		{
//			cfirm->addGroup(cSub);
//		}

//		if (cfirm != NULL)
//		{
//			res.append(cfirm);
//		}
//	}
//	catch(xlnt::exception &e)
//	{
//		*isOk = false;
//		errMsg->append(QString(e.what()));
//	}
//	catch(...)
//	{
//		*isOk = false;
//		errMsg->append("Unknown error");
//	}

//	if (*isOk == false)
//	{
//		qCritical() << Q_FUNC_INFO << ":" << *errMsg;
//	}

//	return res;
//}
