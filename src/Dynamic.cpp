/*
	Dynamic.cpp

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


#include "DateTime.h"
#include "Dynamic.h"
#include "Filter.h"
#include "Globals.h"
#include "Log.h"
#include "Utils.h"



Dynamic::Dynamic()
{
	Clear();
}


Dynamic::~Dynamic()
{
}


bool Dynamic::AsBool()
{
	switch (_DataType)
	{
	case ARDJACK_DATATYPE_BOOLEAN:
		return _BoolVal;

	case ARDJACK_DATATYPE_INTEGER:
		return (_IntVal != 0);

	case ARDJACK_DATATYPE_REAL:
		return (_DoubleVal != 0.0);

	case ARDJACK_DATATYPE_STRING:
		return Utils::String2Bool(_StringVal);

	default:
		return false;
	}
}


//DateTime Dynamic::AsDateTime()
//{
//	switch (_DataType)
//	{
//	case ARDJACK_DATATYPE_DATETIME:
//		return _DateTimeVal;
//
//	default:
//		return DateTime();
//	}
//}


double Dynamic::AsDouble()
{
	switch (_DataType)
	{
	case ARDJACK_DATATYPE_BOOLEAN:
		return _BoolVal ? 1.0 : 0.0;

	case ARDJACK_DATATYPE_INTEGER:
		return _IntVal;

	case ARDJACK_DATATYPE_REAL:
		return _DoubleVal;

	default:
		return 0.0;
	}
}


int Dynamic::AsInt()
{
	switch (_DataType)
	{
	case ARDJACK_DATATYPE_BOOLEAN:
		return _BoolVal ? 1 : 0;

	case ARDJACK_DATATYPE_INTEGER:
		return _IntVal;

	case ARDJACK_DATATYPE_REAL:
		return Utils::Nint(_DoubleVal);

	case ARDJACK_DATATYPE_STRING:
		return atoi(_StringVal);

	default:
		return 0;
	}
}


char* Dynamic::AsString(char* value)
{
	switch (_DataType)
	{
	case ARDJACK_DATATYPE_BOOLEAN:
		if (_BoolVal)
			strcpy(value, "1");
		else
			strcpy(value, "0");
		break;

	case ARDJACK_DATATYPE_INTEGER:
		itoa(_IntVal, value, 10);
		break;

	case ARDJACK_DATATYPE_REAL:
		sprintf(value, "%.3f", _DoubleVal);
		break;

	case ARDJACK_DATATYPE_STRING:
		strcpy(value, _StringVal);

	default:
		strcpy(value, "");
		break;
	}

	return value;
}


void Dynamic::Clear()
{
	_DataType = ARDJACK_DATATYPE_EMPTY;
	_IntVal = 0;
}


bool Dynamic::Copy(Dynamic* src)
{
	_DataType = src->DataType();

	switch (_DataType)
	{
	case ARDJACK_DATATYPE_BOOLEAN:
		_BoolVal = src->AsBool();
		break;

	case ARDJACK_DATATYPE_INTEGER:
		_IntVal = src->AsInt();
		break;

	case ARDJACK_DATATYPE_REAL:
		_DoubleVal = src->AsDouble();
		break;

	case ARDJACK_DATATYPE_STRING:
		src->AsString(_StringVal);
		break;
	}

	return true;
}


int Dynamic::DataType()
{
	return _DataType;
}


bool Dynamic::Equals(Dynamic* src, bool ignoreCase)
{
	if (IsEmpty() || src->IsEmpty())
		return false;

	if (_DataType == src->DataType())
	{
		switch (_DataType)
		{
		case ARDJACK_DATATYPE_BOOLEAN:
			return (_BoolVal == src->AsBool());

		case ARDJACK_DATATYPE_INTEGER:
			return (_IntVal == src->AsInt());

		case ARDJACK_DATATYPE_REAL:
			return (_DoubleVal == src->AsDouble());
			break;

		case ARDJACK_DATATYPE_STRING:
			return Utils::StringEquals(_StringVal, src->String());
		}
	}

	// The data types are different - compare the string representations for now.
	char temp1[ARDJACK_MAX_PART_VALUE_LENGTH];
	AsString(temp1);

	char temp2[ARDJACK_MAX_PART_VALUE_LENGTH];
	src->AsString(temp2);

	return Utils::StringEquals(temp1, temp2, ignoreCase);
}


bool Dynamic::IsEmpty()
{
	return (_DataType == ARDJACK_DATATYPE_EMPTY);
}


bool Dynamic::SetBool(bool value)
{
	_DataType = ARDJACK_DATATYPE_BOOLEAN;
	_BoolVal = value;

	return true;
}


//bool Dynamic::SetDateTime(DateTime value)
//{
//	_DataType = ARDJACK_DATATYPE_DATETIME;
//	_DateTimeVal = value;
//
//	return true;
//}


bool Dynamic::SetDouble(double value)
{
	_DataType = ARDJACK_DATATYPE_REAL;
	_DoubleVal = value;

	return true;
}


bool Dynamic::SetInt(int value)
{
	_DataType = ARDJACK_DATATYPE_INTEGER;
	_IntVal = value;

	return true;
}


bool Dynamic::SetString(const char* value)
{
	_DataType = ARDJACK_DATATYPE_STRING;
	strcpy(_StringVal, value);

	return true;
}


const char* Dynamic::String()
{
	if (_DataType != ARDJACK_DATATYPE_STRING)
	{
		char temp[82];
		ToString(temp);
		Log::LogError(PRM("Not a string: "), temp);

		return Globals::EmptyString;
	}

	return _StringVal;
}


const char* Dynamic::ToString(char* text)
{
	char temp[42];
	AsString(temp);

	sprintf(text, PRM("Dynamic: datatype '%d', '%s'"), _DataType, temp);

	return text;
}


bool Dynamic::ValuesDiffer(Dynamic* src, bool ignoreCase, Filter* filter, bool lastChangeState, long lastChangeMs,
	long lastNotifiedMs)
{
	// Is this value different (enough) to 'src'?
	// When 'filter' is supplied, 'src' is assumed to be the 'last notified value'.
	// 'lastNotifiedMs', if non-zero, is the last time that a significant change was detected.

	if (IsEmpty() && src->IsEmpty())
		return false;

	if (IsEmpty() || src->IsEmpty())
		return true;

	switch (_DataType)
	{
		case ARDJACK_DATATYPE_BOOLEAN:
		{
			bool boolSrc = src->AsBool();

			// Filtering?
			if (NULL == filter)
				return (_BoolVal != boolSrc);

			// Yes.
			return filter->Evaluate(_BoolVal, boolSrc, lastNotifiedMs, lastChangeState, lastChangeMs);
		}

		case ARDJACK_DATATYPE_STRING:
		{
			char temp[ARDJACK_MAX_DYNAMIC_STRING_LENGTH];
			src->AsString(temp);
			return !Utils::StringEquals(_StringVal, temp, ignoreCase);
		}
	}

	// This value is numeric.
	switch (src->DataType())
	{
		// The 'src' value is not numeric.
		case ARDJACK_DATATYPE_BOOLEAN:
		case ARDJACK_DATATYPE_STRING:
			return !Equals(src);
	}

	double dblThis = AsDouble();
	double dblSrc = src->AsDouble();

	// Any filter?
	if (NULL == filter)
		return (abs(dblThis - dblSrc) > 1.99);

	// Yes.
	return filter->Evaluate(dblThis, dblSrc, lastNotifiedMs);
}

