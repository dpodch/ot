
#include <QDebug>

#include <xlnt/xlnt.hpp>
#include <utils/exceptions.hpp>

#include "format1_parser.h"

QMap<int, ExcelItem> Format1Parser::parse(const xlnt::worksheet &ws) const
{
	qDebug() << Q_FUNC_INFO << "...";

	QMap<int, ExcelItem> result;

	const int rateColumnNum = 8;
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
		if ( (fcNum == 1) && (lcNum == 7) )
		{
			item.type = LT_GROUP;
			item.gName = QString::fromStdString(dataCell.value<std::string>());
			item.rateCount = round(rateCell.value<double>() * 100);
			result.insert(rowNum, item);
			qDebug() << rowNum << item.gName << "[" << item.rateCount << "]";
		}
		else if ( (fcNum == 1) && (lcNum == 4) )
		{
			r = (*(++iter));
			if ( (r.top_left().row() == r.top_right().row())
				 && (r.top_left().column().index == 5)
				 && (r.top_right().column().index == 7)
				 && (ws.cell(r.top_left().column(), rowNum).data_type()
					 == xlnt::cell_type::shared_string) )
			{
				auto gCell = ws.cell(r.top_left().column(), rowNum);
				item.pName = QString::fromStdString(dataCell.value<std::string>());
				item.gName = QString::fromStdString(gCell.value<std::string>());
				item.rateCount = round(rateCell.value<double>() * 100);
				qDebug() << rowNum << item.gName
						 << "--->" << item.pName
						 << "[" << item.rateCount << "]";
				result.insert(rowNum, item);
			}
		}
		iter++;
	}

	qDebug() << Q_FUNC_INFO << "Done";

	return result;
}
