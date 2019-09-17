/*
	Configuration.h

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

#include "Globals.h"

class ConfigProp;




class Configuration
{
protected:
	virtual bool GetSizes(int *nameSize, int *valueSize, int *descSize);

public:
	char Name[ARDJACK_MAX_NAME_LENGTH];
	uint8_t PropCount;
	ConfigProp* Properties[ARDJACK_MAX_CONFIG_PROPERTIES];

	Configuration();
	~Configuration();

	virtual ConfigProp* Add(const char* name, int dataType, const char* desc, const char* unit = "");
	virtual ConfigProp* AddBooleanProp(const char* name, const char* desc, bool value = false, const char* unit = "");
	//virtual ConfigProp* AddChild(const char* name, ConfigProp* parent, int dataType, const char* desc, const char* unit = "");
	virtual ConfigProp* AddIntegerProp(const char* name, const char* desc, int value = 0, const char* unit = "");
	virtual ConfigProp* AddRealProp(const char* name, const char* desc, double value = 0.0, const char* unit = "");
	virtual ConfigProp* AddStringProp(const char* name, const char* desc, const char* value = "", const char* unit = "");
	virtual bool GetAsBoolean(const char* path, bool* value);
	virtual bool GetAsInteger(const char* path, int* value);
	virtual bool GetAsReal(const char* path, double* value);
	virtual bool GetAsString(const char* path, char* value);
	virtual bool LogIt();
	virtual ConfigProp* LookupPath(const char* path);
	virtual bool SetFromString(const char* path, const char* value);
	virtual bool SortItems();
};

