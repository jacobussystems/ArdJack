/*
	Part.cpp

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
#include "Device.h"
#include "Dynamic.h"
#include "Enumeration.h"
#include "FilterManager.h"
#include "Log.h"
#include "Part.h"
#include "PartManager.h"
#include "Utils.h"



Part::Part()
{
	Config = NULL;
	Filt = NULL;
	FilterName[0] = NULL;
	IsNew = true;
	ItemCount = 0;
	LastChangeTime = 0;
	Name[0] = NULL;
	NotifiedTime = 0;
	NotifiedValue.Clear();
	Notify = false;
	Pin = 0;
	Subtype = ARDJACK_USERPART_SUBTYPE_NONE;
	Type = ARDJACK_PART_TYPE_NONE;

#ifdef ARDJACK_INCLUDE_MULTI_PARTS
	for (int i = 0; i < ARDJACK_MAX_MULTI_PART_ITEMS; i++)
		Items[i] = NULL;
#endif
}


Part::~Part()
{
	if (NULL != Config)
	{
		delete Config;
		Config = NULL;
	}

#ifdef ARDJACK_INCLUDE_MULTI_PARTS
	for (int i = 0; i < ARDJACK_MAX_MULTI_PART_ITEMS; i++)
	{
		if (NULL != Items[i])
		{
			delete Items[i];
			Items[i] = NULL;
		}
	}
#endif
}


bool Part::Activate()
{
	if (Globals::Verbosity > 6)
		Log::LogInfo(PRM("Part::Activate: '"), Name, "'");

	Filt = NULL;
	NotifiedValue.Clear();

	if (strlen(FilterName) > 0)
	{
		Filt = Globals::FilterMgr->LookupFilter(FilterName);

		if (NULL != Filt)
			Filt->SetActive(true);
	}

	return true;
}


bool Part::AddConfig()
{
	if (Globals::Verbosity > 6)
		Log::LogInfo(PRM("Part::AddConfig: "), Name);

	// WARNING - Uncommenting out the following will significantly increase the heap used by e.g. an 'ArduinoDevice'.
	// If it has 16 Parts and each Part gets a 'Configuration' (at least 96 bytes each), approx. 1600 bytes would be added.

	//if (NULL == Config)
	//	Config = new Configuration();

	////if (Config->AddIntegerProp("Pin", PRM("Pin number."), Pin) == NULL) return false;

	//Config->SortItems();

	return true;
}


bool Part::CheckChange()
{
	// Check for a change (the new value is already in 'Value').
	bool result = false;

	if (Globals::Verbosity > 5)
	{
		char temp[10];
		char temp2[10];

		if (IsDigitalInput())
		{
			Log::LogInfoF(PRM("'%s', Value '%s', nv '%s', lcs %d"),
				Name, Value.AsString(temp), NotifiedValue.AsString(temp2), LastChangeState);
		}
		else
		{
			Log::LogInfoF(PRM("'%s', Value '%s', nv '%s'"),
				Name, Value.AsString(temp), NotifiedValue.AsString(temp2));
		}
	}

	if (Value.ValuesDiffer(&NotifiedValue, false, Filt, LastChangeState, LastChangeTime, NotifiedTime))
	{
		result = true;
		NotifiedValue.Copy(&Value);
		NotifiedTime = Utils::NowMs();
	}

	if (IsDigitalInput())
	{
		bool newState = Value.AsBool();

		if (newState != LastChangeState)
		{
			LastChangeState = newState;
			LastChangeTime = Utils::NowMs();
		}
	}

	return result;
}


bool Part::Configure(StringList* settings, int start, int count)
{
	if (count == 0)
		return true;

	StringList fields;

	char itemName[ARDJACK_MAX_NAME_LENGTH];
	char itemValue[100];
	char temp[102];

	for (int i = start; i < start + count; i++)
	{
		Utils::SplitText(settings->Get(i), '=', &fields, 2, ARDJACK_MAX_VALUE_LENGTH);

		strcpy(itemName, fields.Get(0));
		strcpy(itemValue, fields.Get(1));

		// Special cases.
		if (Utils::StringEquals(itemName, "Filter"))
		{
			strcpy(FilterName, itemValue);
			continue;
		}

		if (Utils::StringEquals(itemName, "Pin"))
		{
			Pin = Utils::String2Int(itemValue, Pin);
			continue;
		}

		// Configuration item?
		if (NULL != Config)
		{
			ConfigProp* item = Config->LookupPath(itemName);

			if (NULL != item)
			{
				item->SetFromString(itemValue);

#ifdef ARDJACK_INCLUDE_MULTI_PARTS
				if (Utils::StringEquals(itemName, "Items"))
					ConfigureMulti(itemValue);
#endif
				continue;
			}
		}

		sprintf(temp, PRM("invalid Configuration item for Part '%s': '%s'"), Name, itemName);
		Log::LogError(temp);
	}

	return true;
}


#ifdef ARDJACK_INCLUDE_MULTI_PARTS

	bool Part::ConfigureMulti(const char* text)
	{
		// Configure a 'Multi' part from 'text'.
		// 'text' should be a list of parts, e.g.
		//      text = "button0 di0 di1"
		//      text = "led0 led2 led4"

		Log::LogInfo(PRM("ConfigureMulti: "), text);

		StringList fields;
		int count = Utils::SplitText(text, ' ', &fields, ARDJACK_MAX_VALUES, ARDJACK_MAX_VALUE_LENGTH);

		char partName[ARDJACK_MAX_NAME_LENGTH];
		Part *part;

		for (int i = 0; i < count; i++)
		{
			strcpy(partName, fields.Get(i));
			part = Owner->LookupPart(partName);

			if (NULL != part)
				Items[Count++] = part;
		}

		return true;
	}

#endif


bool Part::Deactivate()
{
	return true;
}


char* Part::GetConfigStr(char* out)
{
	sprintf(out, PRM("pin=%d \"filter=%s\""), Pin, FilterName);

	return out;
}


bool Part::IsAnalog()
{
	return Globals::PartMgr->IsAnalogType(Type);
}


bool Part::IsAnalogInput()
{
	return IsAnalog() && IsInput();
}


bool Part::IsAnalogOutput()
{
	return IsAnalog() && IsOutput();
}


bool Part::IsDigital()
{
	return Globals::PartMgr->IsDigitalType(Type);
}


bool Part::IsDigitalInput()
{
	return IsDigital() && IsInput();
}


bool Part::IsDigitalOutput()
{
	return IsDigital() && IsOutput();
}


bool Part::IsInput()
{
	return Globals::PartMgr->IsInputType(Type);
}


bool Part::IsOutput()
{
	return Globals::PartMgr->IsOutputType(Type);
}


bool Part::IsTextual()
{
	return Globals::PartMgr->IsTextualType(Type);
}


bool Part::Read(Dynamic* value)
{
	value->Clear();
	Value.Clear();

	return true;
}


bool Part::Write(Dynamic* value)
{
	Value.Copy(value);

	return true;
}

