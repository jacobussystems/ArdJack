/*
	Table.h

	By Jim Davies
	Jacobus Systems, Brighton & Hove, UK
	http://www.jacobus.co.uk

	Provided under the MIT license: https://github.com/jacobussystems/ArdJack/blob/master/LICENSE
	Copyright (c) 2019 James Davies, Jacobus Systems, Brighton & Hove, UK

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
	documentation files (the "Software"), to deal in the Software without restriction, including without limitation
	the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions
	of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
	THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
	THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
	IN THE SOFTWARE.
*/

#pragma once

#ifdef ARDUINO
	#include <arduino.h>
#else
	#include "stdafx.h"
#endif

#include "Globals.h"


struct TableColumnDef
{
	uint8_t Alignment;														// enumeration ARDJACK_HORZ_ALIGN_CENTRE, etc.
	char Caption[ARDJACK_MAX_NAME_LENGTH];
	uint8_t Padding;														// padding (left and right)
	uint8_t Width;															// total width, including padding
};


// A 'pseudo Table', to aid formatting text output.

class Table
{
protected:
	virtual char* AddLeftMargin(char* text);

public:
	uint8_t ColumnCount;
	TableColumnDef* ColumnDefs[ARDJACK_MAX_TABLE_COLUMNS];
	char HorzLineChar;
	uint8_t LeftMargin;
	uint16_t TotalWidth;
	char VertLineChar;

	Table();
	~Table();

	virtual bool AddColumn(const char* caption, int width = 10, int align = ARDJACK_HORZ_ALIGN_LEFT, int padding = 1);
	virtual char* Cell(char* text, const char* colText, int col);
	virtual char* Header(char* text);
	virtual char* HorizontalLine(char* text);
	virtual char* Row(char* text, const char* col0 = "", const char* col1 = "", const char* col2 = "",
		const char* col3 = "", const char* col4 = "", const char* col5 = "", const char* col6 = "", const char* col7 = "",
		const char* col8 = "", const char* col9 = "", const char* col10 = "", const char* col11 = "");
};

