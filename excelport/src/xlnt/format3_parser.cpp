
#include <QDebug>
#include <QTime>

#include <math.h>

#include <xlnt/xlnt.hpp>
#include <utils/exceptions.hpp>

#include "format3_parser.h"

QMap<int, ExcelItem> Format3Parser::parse(const xlnt::worksheet &ws) const
{
	QTime time;
	time.start();

	qDebug() << Q_FUNC_INFO << "...";

	QMap<int, ExcelItem> result;
	QSet<int> rangeRows;
	double totalRates = -1;

	const int rateColumnNum = 3;
	const int dataColumnNum = 1;

	const xlnt::column_t rateColumn(rateColumnNum);
	const xlnt::column_t dataColumn(dataColumnNum);

	auto ranges = ws.merged_ranges();
	auto iter = ranges.begin();

	while (iter != ranges.end())
	{
		xlnt::range_reference r = (*iter);
		uint32_t rowNum = r.top_left().row();
		uint32_t fcNum = r.top_left().column().index;
		uint32_t lcNum = r.top_right().column().index;

		rangeRows.insert(rowNum);

		if ( (ws.has_cell(xlnt::cell_reference(rateColumn, rowNum)) == false)
			 || (rowNum != r.bottom_left().row()) )
		{
			iter++;
			continue;
		}

		xlnt::cell rateCell = ws.cell(rateColumn, rowNum);
		xlnt::cell dataCell = ws.cell(dataColumn, rowNum);
		if ( (rateCell.data_type() != xlnt::cell_type::number)
			 || (dataCell.data_type() != xlnt::cell_type::shared_string) )
		{
			iter++;
			continue;
		}

		ExcelItem item;
		if ( (fcNum == 1) && (lcNum == 2) )
		{
			const QString value = QString::fromStdString(dataCell.value<std::string>());
			if (value.simplified().toLower().contains(QObject::tr("итого")))
			{
				totalRates = round(rateCell.value<double>() * 100);
				qDebug() << rowNum << "Total rates [" << totalRates << "]";
			}
			else
			{
				item.type = LT_GROUP;
				item.gName = value;
				item.rateCount = round(rateCell.value<double>() * 100);
				result.insert(rowNum, item);
				qDebug() << rowNum << item.gName << "[" << item.rateCount << "]";
			}
		}
		iter++;
	}

	{
		auto iter = ws.begin();
		while (iter != ws.end())
		{
			xlnt::cell firstCell = (*iter).front();

			if (rangeRows.contains(firstCell.row()))
			{
				iter++;
				continue;
			}

			bool rateIsOk = false;
			bool groupIsOk = false;
			bool peopleIsOk = false;

			ExcelItem item;

			auto rowIter = (*iter).begin();
			while (rowIter != (*iter).end())
			{
				xlnt::cell cell = (*rowIter);

				if (cell.column().index == 1)
				{
					if (cell.data_type() == xlnt::cell_type::shared_string)
					{
						groupIsOk = true;
						item.gName = QString::fromStdString(cell.value<std::string>());
					}
				}
				else if (cell.column().index == 3)
				{
					if (cell.data_type() == xlnt::cell_type::number)
					{
						item.rateCount = round(cell.value<double>() * 100);
						if (item.rateCount >= 0)
						{
							rateIsOk = true;
						}
					}
				}
				else if (cell.column().index == 2)
				{
					if (cell.data_type() == xlnt::cell_type::shared_string)
					{
						peopleIsOk = true;
						item.pName = QString::fromStdString(cell.value<std::string>());
					}
				}
				else
				{
					break;
				}
				rowIter++;
			}

			if (rateIsOk && groupIsOk && peopleIsOk)
			{
				qDebug() << firstCell.row()
						 << (item.gName.isEmpty() ? item.pName : item.gName)
						 << "[" << item.rateCount << "]";
				result.insert(firstCell.row(), item);
			}
			iter++;
		}
	}

	if (totalRates >= 0)
	{
		Q_FOREACH (ExcelItem i, result.values())
		{
			if (!i.pName.isEmpty())
			{
				totalRates -= i.rateCount;
			}
		}

		if (totalRates != 0)
		{
			qWarning() << Q_FUNC_INFO << "incorrect total rates" << totalRates;
			result.clear();
		}
	}

	qDebug() << Q_FUNC_INFO << "Done" <<time.elapsed();

	return result;
}
