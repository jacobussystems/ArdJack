/*
	DataLogger.cpp

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

#include "Connection.h"
#include "DataLogger.h"
#include "Device.h"
#include "Dynamic.h"
#include "FieldReplacer.h"
#include "Globals.h"
#include "IoTObject.h"
#include "Log.h"
#include "Part.h"
#include "Utils.h"


#ifdef ARDJACK_INCLUDE_DATALOGGERS

//void Connection_Callback(void* caller, Route* route, IoTMessage* msg)
//{
//	//Log::LogInfo(PRM("Connection_Callback"));
//	DataLogger* logger = (DataLogger *)caller;
//	logger->Connection_Callback(route, msg);
//}



DataLogger::DataLogger(const char* name)
	: IoTObject(name)
{
	_Connection = NULL;
	_ConnectionWasActive = false;
	strcpy(_DateFormat, "dd MM yyyy");
	_Device = NULL;
	_DeviceWasActive = false;
	_FieldReplacer = new FieldReplacer();
	_Interval = 1000;																	// ms
	_OutputFormat[0] = NULL;
	_Prefix[0] = NULL;
	strcpy(_TimeFormat, "HH mm ss");

	Events = 0;
	PartCount = 0;
}


DataLogger::~DataLogger()
{
	delete _FieldReplacer;
}


bool DataLogger::Activate()
{
	if (!IoTObject::Activate())
		return false;

	// Save the states.
	_ConnectionWasActive = _Connection->Active();
	_DeviceWasActive = _Device->Active();

	// Start.
	_NextSampleTime = 0;
	_Device->SetActive(true);
	_Connection->SetActive(true);


	return true;
}


bool DataLogger::AddConfig()
{
	if (!IoTObject::AddConfig())
		return false;

	Config->AddStringProp(PRM("Input"), PRM("Input (a Device name)."), "ard");
	Config->AddStringProp(PRM("InParts"), PRM("Device Parts to log (space-separated names)."), "");
	Config->AddIntegerProp(PRM("Interval"), PRM("Sample interval."), _Interval, "ms");
	Config->AddStringProp(PRM("Output"), PRM("Output (a Connection name)."), "");
	Config->AddStringProp(PRM("OutFormat"), PRM("Output format."), "");
	Config->AddStringProp(PRM("Prefix"), PRM("Output prefix."), "");
	Config->AddStringProp(PRM("DateFormat"), PRM("Date format (when 'Prefix' contains ""[date]"")."), _DateFormat);
	Config->AddStringProp(PRM("TimeFormat"), PRM("Time format (when 'Prefix' contains ""[time]"")."), _TimeFormat);

	return Config->SortItems();
}


bool DataLogger::ApplyConfig(bool quiet)
{
	if (Globals::Verbosity > 4)
		Log::LogInfo(PRM("DataLogger::ApplyConfig"));

	char inPartNames[120];

	Config->GetAsString("DateFormat", _DateFormat);
	Config->GetAsString("InParts", inPartNames);
	Config->GetAsInteger("Interval", &_Interval);
	Config->GetAsString("OutFormat", _OutputFormat);
	Config->GetAsString("Prefix", _Prefix);
	Config->GetAsString("TimeFormat", _TimeFormat);

	char name[ARDJACK_MAX_NAME_LENGTH];
	char temp[102];

	// Setup the the input (a Device).
	Config->GetAsString("Input", name);
	IoTObject* obj = Globals::ObjectRegister->LookupName(name);

	if ((NULL == obj) || (obj->Type != ARDJACK_OBJECT_TYPE_DEVICE))
	{
		sprintf(temp, PRM("DataLogger '%s': '%s' is not a Device"), Name, name);
		Log::LogError(temp);
		return false;
	}

	_Device = (Device*)obj;

	// Setup the specified input Parts.
	_Device->LookupParts(inPartNames, Parts, &PartCount);

	// Setup the output (a Connection).
	Config->GetAsString("Output", name);
	obj = Globals::ObjectRegister->LookupName(name);

	if ((NULL == obj) || (obj->Type != ARDJACK_OBJECT_TYPE_CONNECTION))
	{
		sprintf(temp, PRM("DataLogger '%s': '%s' is not a Connection"), Name, name);
		Log::LogError(temp);
		return false;
	}

	_Connection = (Connection*)obj;

	// Setup the 'FieldReplacer'.
	strcpy(_FieldReplacer->Params.DateFormat, _DateFormat);
	strcpy(_FieldReplacer->Params.TimeFormat, _TimeFormat);

	return true;
}


bool DataLogger::Deactivate()
{
	// Deactivate objects that were initially inactive.
	_Device->SetActive(_DeviceWasActive);
	_Connection->SetActive(_ConnectionWasActive);

	return IoTObject::Deactivate();
}


bool DataLogger::Poll()
{
	if (!_Active) return false;

	// Is it time to sample?
	long nowMs = Utils::NowMs();

	if (nowMs >= _NextSampleTime)
	{
		// Yes.
		Sample();

		// Bump the 'next sample' time.
		_NextSampleTime = nowMs + _Interval;
	}

	return true;
}


bool DataLogger::Sample()
{
	char time[20];
	Utils::GetTimeString(time, "");

	if (Globals::Verbosity > 5)
		Log::LogInfo(time, PRM(" Sample"));

	char line[202];
	line[0] = NULL;

	if (strlen(_Prefix) > 0)
	{
		// Apply any replacements in '_Prefix', e.g. '[date]' or '[time]'.
		_FieldReplacer->ReplaceFields(_Prefix, NULL, &FieldReplacer_ReplaceField, line);
		strcat(line, " ");
	}

	Part* part;
	Dynamic value;
	char temp[10];

	for (int i = 0; i < PartCount; i++)
	{
		if (i > 0)
			strcat(line, " ");

		part = Parts[i];

		_Device->Read(part, &value);

		if (part->Value.IsEmpty())
			strcpy(temp, "-");
		else
			part->Value.AsString(temp);

		strcat(line, temp);
	}

	_Connection->OutputText(line);
	Events++;

	return true;
}

#endif
