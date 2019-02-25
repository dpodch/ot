#pragma once

#include <xlnt/i_format_parser.h>

class Format3Parser :public IFormatParser
{
public:
	Format3Parser() {}
	virtual ~Format3Parser() {}

	virtual QMap<int, ExcelItem> parse(const xlnt::worksheet &ws) const override final;

};
