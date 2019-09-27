/*
	PartManager.cpp

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

#include "Globals.h"

#ifdef ARDUINO
	#define _strlwr strlwr
#else
	#include "stdafx.h"
	#include <typeinfo>
#endif

#ifdef ARDUINO
	#ifdef ARDJACK_INCLUDE_ARDUINO_DHT
		#include "ArduinoDHT.h"
	#endif

	#ifdef ARDJACK_INCLUDE_ARDUINO_NEOPIXEL
		#include "ArduinoNeoPixel.h"
	#endif

	#ifdef ARDJACK_INCLUDE_SHIELDS
		#ifdef ARDJACK_INCLUDE_ARDUINO_MF_SHIELD
			#include "ArduinoMFShield.h"
		#endif
	#endif
#else
	#ifdef ARDJACK_INCLUDE_WINDISK
		#include "WinDisk.h"
	#endif

	#ifdef ARDJACK_INCLUDE_WINMEMORY
		#include "WinMemory.h"
	#endif
#endif

#include "Enumeration.h"
#include "IoTObject.h"
#include "Log.h"
#include "Part.h"
#include "PartManager.h"
#include "UserPart.h"
#include "Utils.h"


Enumeration* PartManager::PartSubtypes = NULL;
Enumeration* PartManager::PartTypes = NULL;



PartManager::PartManager()
{
	if (Globals::Verbosity > 7)
		Log::LogInfo(PRM("PartManager ctor"));

	if (NULL == PartTypes)
	{
		PartTypes = new Enumeration(PRM("Part Types"));

		// Abbreviations should be first here, e.g. "AI" before "AnalogInput".
		PartTypes->Add(PRM("Accel"), ARDJACK_PART_TYPE_ACCELEROMETER);
		PartTypes->Add(PRM("Accelerometer"), ARDJACK_PART_TYPE_ACCELEROMETER);
		PartTypes->Add(PRM("AI"), ARDJACK_PART_TYPE_ANALOG_INPUT);
		PartTypes->Add(PRM("AnalogInput"), ARDJACK_PART_TYPE_ANALOG_INPUT);
		PartTypes->Add(PRM("AO"), ARDJACK_PART_TYPE_ANALOG_OUTPUT);
		PartTypes->Add(PRM("AnalogOutput"), ARDJACK_PART_TYPE_ANALOG_OUTPUT);
		PartTypes->Add(PRM("Button"), ARDJACK_PART_TYPE_BUTTON);
		PartTypes->Add(PRM("Device"), ARDJACK_PART_TYPE_DEVICE);
		PartTypes->Add(PRM("DI"), ARDJACK_PART_TYPE_DIGITAL_INPUT);
		PartTypes->Add(PRM("DigitalInput"), ARDJACK_PART_TYPE_DIGITAL_INPUT);
		PartTypes->Add(PRM("DO"), ARDJACK_PART_TYPE_DIGITAL_OUTPUT);
		PartTypes->Add(PRM("DigitalOutput"), ARDJACK_PART_TYPE_DIGITAL_OUTPUT);
		PartTypes->Add(PRM("Disk"), ARDJACK_PART_TYPE_DISK_SPACE);
		PartTypes->Add(PRM("DiskSpace"), ARDJACK_PART_TYPE_DISK_SPACE);
		PartTypes->Add(PRM("Graph"), ARDJACK_PART_TYPE_GRAPHIC_DISPLAY);
		PartTypes->Add(PRM("GraphicDisplay"), ARDJACK_PART_TYPE_GRAPHIC_DISPLAY);
		PartTypes->Add(PRM("Gyro"), ARDJACK_PART_TYPE_GYROSCOPE);
		PartTypes->Add(PRM("Gyroscope"), ARDJACK_PART_TYPE_GYROSCOPE);
		PartTypes->Add(PRM("Hum"), ARDJACK_PART_TYPE_HUMIDITY_SENSOR);
		PartTypes->Add(PRM("HumiditySensor"), ARDJACK_PART_TYPE_HUMIDITY_SENSOR);
		PartTypes->Add(PRM("I2C"), ARDJACK_PART_TYPE_I2C);
		PartTypes->Add(PRM("Kbd"), ARDJACK_PART_TYPE_KEYBOARD);
		PartTypes->Add(PRM("Keyboard"), ARDJACK_PART_TYPE_KEYBOARD);
		PartTypes->Add(PRM("Kpd"), ARDJACK_PART_TYPE_KEYPAD);
		PartTypes->Add(PRM("Keypad"), ARDJACK_PART_TYPE_KEYPAD);
		PartTypes->Add(PRM("LDR"), ARDJACK_PART_TYPE_LIGHT_SENSOR);
		PartTypes->Add(PRM("LightSensor"), ARDJACK_PART_TYPE_LIGHT_SENSOR);
		PartTypes->Add(PRM("LED"), ARDJACK_PART_TYPE_LED);
		PartTypes->Add(PRM("Mag"), ARDJACK_PART_TYPE_MAGNETIC_SENSOR);
		PartTypes->Add(PRM("MagneticSensor"), ARDJACK_PART_TYPE_MAGNETIC_SENSOR);
		PartTypes->Add(PRM("Mem"), ARDJACK_PART_TYPE_MEMORY);
		PartTypes->Add(PRM("Memory"), ARDJACK_PART_TYPE_MEMORY);
		PartTypes->Add(PRM("Mouse"), ARDJACK_PART_TYPE_MOUSE);
		PartTypes->Add(PRM("Multi"), ARDJACK_PART_TYPE_MULTI);
		PartTypes->Add(PRM("None"), ARDJACK_PART_TYPE_NONE);
		PartTypes->Add(PRM("Orient"), ARDJACK_PART_TYPE_ORIENTATION);
		PartTypes->Add(PRM("Orientation"), ARDJACK_PART_TYPE_ORIENTATION);
		PartTypes->Add(PRM("Pot"), ARDJACK_PART_TYPE_POTENTIOMETER);
		PartTypes->Add(PRM("Potentiometer"), ARDJACK_PART_TYPE_POTENTIOMETER);
		PartTypes->Add(PRM("Press"), ARDJACK_PART_TYPE_PRESSURE_SENSOR);
		PartTypes->Add(PRM("PressureSensor"), ARDJACK_PART_TYPE_PRESSURE_SENSOR);
		PartTypes->Add(PRM("Prox"), ARDJACK_PART_TYPE_PROXIMITY_SENSOR);
		PartTypes->Add(PRM("ProximitySensor"), ARDJACK_PART_TYPE_PROXIMITY_SENSOR);
		PartTypes->Add(PRM("Sound"), ARDJACK_PART_TYPE_SOUND);
		PartTypes->Add(PRM("Special"), ARDJACK_PART_TYPE_SPECIAL);
		PartTypes->Add(PRM("Switch"), ARDJACK_PART_TYPE_SWITCH);
		PartTypes->Add(PRM("Temp"), ARDJACK_PART_TYPE_TEMPERATURE_SENSOR);
		PartTypes->Add(PRM("TemperatureSensor"), ARDJACK_PART_TYPE_TEMPERATURE_SENSOR);
		PartTypes->Add(PRM("Text"), ARDJACK_PART_TYPE_TEXT_DISPLAY);
		PartTypes->Add(PRM("TextDisplay"), ARDJACK_PART_TYPE_TEXT_DISPLAY);
		PartTypes->Add(PRM("TouchScreen"), ARDJACK_PART_TYPE_TOUCHSCREEN);
		PartTypes->Add(PRM("TTS"), ARDJACK_PART_TYPE_TTS);
		PartTypes->Add(PRM("User"), ARDJACK_PART_TYPE_USER);
	}

	if (NULL == PartSubtypes)
	{
		PartSubtypes = new Enumeration(PRM("UserPart Subtypes"));

#ifdef ARDUINO
	#ifdef ARDJACK_INCLUDE_ARDUINO_DHT
		PartSubtypes->Add(PRM("DHT"), ARDJACK_USERPART_SUBTYPE_ARDUINO_DHT);
	#endif

	#ifdef ARDJACK_INCLUDE_ARDUINO_NEOPIXEL
		PartSubtypes->Add(PRM("NeoPixel"), ARDJACK_USERPART_SUBTYPE_ARDUINO_NEOPIXEL);
	#endif
#else
	#ifdef ARDJACK_INCLUDE_WINDISK
		PartSubtypes->Add(PRM("WinDisk"), ARDJACK_USERPART_SUBTYPE_WINDISK);
	#endif

	#ifdef ARDJACK_INCLUDE_WINMEMORY
		PartSubtypes->Add(PRM("WinMemory"), ARDJACK_USERPART_SUBTYPE_WINMEMORY);
	#endif
#endif

		PartSubtypes->Add(PRM("None"), ARDJACK_USERPART_SUBTYPE_NONE);
	}
}


PartManager::~PartManager()
{
	if (NULL != PartTypes)
		delete PartTypes;

	if (NULL != PartSubtypes)
		delete PartSubtypes;
}


Part* PartManager::CreatePart(const char* name, int type, int subtype)
{
	Part* result = NULL;

	if (type != ARDJACK_PART_TYPE_USER)
		result = new Part();
	else
		result = CreateUserPart(name, type, subtype);

	char temp[202];

	if (NULL == result)
	{
		sprintf(temp, PRM("Unable to create Part '%s' (type '%s', subtype '%s')"),
			name, PartTypeName(type), PartSubtypeName(subtype));
		Log::LogError(temp);
		return NULL;
	}

	strcpy(result->Name, name);
	result->Type = type;
	result->Subtype = subtype;

	if (Globals::Verbosity > 5)
	{
		if (type == ARDJACK_PART_TYPE_USER)
		{
			sprintf(temp, PRM("Created UserPart '%s' (type '%s', subtype '%s')"),
				result->Name, PartTypeName(type), PartSubtypeName(subtype));
		}
		else
			sprintf(temp, PRM("Created Part '%s' (type '%s')"), result->Name, PartTypeName(type));

		Log::LogInfo(temp);
	}

	return result;
}


Part* PartManager::CreateUserPart(const char* name, int type, int subtype)
{
	Part* result = NULL;

#ifdef ARDUINO
	#ifdef ARDJACK_INCLUDE_ARDUINO_DHT
		if (subtype == ARDJACK_USERPART_SUBTYPE_ARDUINO_DHT)
		{
			result = new ArduinoDHT();
			return result;
		}
	#endif

	#ifdef ARDJACK_INCLUDE_ARDUINO_NEOPIXEL
		if (subtype == ARDJACK_USERPART_SUBTYPE_ARDUINO_NEOPIXEL)
		{
			result = new ArduinoNeoPixel();
			return result;
		}
	#endif
#else
	#ifdef ARDJACK_INCLUDE_WINDISK
		if (subtype == ARDJACK_USERPART_SUBTYPE_WINDISK)
			return new WinDisk();
	#endif

	#ifdef ARDJACK_INCLUDE_WINMEMORY
		if (subtype == ARDJACK_USERPART_SUBTYPE_WINMEMORY)
			return new WinMemory();
	#endif
#endif

		return NULL;
}


const char* PartManager::GetPartTypeName(int partType)
{
	// Get the standard abbreviation or name of a Part type.
	return PartTypes->LookupValue(partType);
}


bool PartManager::IsAnalogType(int type)
{
	switch (type)
	{
	case ARDJACK_PART_TYPE_ACCELEROMETER:
	case ARDJACK_PART_TYPE_ANALOG_INPUT:
	case ARDJACK_PART_TYPE_ANALOG_OUTPUT:
	case ARDJACK_PART_TYPE_DISK_SPACE:
	case ARDJACK_PART_TYPE_GYROSCOPE:
	case ARDJACK_PART_TYPE_HUMIDITY_SENSOR:
	case ARDJACK_PART_TYPE_LIGHT_SENSOR:
	case ARDJACK_PART_TYPE_MAGNETIC_SENSOR:
	case ARDJACK_PART_TYPE_MEMORY:
	case ARDJACK_PART_TYPE_ORIENTATION:
	case ARDJACK_PART_TYPE_POTENTIOMETER:
	case ARDJACK_PART_TYPE_PRESSURE_SENSOR:
	case ARDJACK_PART_TYPE_PROXIMITY_SENSOR:
	case ARDJACK_PART_TYPE_TEMPERATURE_SENSOR:
		return true;
	}

	return false;
}


bool PartManager::IsDigitalType(int type)
{
	switch (type)
	{
	case ARDJACK_PART_TYPE_BUTTON:
	case ARDJACK_PART_TYPE_DIGITAL_INPUT:
	case ARDJACK_PART_TYPE_DIGITAL_OUTPUT:
	case ARDJACK_PART_TYPE_KEYBOARD:
	case ARDJACK_PART_TYPE_KEYPAD:
	case ARDJACK_PART_TYPE_GRAPHIC_DISPLAY:
	case ARDJACK_PART_TYPE_MOUSE:
	case ARDJACK_PART_TYPE_I2C:
	case ARDJACK_PART_TYPE_LED:
	case ARDJACK_PART_TYPE_SOUND:
	case ARDJACK_PART_TYPE_SWITCH:
	case ARDJACK_PART_TYPE_TEXT_DISPLAY:
	case ARDJACK_PART_TYPE_TOUCHSCREEN:
	case ARDJACK_PART_TYPE_TTS:
		return true;
	}

	return false;
}


bool PartManager::IsInputType(int type)
{
	switch (type)
	{
	case ARDJACK_PART_TYPE_ACCELEROMETER:
	case ARDJACK_PART_TYPE_ANALOG_INPUT:
	case ARDJACK_PART_TYPE_BUTTON:
	case ARDJACK_PART_TYPE_DIGITAL_INPUT:
	case ARDJACK_PART_TYPE_DISK_SPACE:
	case ARDJACK_PART_TYPE_GYROSCOPE:
	case ARDJACK_PART_TYPE_HUMIDITY_SENSOR:
	case ARDJACK_PART_TYPE_KEYBOARD:
	case ARDJACK_PART_TYPE_KEYPAD:
	case ARDJACK_PART_TYPE_LIGHT_SENSOR:
	case ARDJACK_PART_TYPE_MAGNETIC_SENSOR:
	case ARDJACK_PART_TYPE_MEMORY:
	case ARDJACK_PART_TYPE_MOUSE:
	case ARDJACK_PART_TYPE_MULTI:
	case ARDJACK_PART_TYPE_ORIENTATION:
	case ARDJACK_PART_TYPE_POTENTIOMETER:
	case ARDJACK_PART_TYPE_PRESSURE_SENSOR:
	case ARDJACK_PART_TYPE_PROXIMITY_SENSOR:
	case ARDJACK_PART_TYPE_SWITCH:
	case ARDJACK_PART_TYPE_TEMPERATURE_SENSOR:
	case ARDJACK_PART_TYPE_TOUCHSCREEN:
		return true;
	}

	return false;
}


bool PartManager::IsOutputType(int type)
{
	switch (type)
	{
	case ARDJACK_PART_TYPE_ANALOG_OUTPUT:
	case ARDJACK_PART_TYPE_DIGITAL_OUTPUT:
	case ARDJACK_PART_TYPE_GRAPHIC_DISPLAY:
	case ARDJACK_PART_TYPE_I2C:
	case ARDJACK_PART_TYPE_LED:
	case ARDJACK_PART_TYPE_SOUND:
	case ARDJACK_PART_TYPE_TEXT_DISPLAY:
	case ARDJACK_PART_TYPE_TTS:
		return true;
	}

	return false;
}


bool PartManager::IsTextualType(int type)
{
	switch (type)
	{
	case ARDJACK_PART_TYPE_KEYBOARD:
	case ARDJACK_PART_TYPE_KEYPAD:
	case ARDJACK_PART_TYPE_GRAPHIC_DISPLAY:
	case ARDJACK_PART_TYPE_TEXT_DISPLAY:
	case ARDJACK_PART_TYPE_TOUCHSCREEN:
	case ARDJACK_PART_TYPE_TTS:
		return true;
	}

	return false;
}


int PartManager::LookupSubtype(const char* name, bool quiet)
{
	return PartSubtypes->LookupName(name, ARDJACK_USERPART_SUBTYPE_NONE, quiet);
}


int PartManager::LookupType(const char* name, bool quiet)
{
	return PartTypes->LookupName(name, ARDJACK_PART_TYPE_NONE, quiet);
}


const char* PartManager::PartSubtypeName(int subtype, const char* defaultName)
{
	return PartSubtypes->LookupValue(subtype, defaultName);
}


const char* PartManager::PartTypeName(int type, const char* defaultName)
{
	return PartTypes->LookupValue(type, defaultName);
}

