#include <xlnt/xlnt.hpp>
#include <utils/exceptions.hpp>
#include <iostream>
#include <sstream>
#include <QFile>
#include <QDebug>
#include <QStringList>

#include "xlntparser.h"

XlntParser::XlntParser()
{

}

XlntParser::~XlntParser()
{

}

std::vector<xlnt::range_reference> thinOut(const std::vector<xlnt::range_reference> &vector)
{
	QSet<int> rowProc;

	std::vector<xlnt::range_reference> res;

	Q_FOREACH (xlnt::range_reference r, vector)
	{
		int bot = r.bottom_left().row();
		int top = r.top_right().row();

		int right = r.bottom_right().column().index;
		int left = r.bottom_left().column().index;

		int prevRight = -1;;
		int prevLeft = -1;

		if (!res.empty())
		{
			prevRight = res.back().bottom_right().column().index;
			prevLeft = res.back().bottom_left().column().index;
		}

		if (top != bot)
		{
			res.clear();
			for (int i = top; i <= bot; i++)
			{
				rowProc.insert(i);
			}
		}
		else if (rowProc.contains(top))
		{
			res.clear();
		}
		else if ( ( (prevLeft == 1 && prevRight == 7)
					|| (prevLeft == 5 && prevRight == 7) )
				 && (left == 1 && right == 4) )
		{
			res.push_back(r);
		}
		else if (prevLeft == 1 && prevRight == 4 && left == 5 && right == 7)
		{
			res.push_back(r);
		}
		else if (left == 1 && right == 7)
		{
			res.push_back(r);
		}
		else
		{
			res.clear();
		}
	}

	return res;
}

QList<Firm*> XlntParser::parse(const QByteArray &ba, bool *isOk, QString *errMsg) const
{
	QList<Firm*> res;
	try
	{
		std::istringstream in(std::string(ba.data(), ba.size()));
		xlnt::workbook excelFile;
		excelFile.load(in);

		const xlnt::worksheet ws = excelFile.sheet_by_index(0);

		std::vector<xlnt::range_reference> range = thinOut(ws.merged_ranges());

		Firm* cfirm = NULL;;
		Subdivision* cSub = NULL;

		Q_FOREACH (xlnt::range_reference r, range)
		{
			xlnt::cell cell = ws.cell(r.bottom_left().column(), r.bottom_left().row());
			bool isSubvision = (cell.font().bold()) && (r.width() == 7);
			if (isSubvision == true)
			{
				if (cSub != NULL)
				{
					cfirm->add(cSub);
				}
				cSub = new Subdivision();
				cSub->setName(QString(cell.value<std::string>().c_str()));
			}
			else if (r.width() == 7)
			{
				if (cfirm != NULL)
				{
					res.append(cfirm);
				}
				cfirm = new Firm();
				cfirm->setName(QString(cell.value<std::string>().c_str()));
			}
			else if (r.width() == 4)
			{
				QString vacancy = QString(ws.cell(xlnt::column_t(r.bottom_left().column().index + 4),
												  r.bottom_left().row())
										  .value<std::string>().c_str());

				Vacancy* v = cSub->getVacancy(vacancy);
				if (v == NULL)
				{
					v = new Vacancy;
					v->setName(vacancy);
					cSub->add(v);
				}

				QStringList fio = QString(cell.value<std::string>().c_str()).split(" ");
				if (fio.size() >= 2)
				{
					People p;
					p.setName(fio.value(1));
					p.setSurname(fio.value(0));
					p.setPatronymic(fio.value(2));
					v->add(p);
				}
			}
		}

		if (cSub != NULL)
		{
			cfirm->add(cSub);
		}

		if (cfirm != NULL)
		{
			res.append(cfirm);
		}

		std::string cellValue = ws.cell("A1").value<std::string>();
		qDebug() << "First cell = " << QString(cellValue.c_str());
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
