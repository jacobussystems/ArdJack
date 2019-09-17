/*
	Beacon.cpp

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

#include "Beacon.h"
#include "Configuration.h"
#include "Connection.h"
#include "Device.h"
#include "Dictionary.h"
#include "FieldReplacer.h"
#include "Globals.h"
#include "Log.h"
#include "Utils.h"



#ifdef ARDJACK_INCLUDE_BEACONS

Beacon::Beacon(const char* name)
	: IoTObject(name)
{
	_Command[0] = NULL;
	strcpy(_DateFormat, "dd MM yyyy");
	_FieldReplacer = new FieldReplacer();
	_Interval = 1000;																		// ms
	_NextOutputTime = 0;
	_Part = NULL;
	_Substitutes = new Dictionary();
	_Target = NULL;
	_TargetWasActive = false;
	_Text[0] = NULL;
	strcpy(_TimeFormat, "HH mm ss");

	Events = 0;
}


Beacon::~Beacon()
{
	delete _FieldReplacer;
	delete _Substitutes;
}


bool Beacon::Activate()
{
	if (!IoTObject::Activate())
		return false;

	// Save the Target state.
	_TargetWasActive = _Target->Active();

	// Start.
	_NextOutputTime = 0;
	_Target->SetActive(true);

	return true;
}


bool Beacon::AddConfig()
{
	if (!IoTObject::AddConfig())
		return false;

	Config->AddStringProp(PRM("Command"), PRM("Command to execute at every output time."), _Command);
	Config->AddStringProp(PRM("DateFormat"), PRM("Date format (when 'Text' contains ""[date]"")."), _DateFormat);
	Config->AddIntegerProp(PRM("Interval"), PRM("Interval."), _Interval, "ms");
	Config->AddStringProp(PRM("Part"), PRM("Part to output to (when 'Target' is a Device)."), "");
	Config->AddStringProp(PRM("Target"), PRM("Target object (name of a Connection or Device)."), "");
	Config->AddStringProp(PRM("Text"), PRM("Text to output."), "");
	Config->AddStringProp(PRM("TimeFormat"), PRM("Time format (when 'Text' contains ""[time]"")."), _TimeFormat);

	return Config->SortItems();
}


bool Beacon::ApplyConfig(bool quiet)
{
	if (Globals::Verbosity > 4)
		Log::LogInfo(PRM("Beacon::ApplyConfig"));

	char name[ARDJACK_MAX_NAME_LENGTH];
	char temp[102];
	char value[82];

	// Get the Target.
	Config->GetAsString("Target", name);
	if (strlen(name) == 0)
	{
		sprintf(temp, PRM("Beacon '%s': No Target name"), Name);
		Log::LogError(temp);
		return false;
	}

	_Target = Globals::ObjectRegister->LookupName(name);
	if (NULL == _Target)
	{
		sprintf(temp, PRM("Beacon '%s': Invalid Target name ('%s')"), Name, name);
		Log::LogError(temp);
		return false;
	}

	if (_Target->Type == ARDJACK_OBJECT_TYPE_DEVICE)
	{
		// Get the Part.
		Config->GetAsString("Part", name);
		if (strlen(name) == 0)
		{
			sprintf(temp, PRM("Beacon '%s': No Part for Target Device '%s'"), Name, _Target->Name);
			Log::LogError(temp);
			return false;
		}

		Device* dev = (Device*)_Target;
		_Part = dev->LookupPart(name);

		if (NULL == _Part)
		{
			sprintf(temp, PRM("Beacon '%s': No such Part for Target Device '%s': '%s'"), Name, _Target->Name, name);
			Log::LogError(temp);
			return false;
		}
	}

	// Get the other configuration values.
	Config->GetAsString(PRM("Command"), _Command);
	Config->GetAsString(PRM("DateFormat"), _DateFormat);
	Config->GetAsString(PRM("Text"), _Text);
	Config->GetAsString(PRM("TimeFormat"), _TimeFormat);

	Config->GetAsString(PRM("Interval"), value);
	_Interval = Utils::String2Int(value, _Interval);

	// Setup the 'Substitutes' dictionary.
	_Substitutes->Add(PRM("beacon"), Name);

	// Setup the 'FieldReplacer'.
	strcpy(_FieldReplacer->Params.DateFormat, _DateFormat);
	strcpy(_FieldReplacer->Params.TimeFormat, _TimeFormat);

	return true;
}


char* Beacon::ApplyReplacements(const char* text, char* output)
{
	// Apply any replacements  in 'text'.
	_FieldReplacer->ReplaceFields(text, _Substitutes, &FieldReplacer_ReplaceField, output);

	return output;
}


bool Beacon::Deactivate()
{
	// Deactivate the Target, if it was initially inactive.
	_Target->SetActive(_TargetWasActive);

	return true;
}


bool Beacon::Output()
{
	if (!_Active) return false;

	bool status = true;

	if (strlen(_Text) > 0)
	{
		// Send '_Text'.
		status = !Send(_Text);
	}

	if (strlen(_Command) > 0)
	{
		// Execute '_Command'.
		Globals::QueueCommand(_Command);
	}

	Events++;

	return status;
}


bool Beacon::Poll()
{
	if (!_Active) return false;

	// Time to output?
	long nowMs = Utils::NowMs();

	if (nowMs >= _NextOutputTime)
	{
		// Yes.
		Output();

		// Bump the 'next output' time.
		_NextOutputTime = nowMs + _Interval;
	}

	return true;
}


bool Beacon::Send(const char* text)
{
	// Process and send 'text'.
	if (!_Active) return false;

	char useText[202];
	ApplyReplacements(text, useText);

	if (Globals::Verbosity > 3)
		Log::LogInfo(PRM("Beacon::Send: To '"), _Target->Name, "': '", useText, "'");

	switch (_Target->Type)
	{
		case ARDJACK_OBJECT_TYPE_CONNECTION:
			{
				Connection* conn = (Connection*)_Target;
				conn->OutputText(useText);
			}
			break;

		case ARDJACK_OBJECT_TYPE_DEVICE:
			break;
	}

	return true;
}

#endif
