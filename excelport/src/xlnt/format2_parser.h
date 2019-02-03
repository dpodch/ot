#pragma once

#include <xlnt/i_format_parser.h>

class Format2Parser :public IFormatParser
{
public:
	Format2Parser() {}
	virtual ~Format2Parser() {}

	virtual QMap<int, ExcelItem> parse(const xlnt::worksheet &ws) const override final;

};
