/*
	ArduinoDHT.cpp

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

// Arduino only.

#include "Globals.h"

#ifdef ARDUINO

#ifdef ARDJACK_INCLUDE_ARDUINO_DHT

#define _strlwr strlwr

#include "stdafx.h"
#include <typeinfo>

#include <SimpleDHT.h>

#include "ArduinoDHT.h"
#include "Log.h"
#include "UserPart.h"
#include "Utils.h"


// For DHT11 and DHT22.
// See notes in ArduinoDHT.h.


ArduinoDHT::ArduinoDHT()
	: UserPart()
{
	_Dht11 = NULL;
	_Dht11_MinInterval = 1000;
	_Dht22 = NULL;
	_Dht22_MinInterval = 2000;
	_Interval = 1000;
	_Model = ARDJACK_ARDUINO_DHT_MODEL_DHT11;
	_ModelName[0] = NULL;
	_NextSampleTime = 0;
	_Variable = ARDJACK_ARDUINO_DHT_VAR_TEMPERATURE;
	_VariableName[0] = NULL;

	_IsAnalog = true;
	_IsInput = true;
}


ArduinoDHT::~ArduinoDHT()
{
	if (NULL != _Dht11)
		delete _Dht11;

	if (NULL != _Dht22)
		delete _Dht22;
}


bool ArduinoDHT::Activate()
{
	if (Globals::Verbosity > 6)
		Log::LogInfo(PRM("ArduinoDHT::Activate: '"), Name, "'");

	char temp[102];

	int oldPin = Pin;
	int oldModel = _Model;

	//Config->GetAsInteger("Pin", &Pin);
	Config->GetAsString("Model", _ModelName);
	Config->GetAsString("Variable", _VariableName);

	// Get the Model type.
	if (Utils::StringEquals(_ModelName, "dht11"))
		_Model = ARDJACK_ARDUINO_DHT_MODEL_DHT11;
	else
		_Model = ARDJACK_ARDUINO_DHT_MODEL_DHT22;

	if ((Pin != oldPin) || (_Model != oldModel))
	{
		// The Pin and/or Model have changed - delete any current DHT objects.
		DeleteObjects();
	}

	// Create a new DHT object if necessary.
	switch (_Model)
	{
	case ARDJACK_ARDUINO_DHT_MODEL_DHT11:
		if (NULL == _Dht11)
		{
			if (Globals::Verbosity > 3)
			{
				sprintf(temp, PRM("Creating a SimpleDHT11 on pin %d"), Pin);
				Log::LogInfo(PRM("ArduinoDHT::Activate: "), temp);
			}

			_Dht11 = new SimpleDHT11(Pin);
		}
		break;

	default:
		if (NULL == _Dht22)
		{
			if (Globals::Verbosity > 3)
			{
				sprintf(temp, PRM("Creating a SimpleDHT22 on pin %d"), Pin);
				Log::LogInfo(PRM("ArduinoDHT::Activate: "), temp);
			}

			_Dht22 = new SimpleDHT22(Pin);
		}
		break;
	}

	// Get the Variable type.
	if (Utils::StringEquals(_VariableName, "humidity"))
		_Variable = ARDJACK_ARDUINO_DHT_VAR_HUMIDITY;
	else
		_Variable = ARDJACK_ARDUINO_DHT_VAR_TEMPERATURE;

	// Set the interval.
	switch (_Model)
	{
	case ARDJACK_ARDUINO_DHT_MODEL_DHT11:
		_Interval = _Dht11_MinInterval;
		break;

	default:
		_Interval = _Dht22_MinInterval;
		break;
	}

	// Set the next sample time.
	_NextSampleTime = 0;

	if (Globals::Verbosity > 3)
	{
		sprintf(temp, PRM("Pin %d, model '%s', variable '%s', interval %d ms"), Pin, _ModelName, _VariableName, _Interval);
		Log::LogInfo(PRM("ArduinoDHT::Activate: "), temp);
	}

	return true;
}


bool ArduinoDHT::AddConfig()
{
	if (!UserPart::AddConfig())
		return false;

	if (NULL == Config)
		Config = new Configuration();

	Config->AddStringProp("Model", PRM("Model name (DHT11 | DHT22)."), "DHT11");
	Config->AddStringProp("Variable", PRM("Variable name (Temperature | Humidity)."), "Temperature");

	Config->SortItems();

	return true;
}


bool ArduinoDHT::Deactivate()
{

	return true;
}


bool ArduinoDHT::DeleteObjects()
{
	// Delete any current DHT object(s).
	if (NULL != _Dht11)
	{
		delete _Dht11;
		_Dht11 = NULL;
	}

	if (NULL != _Dht22)
	{
		delete _Dht22;
		_Dht22 = NULL;
	}
}


bool ArduinoDHT::Read(Dynamic* value)
{
	// Time to sample again?
	long nowMs = Utils::NowMs();

	if (nowMs < _NextSampleTime)
	{
		// No - keep using the current value.
		return true;
	}

	// Yes.
	if (Globals::Verbosity > 4)
		Log::LogInfoF(PRM("ArduinoDHT::Read: Reading '%s'"), _ModelName);

	// Bump the 'next sample' time.
	_NextSampleTime = nowMs + _Interval;

	byte temperature = 0;
	byte humidity = 0;
	int err;

	if (_Model == ARDJACK_ARDUINO_DHT_MODEL_DHT11)
		err = _Dht11->read(&temperature, &humidity, NULL);
	else
		err = _Dht22->read(&temperature, &humidity, NULL);

	if (err != SimpleDHTErrSuccess)
	{
		Log::LogErrorF(PRM("%s read failed, err %d"), _ModelName, err);
		return false;
	}

	int intVal = (_Variable == ARDJACK_ARDUINO_DHT_VAR_TEMPERATURE) ? temperature : humidity;
	value->SetInt(intVal);

	return true;
}

#endif

#endif
