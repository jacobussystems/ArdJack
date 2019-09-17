/*
	IoTObject.cpp

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
	#include <typeinfo>
#endif

#include "ConfigProp.h"
#include "Configuration.h"
#include "Globals.h"
#include "IoTObject.h"
#include "Log.h"
#include "Utils.h"



IoTObject::IoTObject(const char* name)
{
	if (Globals::Verbosity > 7)
		Log::LogInfo(PRM("IoTObject ctor: '"), name, "'");

	_Active = false;

	Config = new Configuration();
	strcpy(Name, name);
	Subtype = ARDJACK_OBJECT_SUBTYPE_UNKNOWN;
	Type = ARDJACK_OBJECT_TYPE_UNKNOWN;
}


IoTObject::~IoTObject()
{
	if (Globals::Verbosity > 7)
		Log::LogInfo(PRM("IoTObject ~"));

	if (NULL != Config)
		delete Config;
}


bool IoTObject::Activate()
{
	if (Globals::Verbosity > 3)
		Log::LogInfo(PRM("IoTObject::Activate: '"), Name, "'");

	if (!ValidateConfig())
		return false;

	if (!ApplyConfig())
		return false;

	return true;
}


bool IoTObject::Active()
{
	return _Active;
}



bool IoTObject::AddConfig()
{
	// To be called -after- the constructor (not from it), when instances of subclasses have been fully created.
	if (Globals::Verbosity > 4)
		Log::LogInfo(PRM("IoTObject::AddConfig: '"), Name, "'");

	return true;
}


bool IoTObject::ApplyConfig(bool quiet)
{
	if (Globals::Verbosity > 4)
		Log::LogInfo(PRM("IoTObject::ApplyConfig"));

	return true;
}


bool IoTObject::Configure(const char* entity, StringList* settings, int start, int count)
{
	if (count == 0)
		return true;

	char propName[80];
	char propValue[ARDJACK_MAX_VALUE_LENGTH];

	propName[0] = NULL;
	propValue[0] = NULL;

	// Ensure it's inactive.
	bool wasActive = Active();

	if (!SetActive(false))
		return false;

	StringList fields;

	for (int i = start; i < start + count; i++)
	{
		int count = Utils::SplitText(settings->Get(i), '=', &fields, 2, ARDJACK_MAX_VALUE_LENGTH);
		strcpy(propName, fields.Get(0));

		if (count >= 2)
			strcpy(propValue, fields.Get(1));
		else
			strcpy(propValue, "");

		ConfigureProperty(propName, propValue, settings->Get(i));
	}

	// Restore the previous Active state.
	if (wasActive)
	{
		if (!SetActive(true))
			return false;
	}

	return true;
}


bool IoTObject::ConfigureProperty(const char* propName, const char* propValue, const char* text)
{
	ConfigProp* prop = Config->LookupPath(propName);

	if (NULL == prop)
	{
		Log::LogError(PRM("Invalid Config property in '"), text, "'");
		return false;
	}

	if (Globals::Verbosity > 7)
		Log::LogInfo(PRM("Config property: '"), propName, "' = '", propValue, "'");

	return prop->SetFromString(propValue);
}


bool IoTObject::Deactivate()
{
	return true;
}


bool IoTObject::Poll()
{
	return false;
}


bool IoTObject::SetActive(bool state)
{
	if (state == _Active)
		return true;

	if (state)
	{
		// Become active.
		if (!Activate())
		{
			Log::LogInfo(PRM("SetActive(true): cancelled"));
			return false;
		}

		_Active = true;
	}
	else
	{
		// Become inactive.
		if (!Deactivate())
		{
			Log::LogInfo(PRM("SetActive(false): cancelled"));
			return false;
		}

		_Active = false;
	}

	return true;
}


bool IoTObject::Special(int action)
{
	char temp[102];
	sprintf(temp, PRM("IoTObject::Special: Action %d - ignored"), action);
	Log::LogError(temp);

	return true;
}


const char* IoTObject::ToString(char* text)
{
	sprintf(text, PRM("'%s' (type '%s', subtype '%s')"),
		Name, Register::ObjectTypeName(Type), Register::ObjectSubtypeName(Type, Subtype));

	return text;
}


bool IoTObject::ValidateConfig(bool quiet)
{
	if (Globals::Verbosity > 4)
		Log::LogInfo(PRM("IoTObject::ValidateConfig"));

	return true;
}

