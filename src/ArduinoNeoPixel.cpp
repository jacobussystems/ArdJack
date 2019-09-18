/*
	ArduinoNeoPixel.cpp

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

#ifdef ARDJACK_INCLUDE_ARDUINO_NEOPIXEL

#define _strlwr strlwr

#include "stdafx.h"
#include <typeinfo>

#include <Adafruit_NeoPixel.h>

#include "ArduinoNeoPixel.h"
#include "Log.h"
#include "UserPart.h"
#include "Utils.h"



ArduinoNeoPixel::ArduinoNeoPixel()
	: UserPart()
{
	_Strip = new Adafruit_NeoPixel(NUMBER_PIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

	//_ModelName[0] = NULL;
	//_VariableName[0] = NULL;

	_IsAnalog = true;
	_IsOutput = true;
}


ArduinoNeoPixel::~ArduinoNeoPixel()
{
	if (NULL != _Strip)
		delete _Strip;
}


bool ArduinoNeoPixel::Activate()
{
	//Config->GetAsString("Model", _ModelName);
	//Config->GetAsString("Variable", _VariableName);

	//Log::LogInfo(PRM("ArduinoNeoPixel::Activate: Model '"), _ModelName, "', variable '", _VariableName, "'");

	Log::LogInfo(PRM("ArduinoNeoPixel::Activate"));
	_Strip->begin();

	return true;
}


bool ArduinoNeoPixel::AddConfig()
{
	if (!UserPart::AddConfig())
		return false;

	//if (NULL == Config)
	//	Config = new Configuration();

	////if (Config->AddStringProp("Model", PRM("Model name (DHT11 | DHT22)."), "DHT11") == NULL) return false;
	////if (Config->AddStringProp("Variable", PRM("Variable name (Temperature | Humidity)."), "Temperature") == NULL) return false;

	//Config->SortItems();

	return true;
}


bool ArduinoNeoPixel::Deactivate()
{
	return true;
}


uint32_t ArduinoNeoPixel::Wheel(byte WheelPos)
{
	// Input a value 0 to 255 to get a color value.
	// The colours are a transition r - g - b - back to r.

	WheelPos = 255 - WheelPos;

	if (WheelPos < 85)
	{
		return _Strip->Color(255 - WheelPos * 3, 0, WheelPos * 3);
	}
	else if (WheelPos < 170)
	{
		WheelPos -= 85;
		return _Strip->Color(0, WheelPos * 3, 255 - WheelPos * 3);
	}
	else
	{
		WheelPos -= 170;
		return _Strip->Color(WheelPos * 3, 255 - WheelPos * 3, 0);
	}
}


bool ArduinoNeoPixel::Write(Dynamic value)
{
	if (count > 0)
	{
		int color = value.AsInt;

		_Strip->setPixelColor(0, Wheel(color));
		_Strip->show();
	}

	return true;
}

#endif

#endif
