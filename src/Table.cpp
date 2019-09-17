/*
	Table.cpp

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

#include "pch.h"

#ifdef ARDUINO
	#define _strlwr strlwr
#else
	#include "stdafx.h"
	#include <typeinfo>
#endif

#include "Globals.h"
#include "Log.h"
#include "Table.h"
#include "Utils.h"




Table::Table()
{
	ColumnCount = 0;
	HorzLineChar = '_';
	LeftMargin = 3;
	TotalWidth = 0;
	VertLineChar = '|';

	for (int i = 0; i < ARDJACK_MAX_TABLE_COLUMNS; i++)
		ColumnDefs[i] = NULL;
}


Table::~Table()
{
	for (int i = 0; i < ARDJACK_MAX_TABLE_COLUMNS; i++)
	{
		if (NULL != ColumnDefs[i])
		{
			delete ColumnDefs[i];
			ColumnDefs[i] = NULL;
		}
	}

	ColumnCount = 0;
}


bool Table::AddColumn(const char* caption, int width, int align, int padding)
{
	TableColumnDef* colDef = new TableColumnDef();
	ColumnDefs[ColumnCount++] = colDef;

	colDef->Alignment = align;
	strcpy(colDef->Caption, caption);
	colDef->Padding = padding;
	colDef->Width = width;

	TotalWidth += width;

	return true;
}


char* Table::AddLeftMargin(char* text)
{
	Utils::RepeatChar(text, ' ', LeftMargin);
	return text;
}


char* Table::Cell(char* text, const char* colText, int col)
{
	text[0] = NULL;

	if (col >= ColumnCount)
		return text;

	TableColumnDef* colDef = ColumnDefs[col];

	if ((NULL == colText) || (strlen(colText) == 0))
	{
		// There's no text.
		return Utils::RepeatChar(text, ' ', colDef->Width);
	}

	char format[22];
	char padding[82];
	char temp[102];

	// Left-side padding.
	Utils::RepeatChar(padding, ' ', colDef->Padding);
	strcat(text, padding);

	// Align the column text.
	int textWidth = colDef->Width - 2 * colDef->Padding;

	switch (colDef->Alignment)
	{
	case ARDJACK_HORZ_ALIGN_CENTRE:
		{
			int remainder = textWidth - (NULL != colText) ? strlen(colText) : 0;

			if (remainder <= 0)
				strcat(text, colText);
			else
			{
				int spaces = remainder / 2;
				Utils::RepeatChar(padding, ' ', spaces);
				strcat(text, padding);

				strcat(text, colText);

				Utils::RepeatChar(padding, ' ', remainder - spaces);
				strcat(text, padding);
			}
		}
		break;

	case ARDJACK_HORZ_ALIGN_LEFT:
		sprintf(format, "%%-%ds", textWidth);
		sprintf(temp, format, (NULL != colText) ? colText : "");
		strcat(text, temp);
		break;

	case ARDJACK_HORZ_ALIGN_RIGHT:
		sprintf(format, "%%%ds", textWidth);
		sprintf(temp, format, (NULL != colText) ? colText : "");
		strcat(text, temp);
		break;
	}

	if (col < ColumnCount - 1)
	{
		// Right-side padding.
		Utils::RepeatChar(padding, ' ', colDef->Padding);
		strcat(text, padding);
	}

	return text;
}


char* Table::Header(char* text)
{
	text[0] = NULL;
	AddLeftMargin(text);

	char temp[82];

	for (int col = 0; col < ColumnCount; col++)
	{
		TableColumnDef* colDef = ColumnDefs[col];
		Cell(temp, colDef->Caption, col);
		strcat(text, temp);
	}

	return text;
}


char* Table::HorizontalLine(char* text)
{
	text[0] = NULL;

	AddLeftMargin(text);

	// A horizontal line.
	char temp[202];
	Utils::RepeatChar(temp, HorzLineChar, TotalWidth);

	strcat(text, temp);

	return text;
}


char* Table::Row(char* text, const char* col0, const char* col1, const char* col2,
	const char* col3, const char* col4, const char* col5, const char* col6, const char* col7,
	const char* col8, const char* col9, const char* col10, const char* col11)
{
	text[0] = NULL;
	AddLeftMargin(text);

	if (strlen(col0) == 0) return text;

	char work[102];
	int col = 0;

	strcat(text, Cell(work, col0, col++));
	strcat(text, Cell(work, col1, col++));
	strcat(text, Cell(work, col2, col++));
	strcat(text, Cell(work, col3, col++));
	strcat(text, Cell(work, col4, col++));
	strcat(text, Cell(work, col5, col++));
	strcat(text, Cell(work, col6, col++));
	strcat(text, Cell(work, col7, col++));
	strcat(text, Cell(work, col8, col++));
	strcat(text, Cell(work, col9, col++));
	strcat(text, Cell(work, col10, col++));
	strcat(text, Cell(work, col11, col++));

	return text;
}

