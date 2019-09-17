/*
	ArduinoMFShield.cpp

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

#ifdef ARDJACK_INCLUDE_SHIELDS

#ifdef ARDUINO

#ifdef ARDJACK_INCLUDE_ARDUINO_MF_SHIELD

#define _strlwr strlwr

#include "stdafx.h"
#include <typeinfo>

// Uses 'MFShield' from:
//		https://github.com/DireCat/MFShield

#include <MFShield.h>

#include "ArduinoMFShield.h"
#include "Device.h"
#include "Dynamic.h"
#include "Log.h"
#include "Part.h"
#include "Shield.h"
#include "Utils.h"



ArduinoMFShield::ArduinoMFShield(const char* name)
	: Shield(name)
{
	_MFShield = new MFShield();

	_MFShield->showDisplay(true);
}


ArduinoMFShield::~ArduinoMFShield()
{
	if (NULL != _MFShield)
		delete _MFShield;
}


bool ArduinoMFShield::CreateInventory()
{
	// Add Parts to Device 'Owner'.
	if (Globals::Verbosity > 3)
		Log::LogInfo(PRM("ArduinoMFShield::CreateInventory"));

	Owner->PartCount = 0;

	// Analog Inputs (ADC pins).
	//Owner->AddParts("ai", 4, ARDJACK_PART_TYPE_ANALOG_INPUT, 0, 0, A0);

	// Analog Outputs (DAC pins).
	//Owner->AddParts("ao", 2, ARDJACK_PART_TYPE_ANALOGOUTPUT, 0, DAC0);

	// Buttons.
	Owner->AddParts("button", 3, ARDJACK_PART_TYPE_BUTTON, 0, 0, A1);

	// Digital Inputs.
	//Owner->AddPart("di0", ARDJACK_PART_TYPE_DIGITAL_INPUT, 0, 2);

	// Digital Outputs.
	//Owner->AddParts("do", 2, ARDJACK_PART_TYPE_DIGITAL_OUTPUT, 0, 0, 5);

	// LEDs.
	Owner->AddPart("led0", ARDJACK_PART_TYPE_LED, 0, 13);
	Owner->AddPart("led1", ARDJACK_PART_TYPE_LED, 0, 12);
	Owner->AddPart("led2", ARDJACK_PART_TYPE_LED, 0, 11);
	Owner->AddPart("led3", ARDJACK_PART_TYPE_LED, 0, 10);

	// Potentiometer inputs.
	Owner->AddPart("pot0", ARDJACK_PART_TYPE_POTENTIOMETER, 0, A0);

	// Sounds.
	Owner->AddPart("sound0", ARDJACK_PART_TYPE_SOUND, 0, 3);

	// 4 x 7-segment text display.
	Owner->AddPart("text0", ARDJACK_PART_TYPE_TEXT_DISPLAY, 0, 0);

	return true;
}


void ArduinoMFShield::OnKeyPress(uint8_t key)
{
	if (Globals::Verbosity > 3)
		Log::LogInfoF(PRM("ArduinoMFShield::OnKeyPress: key %d"), key);

}


bool ArduinoMFShield::Poll()
{
	_MFShield->loop();

	return true;
}


bool ArduinoMFShield::ReadPart(Part* part, Dynamic* value)
{
	switch (part->Type)
	{
	default:
		return false;
	}

	return true;
}


bool ArduinoMFShield::WritePart(Part* part, Dynamic* value)
{
	if (Globals::Verbosity > 3)
	{
		char temp[102];
		char temp2[ARDJACK_MAX_DYNAMIC_STRING_LENGTH];
		value.AsString(temp2);
		sprintf(temp, PRM("ArduinoMFShield::WritePart: type %d, '%s'"), part->Type, temp2);
		Log::LogInfo(temp);
	}

	int intValue;

	switch (part->Type)
	{
	//case ARDJACK_PART_TYPE_LED:
	//	_MFShield->setLed(0, 1);
	//	break;

	case ARDJACK_PART_TYPE_SOUND:
		// Beep for 5 ms (no longer, because it's too loud!).
		_MFShield->beep(5);
		break;

	case ARDJACK_PART_TYPE_TEXT_DISPLAY:
		intValue = value.AsInt();
		_MFShield->display(intValue);
		break;

	default:
		return false;
	}

	return true;
}

#endif
#endif
#endif

