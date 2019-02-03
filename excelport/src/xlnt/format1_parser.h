#pragma once

#include <xlnt/i_format_parser.h>

class Format1Parser :public IFormatParser
{
public:
	Format1Parser() {}
	virtual ~Format1Parser() {}

	virtual QMap<int, ExcelItem> parse(const xlnt::worksheet &ws) const override final;

};
