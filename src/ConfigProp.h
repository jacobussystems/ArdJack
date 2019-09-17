/*
	ConfigProp.h

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
	#include "DetectBoard.h"
#else
	#include "stdafx.h"
#endif

#include <stdint.h>

#include "Globals.h"

class StringList;



class ConfigProp
{
protected:
	StringList* _Strings;													// [0] Name, [1] Description, [2] Unit, [3] Value

public:
	uint8_t DataType;														// enumeration ARDJACK_DATATYPE_BOOLEAN etc.
	//ConfigProp* Parent;
	bool Value_Boolean;
	long Value_Integer;
	double Value_Real;

	ConfigProp();
	~ConfigProp();

	virtual const char* Description();
	virtual bool GetAsBoolean(bool* value);
	virtual bool GetAsInteger(int* value);
	virtual bool GetAsReal(double* value);
	virtual bool GetAsString(char* value);
	virtual const char* Name();
	//virtual char* Path(char* out);
	virtual bool SetDescription(const char* value);
	virtual bool SetFromString(const char* value);
	virtual bool SetName(const char* value);
	virtual bool SetUnit(const char* value);
	virtual const char* StringValue();
	virtual const char* Unit();
};

