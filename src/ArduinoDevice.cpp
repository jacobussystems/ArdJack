/*
	ArduinoDevice.cpp

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

#ifdef ARDUINO

#include "ArduinoDevice.h"
#include "Dynamic.h"
#include "Log.h"
#include "Part.h"
#include "Shield.h"
#include "UserPart.h"
#include "Utils.h"



ArduinoDevice::ArduinoDevice(const char *name) : Device(name)
{
	strcpy(DeviceClass, "ArduinoDevice");

	CreateDefaultInventory();
}


ArduinoDevice::~ArduinoDevice()
{
}


bool ArduinoDevice::AddConfig()
{
	if (!Device::AddConfig())
		return false;

	Log::LogInfo(PRM("ArduinoDevice::AddConfig"));

	//if (Config->AddStringProp("?", "?") == NULL) return false;

	return true;
}


Part* ArduinoDevice::AddPart(const char* name, int type, int subtype, int pin)
{
	Part *pPart = Device::AddPart(name, type, subtype, pin);

	switch (type)
	{
		case ARDJACK_PART_TYPE_ANALOG_INPUT:
		case ARDJACK_PART_TYPE_HUMIDITY_SENSOR:
		case ARDJACK_PART_TYPE_LIGHT_SENSOR:
		case ARDJACK_PART_TYPE_POTENTIOMETER:
		case ARDJACK_PART_TYPE_TEMPERATURE_SENSOR:
			break;

		case ARDJACK_PART_TYPE_ANALOG_OUTPUT:
			break;

		case ARDJACK_PART_TYPE_BUTTON:
		case ARDJACK_PART_TYPE_DIGITAL_INPUT:
		case ARDJACK_PART_TYPE_SWITCH:
			pinMode(pin, INPUT);
			break;

		case ARDJACK_PART_TYPE_DIGITAL_OUTPUT:
		case ARDJACK_PART_TYPE_LED:
		case ARDJACK_PART_TYPE_SOUND:
			pinMode(pin, OUTPUT);
			break;

		case ARDJACK_PART_TYPE_TEXT_DISPLAY:
			break;
	}

	return pPart;
}


bool ArduinoDevice::ApplyConfig(bool quiet)
{
	// N.B. DON'T call the base class as the methods may overlap.
	if (Globals::Verbosity > 4)
		Log::LogInfo(PRM("ArduinoDevice::ApplyConfig"));

	//if (!CreateDefaultInventory())
	//	return false;

	if (!Device::ApplyConfig(quiet))
		return false;

	return true;
}


#ifdef ARDJACK_ARDUINO_DUE
	bool ArduinoDevice::ConfigureDUE()
	{
		Log::LogInfo(PRM("ConfigureDUE"));

		// Analog Inputs (ADC pins).
		AddParts("ai", 6, ARDJACK_PART_TYPE_ANALOG_INPUT, 0, 0, A0);

		// Analog Outputs (DAC pins).
		AddParts("ao", 2, ARDJACK_PART_TYPE_ANALOG_OUTPUT, 0, 0, DAC0);

		// Digital Inputs.
#ifdef ARDJACK_INCLUDE_ARDUINO_DHT
		// Keep pin 2 for DHT.
		AddParts("di", 5, ARDJACK_PART_TYPE_DIGITAL_INPUT, 0, 0, 3);
#else
		AddParts("di", 6, ARDJACK_PART_TYPE_DIGITAL_INPUT, 0, 0, 2);
#endif

		// Digital Outputs.
		AddParts("do", 5, ARDJACK_PART_TYPE_DIGITAL_OUTPUT, 0, 0, 8);

		// LEDs.
		AddPart("led0", ARDJACK_PART_TYPE_LED, 0, LED_BUILTIN);

#ifdef ARDJACK_INCLUDE_ARDUINO_DHT
		// DHT.
		AddPart("dht0", ARDJACK_PART_TYPE_USER, ARDJACK_USERPART_SUBTYPE_ARDUINO_DHT, 2);
#endif

		return true;
	}
#endif


#ifdef ARDJACK_ARDUINO_MEGA2560
	bool ArduinoDevice::ConfigureMega2560()
	{
		Log::LogInfo(PRM("ConfigureMega2560"));

		// Analog Inputs (ADC pins).
		AddParts("ai", 4, ARDJACK_PART_TYPE_ANALOG_INPUT, 0, 0, A0);

		// Digital Inputs.
		AddParts("di", 4, ARDJACK_PART_TYPE_DIGITAL_INPUT, 0, 0, 2);

		// Digital Outputs.
		AddParts("do", 4, ARDJACK_PART_TYPE_DIGITAL_OUTPUT, 0, 0, 8);

		// LEDs.
		AddPart("led0", ARDJACK_PART_TYPE_LED, 0, LED_BUILTIN);

		return true;
	}
#endif


#ifdef ARDJACK_ARDUINO_MKR
	bool ArduinoDevice::ConfigureArduinoMKR()
	{
		Log::LogInfo(PRM("ConfigureArduinoMKR"));

		// Analog Inputs (ADC pins).
		AddParts("ai", 6, ARDJACK_PART_TYPE_ANALOG_INPUT, 0, 0, PIN_A1);

		// Analog Outputs (DAC pins).
		AddPart("ao0", ARDJACK_PART_TYPE_ANALOG_OUTPUT, 0, PIN_A0);

		// Digital Inputs.
#ifdef ARDJACK_INCLUDE_ARDUINO_DHT
		// Keep pin 2 for DHT.
		AddParts("di", 2, ARDJACK_PART_TYPE_DIGITAL_INPUT, 0, 0, PIN_PA00);
		AddParts("di", 3, ARDJACK_PART_TYPE_DIGITAL_INPUT, 0, 2, PIN_PA03);
		AddParts("di", 1, ARDJACK_PART_TYPE_DIGITAL_INPUT, 0, 5, PIN_PA09);
#else
		AddParts("di", 6, ARDJACK_PART_TYPE_DIGITAL_INPUT, 0, 0, PIN_PA00);
#endif

		// Digital Outputs.
		AddParts("do", 4, ARDJACK_PART_TYPE_DIGITAL_OUTPUT, 0, 0, PIN_PA10);

		// LEDs.
		AddPart("led0", ARDJACK_PART_TYPE_LED, 0, LED_BUILTIN);

#ifdef ARDJACK_INCLUDE_ARDUINO_DHT
		// DHT.
		AddPart("dht0", ARDJACK_PART_TYPE_USER, ARDJACK_USERPART_SUBTYPE_ARDUINO_DHT, PIN_PA02);
#endif

		return true;
	}
#endif


#ifdef ARDJACK_ARDUINO_SAMD_ZERO
	bool ArduinoDevice::ConfigureSparkFunRedBoardTurbo()
	{
		// Analog Inputs.
		AddParts("ai", 5, ARDJACK_PART_TYPE_ANALOG_INPUT, 0, 0, A1);

		// Analog Output (DAC pin).
		AddPart("ao0", ARDJACK_PART_TYPE_ANALOG_OUTPUT, 0, A0);

		// Digital Inputs.
#ifdef ARDJACK_INCLUDE_ARDUINO_DHT
		// Keep pin 2 for DHT.
		AddParts("di", 2, ARDJACK_PART_TYPE_DIGITAL_INPUT, 0, 0, 0);
		AddParts("di", 3, ARDJACK_PART_TYPE_DIGITAL_INPUT, 0, 2, 3);
		//AddParts("di", 1, ARDJACK_PART_TYPE_DIGITAL_INPUT, 0, 5, 9);
#else
		AddParts("di", 6, ARDJACK_PART_TYPE_DIGITAL_INPUT, 0, 0, 0);
		//AddParts("di", 1, ARDJACK_PART_TYPE_DIGITAL_INPUT, 0, 6, 9);
#endif

		// Digital Outputs.
		AddParts("do", 3, ARDJACK_PART_TYPE_DIGITAL_OUTPUT, 0, 0, 10);

		// LEDs.
		AddPart("led0", ARDJACK_PART_TYPE_LED, 0, LED_BUILTIN);

#ifdef ARDJACK_INCLUDE_ARDUINO_DHT
		// DHT.
		AddPart("dht0", ARDJACK_PART_TYPE_USER, ARDJACK_USERPART_SUBTYPE_ARDUINO_DHT, 2);
#endif

#ifdef ARDJACK_INCLUDE_ARDUINO_NEOPIXEL
		// Multi-color LED (WS2812) on pin 44.
		AddPart("led1", ARDJACK_PART_TYPE_USER, ARDJACK_USERPART_SUBTYPE_ARDUINO_NEOPIXEL, RGB_LED);
#endif

		return true;
	}
#endif


#ifdef ARDJACK_ARDUINO_UNO
	bool ArduinoDevice::ConfigureUno()
	{
		Log::LogInfo(PRM("ConfigureUno"));

		// Analog Inputs (ADC pins).
		AddParts("ai", 6, ARDJACK_PART_TYPE_ANALOG_INPUT, 0, 0, A0);

		// Analog Outputs (DAC pins).
		AddParts("ao", 0, ARDJACK_PART_TYPE_ANALOG_OUTPUT, 0, 0, 66);

		// Digital Inputs.
		AddParts("di", 3, ARDJACK_PART_TYPE_DIGITAL_INPUT, 0, 0, 4);
		AddParts("di", 2, ARDJACK_PART_TYPE_DIGITAL_INPUT, 3, 0, 11);

		// Digital Outputs.
		AddParts("do", 5, ARDJACK_PART_TYPE_DIGITAL_OUTPUT, 0, 0, 14);				// start pin number?

		// LEDs.
		AddPart("led0", ARDJACK_PART_TYPE_LED, 0, LED_BUILTIN);

		return true;
	}
#endif

#ifdef ARDJACK_ESP32
	bool ArduinoDevice::ConfigureEspressifESP32()
	{
		// Using a 'DOIT ESP32 DEVKIT V4' for development.
		Log::LogInfo(PRM("ConfigureEspressifESP32"));

		// Analog Inputs (ADC pins).
		AddPart("ai0", ARDJACK_PART_TYPE_ANALOG_INPUT, 0, A0);
		AddPart("ai1", ARDJACK_PART_TYPE_ANALOG_INPUT, 0, A3);
		AddPart("ai2", ARDJACK_PART_TYPE_ANALOG_INPUT, 0, A4);
		AddPart("ai3", ARDJACK_PART_TYPE_ANALOG_INPUT, 0, A5);

		// Analog Outputs (DAC pins).
		AddPart("ao0", ARDJACK_PART_TYPE_ANALOG_OUTPUT, 0, DAC1);
		AddPart("ao1", ARDJACK_PART_TYPE_ANALOG_OUTPUT, 0, DAC2);

		// Digital Inputs.
#ifdef ARDJACK_INCLUDE_ARDUINO_DHT
		// Keep pin 2 for DHT.
		AddParts("di", 2, ARDJACK_PART_TYPE_DIGITAL_INPUT, 0, 0, 0);
		AddParts("di", 2, ARDJACK_PART_TYPE_DIGITAL_INPUT, 0, 2, 3);
		//AddParts("di", 1, ARDJACK_PART_TYPE_DIGITAL_INPUT, 0, 5, 9);
#else
		// Keep pin 2 for the builtin LED.
		AddParts("di", 2, ARDJACK_PART_TYPE_DIGITAL_INPUT, 0, 0, 0);
		AddParts("di", 2, ARDJACK_PART_TYPE_DIGITAL_INPUT, 0, 2, 3);
		//AddParts("di", 1, ARDJACK_PART_TYPE_DIGITAL_INPUT, 0, 6, 9);
#endif

		// Digital Outputs.
		AddParts("do", 4, ARDJACK_PART_TYPE_DIGITAL_OUTPUT, 0, 0, 10);

		// LEDs.
		//AddPart("led0", ARDJACK_PART_TYPE_LED, 0, 23);				// LED_BUILTIN);

#ifdef ARDJACK_INCLUDE_ARDUINO_DHT
		// DHT.
		// Using pin 2 for DHT.
		AddPart("dht0", ARDJACK_PART_TYPE_USER, ARDJACK_USERPART_SUBTYPE_ARDUINO_DHT, 2);
#endif

		return true;
	}
#endif


#ifdef ARDJACK_FEATHER_M0
	bool ArduinoDevice::ConfigureFeatherM0()
	{
		// HANGS HERE?
		Log::LogInfo(PRM("ConfigureFeatherM0"));

		// Analog Inputs (ADC pins).
		AddParts("ai", 5, ARDJACK_PART_TYPE_ANALOG_INPUT, 0, 0, PIN_A1);

		// Analog Outputs (DAC pins).
		AddPart("ao0", ARDJACK_PART_TYPE_ANALOG_OUTPUT, 0, PIN_A0);

		// Digital Inputs.
#ifdef ARDJACK_INCLUDE_ARDUINO_DHT
		// Keep pin 2 for DHT.
		AddParts("di", 2, ARDJACK_PART_TYPE_DIGITAL_INPUT, 0, 0, 0);
		AddParts("di", 3, ARDJACK_PART_TYPE_DIGITAL_INPUT, 0, 2, 3);
		//AddParts("di", 1, ARDJACK_PART_TYPE_DIGITAL_INPUT, 0, 5, 9);
#else
		AddParts("di", 6, ARDJACK_PART_TYPE_DIGITAL_INPUT, 0, 0, 0);
		//AddParts("di", 1, ARDJACK_PART_TYPE_DIGITAL_INPUT, 0, 6, 9);
#endif

		// Digital Outputs.
		AddParts("do", 3, ARDJACK_PART_TYPE_DIGITAL_OUTPUT, 0, 0, 10);

		// LEDs.
		AddPart("led0", ARDJACK_PART_TYPE_LED, 0, LED_BUILTIN);						// "red LED next to the USB jack"

#ifdef ARDJACK_INCLUDE_ARDUINO_DHT
		// DHT.
		AddPart("dht0", ARDJACK_PART_TYPE_USER, ARDJACK_USERPART_SUBTYPE_ARDUINO_DHT, 2);
#endif

		return true;
	}
#endif


bool ArduinoDevice::CreateDefaultInventory()
{
	// Configure for the board.
	// N.B. The board-dependent configuration below is fixed at COMPILE time, as the Arduino model is known then,
	// whereas SHIELD configuration may be done any number of times at RUNTIME (as the User can change the 'Shield' option).
	
	// If a Shield is removed, this method needs to be called to re-establish the boards' configuration.

	PartCount = 0;

#ifdef ARDJACK_ARDUINO_DUE
	ConfigureDUE();
#endif

#ifdef ARDJACK_ARDUINO_MEGA2560
	ConfigureMega2560();
#endif

#ifdef ARDJACK_ARDUINO_MKR
	ConfigureArduinoMKR();
#endif

#ifdef ARDJACK_ARDUINO_SAMD_ZERO
	ConfigureSparkFunRedBoardTurbo();
#endif

#ifdef ARDJACK_ARDUINO_UNO
	ConfigureUno();
#endif

#ifdef ARDJACK_ESP32
	ConfigureEspressifESP32();
#endif

#ifdef ARDJACK_FEATHER_M0
	ConfigureFeatherM0();
#endif

	return true;
}


bool ArduinoDevice::Read(Part* part, Dynamic* value)
{
	if (Globals::Verbosity > 5)
		Log::LogInfoF(PRM("Read: '%s', part '%s'"), Name, part->Name);

	value->Clear();

#ifdef ARDJACK_INCLUDE_SHIELDS
	// Using a shield?
	if (NULL != DeviceShield)
	{
		bool handled = false;

		// Offer this Read to the Shield.
		if (!DeviceShield->ReadPart(part, value, &handled))
			return false;

		if (handled)
			return true;
	}
#endif

	switch (part->Type)
	{
		case ARDJACK_PART_TYPE_ANALOG_INPUT:
		case ARDJACK_PART_TYPE_HUMIDITY_SENSOR:
		case ARDJACK_PART_TYPE_LIGHT_SENSOR:
		case ARDJACK_PART_TYPE_POTENTIOMETER:
		case ARDJACK_PART_TYPE_TEMPERATURE_SENSOR:
			if (!ReadAnalog(part, value))
				return false;
			break;

		case ARDJACK_PART_TYPE_BUTTON:
		case ARDJACK_PART_TYPE_DIGITAL_INPUT:
		case ARDJACK_PART_TYPE_SWITCH:
			if (!ReadDigital(part, value))
				return false;
			break;

#ifdef ARDJACK_INCLUDE_MULTI_PARTS
		case ARDJACK_PART_TYPE_MULTI:
			if (!ReadMulti(part, value))
				return false;
			break;
#endif

		case ARDJACK_PART_TYPE_USER:
			{
				UserPart* userPart = (UserPart*)part;

				if (!userPart->Read(value))
					return false;
			}
			break;

		default:
			value->Copy(&part->Value);
			return true;
	}

	if (!value->IsEmpty())
		part->Value.Copy(value);

	return true;
}


bool ArduinoDevice::ReadAnalog(Part* pPart, Dynamic* value)
{
	int intValue = analogRead(pPart->Pin);

	value->SetInt(intValue);

	return true;
}


bool ArduinoDevice::ReadDigital(Part* part, Dynamic* value)
{
	int intVal = digitalRead(part->Pin);
	bool state = (intVal == HIGH);

	value->SetBool(state);

	return true;
}


bool ArduinoDevice::Write(Part* part, Dynamic* value)
{
	if (Globals::Verbosity > 5)
	{
		if (value->IsEmpty())
			Log::LogInfoF(PRM("ArduinoDevice::Write: '%s', part '%s', no value"), Name, part->Name);
		else
		{
			char temp[ARDJACK_MAX_PART_VALUE_LENGTH];
			value->AsString(temp);

			Log::LogInfoF(PRM("ArduinoDevice::Write: '%s', part '%s', value '%s'"), Name, part->Name, temp);
		}
	}

#ifdef ARDJACK_INCLUDE_SHIELDS
	// Using a shield?
	if (NULL != DeviceShield)
	{
		bool handled = false;

		// Offer this Write to the Shield.
		if (!DeviceShield->WritePart(part, value, &handled))
			return false;

		if (handled)
			return true;
	}
#endif

	switch (part->Type)
	{
		case ARDJACK_PART_TYPE_ANALOG_OUTPUT:
			return WriteAnalog(part, value);

		case ARDJACK_PART_TYPE_DIGITAL_OUTPUT:
		case ARDJACK_PART_TYPE_LED:
			return WriteDigital(part, value);

#ifdef ARDJACK_INCLUDE_MULTI_PARTS
		case ARDJACK_PART_TYPE_MULTI:
			return WriteMulti(part, value);
#endif

		case ARDJACK_PART_TYPE_SOUND:
			// TEMPORARY:
			return WriteDigital(part, value);

		case ARDJACK_PART_TYPE_USER:
			UserPart* userPart = (UserPart*)part;
			return userPart->Write(value);
	}

	return false;
}


bool ArduinoDevice::WriteAnalog(Part* part, Dynamic* value)
{
	if (value->IsEmpty())
		return true;

#ifdef ARDJACK_ESP32
	return false;
#else
	int intVal = value->AsInt();

	if (Globals::Verbosity > 4)
		Log::LogInfoF(PRM("'%s': Writing %d to '%s', pin %d"), Name, intVal, part->Name, part->Pin);

	analogWrite(part->Pin, intVal);
	part->Value.SetInt(intVal);
#endif

	return true;
}


bool ArduinoDevice::WriteDigital(Part* part, Dynamic* value)
{
	bool newState = false;
	char temp[120];

	if (value->IsEmpty())
	{
		// No value supplied - toggle the state.
		if (Globals::Verbosity > 5)
			Log::LogInfoF(PRM("'%s': Toggling '%s', pin %d"), Name, part->Name, part->Pin);

		bool curState = part->Value.AsBool();
		newState = !curState;
	}
	else
		newState = value->AsBool();

	if (Globals::Verbosity > 4)
		Log::LogInfoF(PRM("'%s': Writing %d to '%s', pin %d"), Name, newState, part->Name, part->Pin);

	digitalWrite(part->Pin, newState ? HIGH : LOW);
	part->Value.SetBool(newState);

	return true;
}

#endif
