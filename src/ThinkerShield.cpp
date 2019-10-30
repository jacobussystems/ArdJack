/*
	ThinkerShield.cpp

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
	#include <arduino.h>
#else
	#include "stdafx.h"

	// Dummy pin numbers for use in Windows (for dev only).
	const static int A0 = 100;
	const static int A4 = 104;
	const static int A5 = 105;
	const static int DAC0 = 120;
	const static int LED_BUILTIN = 13;
#endif

#include "Device.h"
#include "Log.h"
#include "Shield.h"
#include "ThinkerShield.h"
#include "Utils.h"


#ifdef ARDJACK_INCLUDE_SHIELDS
#ifdef ARDJACK_INCLUDE_THINKER_SHIELD

ThinkerShield::ThinkerShield(const char* name)
	: Shield(name)
{
}


bool ThinkerShield::CreateInventory()
{
	// Add Parts to Device 'Owner'.
	if (Globals::Verbosity > 3)
		Log::LogInfoF(PRM("ThinkerShield::CreateInventory: Entry, %d Parts"), Owner->PartCount);

	Owner->PrepareForCreateInventory();

	// Analog Inputs (ADC pins).
	Owner->AddParts("ai", 4, ARDJACK_PART_TYPE_ANALOG_INPUT, 0, 0, A0);

	// Analog Outputs (DAC pins).
	//Owner->AddParts("ao", 2, ARDJACK_PART_TYPE_ANALOGOUTPUT, 0, DAC0);

	// Buttons.
	Owner->AddPart("button0", ARDJACK_PART_TYPE_BUTTON, 0, 7);

	// Digital Inputs.
	Owner->AddPart("di0", ARDJACK_PART_TYPE_DIGITAL_INPUT, 0, 2);
	Owner->AddPart("di1", ARDJACK_PART_TYPE_DIGITAL_INPUT, 1, 4);

	// Digital Outputs.
	// Correct?
	//Owner->AddParts("do", 2, ARDJACK_PART_TYPE_DIGITAL_OUTPUT, 0, 0, 5);		// LED_BUILTIN = 6 on MKR1010
	Owner->AddParts("do", 1, ARDJACK_PART_TYPE_DIGITAL_OUTPUT, 0, 0, 5);

	// LEDs.
	// Correct?
#ifdef ARDJACK_ESP32
	//Owner->AddParts("led", 5, ARDJACK_PART_TYPE_LED, 0, 1, ?);
#else
	//Owner->AddPart("led0", ARDJACK_PART_TYPE_LED, 0, LED_BUILTIN);
	//Owner->AddParts("led", 5, ARDJACK_PART_TYPE_LED, 0, 1, 8);
	Owner->AddParts("led", 6, ARDJACK_PART_TYPE_LED, 0, 0, 8);
#endif

	// Light Sensor inputs.
	Owner->AddPart("ldr0", ARDJACK_PART_TYPE_LIGHT_SENSOR, 0, A4);

	// Potentiometer inputs.
	Owner->AddPart("pot0", ARDJACK_PART_TYPE_POTENTIOMETER, 0, A5);

	// Sounds.
	Owner->AddPart("sound0", ARDJACK_PART_TYPE_SOUND, 0, 3);

	Owner->RemoveOldParts();

	if (Globals::Verbosity > 3)
		Log::LogInfoF(PRM("ThinkerShield::CreateInventory: Exit, %d Parts"), Owner->PartCount);

	return true;
}

#endif
#endif
