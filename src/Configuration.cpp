/*
	Configuration.cpp

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
#include "Configuration.h"
#include "Globals.h"
#include "Log.h"
#include "Table.h"
#include "Utils.h"



Configuration::Configuration()
{
	strcpy(Name, "");
	PropCount = 0;

	for (int i = 0; i < ARDJACK_MAX_CONFIG_PROPERTIES; i++)
		Properties[i] = NULL;
}


Configuration::~Configuration()
{
	for (int i = 0; i < ARDJACK_MAX_CONFIG_PROPERTIES; i++)
	{
		if (NULL != Properties[i])
			delete Properties[i];
	}
}


ConfigProp* Configuration::Add(const char* name, int dataType, const char* desc, const char* unit)
{
	char temp[102];

	ConfigProp* result = LookupPath(name);

	if (NULL != result)
	{
		sprintf(temp, PRM("Reusing existing Configuration property '%s'"), result->Name());
		Log::LogInfo(temp);

		result->DataType = dataType;
		//result->Parent = NULL;
		result->SetDescription(desc);
		result->SetUnit(unit);

		return result;
	}

	if (PropCount >= ARDJACK_MAX_CONFIG_PROPERTIES)
	{
		sprintf(temp, PRM("Max.no.of Configuration properties reached (%d) - no room to add '%s'"),
			ARDJACK_MAX_CONFIG_PROPERTIES, name);
		Log::LogInfo(temp);

		return NULL;
	}

	// Create a new property.
	result = Properties[PropCount];

	if (NULL == result)
	{
		result = new ConfigProp();
		Properties[PropCount] = result;
	}

	PropCount++;

	result->SetName(name);
	result->DataType = dataType;
	result->SetDescription(desc);
	result->SetUnit(unit);

	return result;
}


ConfigProp* Configuration::AddBooleanProp(const char* name, const char* desc, bool value, const char* unit)
{
	ConfigProp* result = Add(name, ARDJACK_DATATYPE_BOOLEAN, desc, unit);
	if (NULL == result) return NULL;

	result->Value_Boolean = value;

	return result;
}


//ConfigProp* Configuration::AddChild(const char* name, ConfigProp* parent, int dataType, const char* desc, const char* unit)
//{
//	ConfigProp* result = Add(name, dataType, desc, unit);
//	if (NULL == result) return NULL;
//
//	result->Parent = parent;
//
//	return result;
//}


ConfigProp* Configuration::AddIntegerProp(const char* name, const char* desc, int value, const char* unit)
{
	ConfigProp* result = Add(name, ARDJACK_DATATYPE_INTEGER, desc, unit);
	if (NULL == result) return NULL;

	result->Value_Integer = value;

	return result;
}


ConfigProp* Configuration::AddRealProp(const char* name, const char* desc, double value, const char* unit)
{
	ConfigProp* result = Add(name, ARDJACK_DATATYPE_REAL, desc, unit);
	if (NULL == result) return NULL;

	result->Value_Real = value;

	return result;
}


ConfigProp* Configuration::AddStringProp(const char* name, const char* desc, const char* value, const char* unit)
{
	ConfigProp* result = Add(name, ARDJACK_DATATYPE_STRING, desc, unit);
	if (NULL == result) return NULL;

	result->SetFromString(value);

	return result;
}


bool Configuration::GetAsBoolean(const char* path, bool* value)
{
	value[0] = NULL;

	ConfigProp* prop = LookupPath(path);
	if (NULL == prop) return false;

	return prop->GetAsBoolean(value);
}


bool Configuration::GetAsInteger(const char* path, int* value)
{
	value[0] = NULL;

	ConfigProp* prop = LookupPath(path);
	if (NULL == prop) return false;

	return prop->GetAsInteger(value);
}


bool Configuration::GetAsReal(const char* path, double* value)
{
	value[0] = NULL;

	ConfigProp* prop = LookupPath(path);
	if (NULL == prop) return false;

	return prop->GetAsReal(value);
}


bool Configuration::GetAsString(const char* path, char* value)
{
	value[0] = NULL;

	ConfigProp* prop = LookupPath(path);
	if (NULL == prop) return false;

	return prop->GetAsString(value);
}


bool Configuration::GetSizes(int *nameSize, int *valueSize, int *descSize)
{
	*nameSize = Utils::StringLen("Name");
	*valueSize = Utils::StringLen("Value");
	*descSize = Utils::StringLen("Description");

	char temp[82];
	char value[82];

	for (int i = 0; i < PropCount; i++)
	{
		ConfigProp* prop = Properties[i];

		// Check the Name size.
		if (Utils::StringLen(prop->Name()) > *nameSize)
			*nameSize = Utils::StringLen(prop->Name());

		// Check the Description size.
		if (Utils::StringLen(prop->Description()) > *descSize)
			*descSize = Utils::StringLen(prop->Description());

		// Check the Value + Unit size.
		prop->GetAsString(value);
		temp[0] = NULL;

		if (prop->DataType == ARDJACK_DATATYPE_STRING)
		{
			strcat(temp, "'");
			strcat(temp, value);
			strcat(temp, "'");
		}
		else
		{
			strcat(temp, value);

			if (strlen(prop->Unit()) > 0)
			{
				strcat(temp, " ");
				strcat(temp, prop->Unit());
			}
		}

		if (Utils::StringLen(temp) > *valueSize)
			*valueSize = Utils::StringLen(temp);
	}

	return true;
}


bool Configuration::LogIt()
{
	Log::LogInfo();
	Log::LogInfo(PRM("CONFIGURATION"));

	int descSize;
	int nameSize;
	int valueSize;
	GetSizes(&nameSize, &valueSize, &descSize);

	Table table;
	table.AddColumn("Name", nameSize + 2);
	table.AddColumn("Value", valueSize + 2);
	table.AddColumn("Description", descSize + 2);

	char line[202];
	Log::LogInfo(table.HorizontalLine(line));
	Log::LogInfo(table.Header(line));
	Log::LogInfo(table.HorizontalLine(line));

	char temp[100];
	char value[80];

	for (int i = 0; i < PropCount; i++)
	{
		ConfigProp* prop = Properties[i];
		temp[0] = NULL;

		prop->GetAsString(value);

		if (prop->DataType == ARDJACK_DATATYPE_STRING)
		{
			strcat(temp, "'");
			strcat(temp, value);
			strcat(temp, "'");
		}
		else
		{
			strcat(temp, value);

			if (strlen(prop->Unit()) > 0)
			{
				strcat(temp, " ");
				strcat(temp, prop->Unit());
			}
		}

		Log::LogInfo(table.Row(line, prop->Name(), temp, prop->Description()));
	}

	Log::LogInfo(table.HorizontalLine(line));
	Log::LogInfo();

	return true;
}


ConfigProp* Configuration::LookupPath(const char* path)
{
	ConfigProp* result = NULL;
	//char temp[200];

	for (int i = 0; i < PropCount; i++)
	{
		ConfigProp* prop = Properties[i];

		//if (Utils::StringEquals(prop->Path(temp), path))
		if (Utils::StringEquals(prop->Name(), path))
		{
			result = prop;
			break;
		}
	}

	return result;
}


bool Configuration::SetFromString(const char* name, const char* value)
{
	ConfigProp* item = LookupPath(name);
	if (NULL == item) return false;

	return item->SetFromString(value);
}


int SortItemsCompare(const void* a, const void* b)
{
	// Thanks to:
	//		https://stackoverflow.com/questions/23689687/sorting-an-array-of-struct-pointers-using-qsort

	ConfigProp* itemA = *(ConfigProp**)a;
	char nameA[ARDJACK_MAX_NAME_LENGTH];
	strcpy(nameA, itemA->Name());

	ConfigProp* itemB = *(ConfigProp**)b;
	char nameB[ARDJACK_MAX_NAME_LENGTH];
	strcpy(nameB, itemB->Name());

#ifdef ARDUINO
	return strcasecmp(nameA, nameB);
#else
	return stricmp(nameA, nameB);
#endif
}


bool Configuration::SortItems()
{
	//qsort(Properties, PropCount, sizeof(ConfigProp*), SortItemsCompare);

	return true;
}

