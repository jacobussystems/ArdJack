/*
	ConfigProp.cpp

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
#else
	#include "stdafx.h"
#endif

#include "ConfigProp.h"
#include "Globals.h"
#include "Log.h"
#include "StringList.h"
#include "Utils.h"




ConfigProp::ConfigProp()
{
	_Strings = new StringList(40);

	DataType = ARDJACK_DATATYPE_BOOLEAN;
	//Parent = NULL;
	Value_Boolean = false;
	Value_Integer = 0;
	Value_Real = 0.0;
}


ConfigProp::~ConfigProp()
{
	delete _Strings;
}


const char* ConfigProp::Description()
{
	return _Strings->Get(1);
}


bool ConfigProp::GetAsBoolean(bool* value)
{
	switch (DataType)
	{
	case ARDJACK_DATATYPE_BOOLEAN:
		*value = Value_Boolean;
		break;

	case ARDJACK_DATATYPE_INTEGER:
		*value = (Value_Integer != 0);
		break;

	case ARDJACK_DATATYPE_REAL:
		*value = (Value_Real != 0.0);
		break;

	case ARDJACK_DATATYPE_STRING:
		*value = Utils::String2Bool(StringValue(), *value);
		break;
	}

	return true;
}


bool ConfigProp::GetAsInteger(int* value)
{
	switch (DataType)
	{
	case ARDJACK_DATATYPE_BOOLEAN:
		*value = Value_Boolean ? 1 : 0;
		break;

	case ARDJACK_DATATYPE_INTEGER:
		*value = Value_Integer;
		break;

	case ARDJACK_DATATYPE_REAL:
		*value = Utils::Nint(Value_Real);
		break;

	case ARDJACK_DATATYPE_STRING:
		Utils::String2Int(StringValue(), *value);
		break;
	}

	return true;
}


bool ConfigProp::GetAsReal(double* value)
{
	switch (DataType)
	{
	case ARDJACK_DATATYPE_BOOLEAN:
		*value = Value_Boolean ? 1 : 0;
		break;

	case ARDJACK_DATATYPE_INTEGER:
		*value = Value_Integer;
		break;

	case ARDJACK_DATATYPE_REAL:
		*value = Value_Real;
		break;

	case ARDJACK_DATATYPE_STRING:
		Utils::String2Double(StringValue(), *value);
		break;
	}

	return true;
}


bool ConfigProp::GetAsString(char* value)
{
	switch (DataType)
	{
	case ARDJACK_DATATYPE_BOOLEAN:
		strcpy(value, Utils::Bool2yesno(Value_Boolean));
		break;

	case ARDJACK_DATATYPE_INTEGER:
		ltoa(Value_Integer, value, 10);
		break;

	case ARDJACK_DATATYPE_REAL:
		sprintf(value, "%.4g", Value_Real);
		break;

	case ARDJACK_DATATYPE_STRING:
		strcpy(value, StringValue());
		break;
	}

	return true;
}


const char* ConfigProp::Name()
{
	return _Strings->Get(0);
}


//char* ConfigProp::Path(char* out)
//{
//	// Get the full path of this property.
//	out[0] = NULL;
//
//	if (NULL != Parent)
//	{
//		// Get the parent's path.
//		Parent->Path(out);
//	}
//
//	if (strlen(out) > 0)
//		strcat(out, ".");
//
//	strcat(out, Name());
//
//	return out;
//}


bool ConfigProp::SetDescription(const char* value)
{
	return _Strings->Put(1, value);
}


bool ConfigProp::SetName(const char* value)
{
	return _Strings->Put(0, value);
}


bool ConfigProp::SetFromString(const char* value)
{
	switch (DataType)
	{
	case ARDJACK_DATATYPE_BOOLEAN:
		Value_Boolean = Utils::String2Bool(value, Value_Boolean);
		break;

	case ARDJACK_DATATYPE_INTEGER:
		Value_Integer = Utils::String2Int(value, Value_Integer);
		break;

	case ARDJACK_DATATYPE_REAL:
		Value_Real = Utils::String2Double(value, Value_Real);
		break;

	case ARDJACK_DATATYPE_STRING:
		_Strings->Put(3, value);
		break;
	}

	return true;
}


bool ConfigProp::SetUnit(const char* value)
{
	return _Strings->Put(2, value);
}


const char* ConfigProp::StringValue()
{
	return _Strings->Get(3);
}


const char* ConfigProp::Unit()
{
	return _Strings->Get(2);
}

