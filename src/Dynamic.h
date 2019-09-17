/*
	Dynamic.h

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

#include "Globals.h"

class Filter;
class DateTime;



class Dynamic
{
protected:
	union
	{
		bool _BoolVal;
		//DateTime _DateTimeVal;
		double _DoubleVal;
		int _IntVal;
		char _StringVal[ARDJACK_MAX_DYNAMIC_STRING_LENGTH];
	};

	uint8_t _DataType;
	//int _DataType;

public:
	Dynamic();
	~Dynamic();

	bool AsBool();
	//DateTime AsDateTime();
	double AsDouble();
	int AsInt();
	char* AsString(char* value);
	void Clear();
	bool Copy(Dynamic* src);
	int DataType();
	bool Equals(Dynamic* src, bool ignoreCase = false);
	bool IsEmpty();
	bool SetBool(bool value);
	//bool SetDateTime(DateTime value);
	bool SetDouble(double value);
	bool SetInt(int value);
	bool SetString(const char* value);
	const char* String();
	const char* ToString(char* text);
	bool ValuesDiffer(Dynamic* src, bool ignoreCase = false, Filter* filter = NULL, bool lastChangeState = false,
		long lastChangeMs = 0, long lastNotifiedMs = 0);
};

