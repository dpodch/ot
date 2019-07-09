
#include <QDebug>
#include <QTime>

#include <math.h>

#include <QTextCodec>
#include <xlnt/xlnt.hpp>
#include <utils/exceptions.hpp>

#include "format2_parser.h"

QMap<int, ExcelItem> Format2Parser::parse(const xlnt::worksheet &ws) const
{
	QTime time;
	time.start();

	qDebug() << Q_FUNC_INFO << "...";

	QMap<int, ExcelItem> result;

	auto ranges = ws.merged_ranges();
	auto iter = ranges.begin();

	while (iter != ranges.end())
	{
		xlnt::range_reference r = (*iter);
		uint32_t rowNum = r.top_left().row();
		uint32_t fcNum = r.top_left().column().index;
		uint32_t lcNum = r.top_right().column().index;
		bool hasfcCell = ws.has_cell(xlnt::cell_reference(xlnt::column_t(fcNum), rowNum));

		if (hasfcCell == false)
		{
			iter++;
			continue;
		}

		auto cell = ws.cell(xlnt::column_t(fcNum), rowNum);

		if ( (fcNum == 3)
				&& ( (lcNum == 12) || (lcNum == 10) )
				&& (cell.data_type() == xlnt::cell_type::shared_string)
				&& (cell.font().bold() == true) )
		{
			ExcelItem item;
			std::string str = cell.value<std::string>();
			item.gName = QString::fromStdString(str);
			item.rateCount = -1;

			result.insert(rowNum, item);

			iter++;
			continue;
		}

		if ( (ws.has_cell(xlnt::cell_reference(xlnt::column_t(7), rowNum)) == false)
			 || (ws.cell(xlnt::column_t(7), rowNum).data_type() != xlnt::cell_type::number)
			 || (rowNum != r.bottom_left().row()) )
		{
			iter++;
			continue;
		}

		auto rateCell = ws.cell(xlnt::column_t(7), rowNum);

		if ( (fcNum == 1)
				&& (lcNum == 6)
				&& (cell.data_type() == xlnt::cell_type::shared_string) )
		{
			r = (*(++iter));
			if ( (r.top_left().row() == r.top_right().row())
				 && (r.top_left().row() == rowNum)
				 && (r.top_left().column().index == 7)
				 && (r.top_right().column().index == 8) )
			{
				std::string str = cell.value<std::string>();
				QString value = QString::fromStdString(str);

				ExcelItem item;
				if (cell.font().bold() == true)
				{
					item.gName = value;
				}
				else
				{
					item.pName = value;
				}

				item.rateCount = round(rateCell.value<double>() * 100);
				result.insert(rowNum, item);

				qDebug() << rowNum << (item.gName.isEmpty() ? item.pName : item.gName)
												<< "[" << item.rateCount << "]";
			}
			else
			{
				qCritical() << Q_FUNC_INFO << "Error parse excel format";
			}
		}

		if ( (fcNum == 1)
				&& (lcNum == 4)
				&& (cell.data_type() == xlnt::cell_type::shared_string)
				&& (cell.font().bold() == true) )
		{
			r = (*(++iter));
			if ( (r.top_left().row() == r.top_right().row())
				 && (r.top_left().row() == rowNum)
				 && (r.top_left().column().index == 7)
				 && (r.top_right().column().index == 8) )
			{
				QString value = QString::fromStdString(cell.value<std::string>());

				ExcelItem item;
				item.gName = value;
				if (item.gName.isEmpty())
				{
					qDebug() <<"!!";
				}
				item.rateCount = round(rateCell.value<double>() * 100);
				result.insert(rowNum, item);

//				qDebug() << rowNum << item.gName << "[" << item.rateCount << "]";
			}
			else
			{
				qCritical() << Q_FUNC_INFO << "Error parse excel format";
			}
		}
		iter++;
	}

	QList<int> rows = result.keys();

	int row = -1;
	ExcelItem item;

	for (int i = 0; i < rows.size(); i++)
	{
		ExcelItem t = result.value(rows.at(i));
		if (t.rateCount < 0)
		{
			if (row != -1)
			{
				result.insert(row, item);
			}
			row = rows.at(i);
			item = t;
			item.rateCount = 0;
		}
		else if (t.pName.isEmpty() == false)
		{
//			qDebug () << "rate"
//					  << "\nto" << item.gName << item.rateCount
//					  << "\nfrom"<< t.gName << t.pName << t.rateCount;
			item.rateCount += t.rateCount;
		}
	}

	if (row != -1)
	{
		result.insert(row, item);
	}

	qDebug() << Q_FUNC_INFO << "Done" << time.elapsed();

	return result;
}


