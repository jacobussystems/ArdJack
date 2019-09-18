/*
	VellermanK8055Device.cpp

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

// Windows only.

#ifdef ARDUINO
#else

#include "stdafx.h"

#include "DeviceCodec1.h"
#include "Dynamic.h"
#include "Log.h"
#include "Part.h"
#include "Utils.h"
#include "VellemanK8055Device.h"



VellemanK8055Device::VellemanK8055Device(const char* name)
	: Device(name)
{
	_hDLL = NULL;
	CreateDefaultInventory();

	//if (NULL == _hDLL)
	//{
		char dllName[] = "k8055d";
		Log::LogInfo(PRM("VellemanK8055Device::ctor: '"), Name, "', DLL '", dllName, "'");

		_hDLL = LoadLibrary(dllName);

		if (NULL == _hDLL)
		{
			Log::LogError(PRM("DLL not found: "), dllName);
			return;
		}

		if (!InitDLL())
		{
			Log::LogError(PRM("Library initialization failed: "), dllName);
			FreeLibrary(_hDLL);
			_hDLL = NULL;
			return;
		}
	//}
}


VellemanK8055Device::~VellemanK8055Device()
{
	if (NULL != _hDLL)
	{
		FreeLibrary(_hDLL);
		_hDLL = NULL;
	}
}


bool VellemanK8055Device::CreateDefaultInventory()
{
	char temp[10];

	for (int i = 0; i < 2; i++)
	{
		sprintf(temp, "ai%d", i);
		AddPart(temp, ARDJACK_PART_TYPE_ANALOG_INPUT, i, i + 1);
	}

	for (int i = 0; i < 2; i++)
	{
		sprintf(temp, "ao%d", i);
		AddPart(temp, ARDJACK_PART_TYPE_ANALOG_OUTPUT, i, i + 1);
	}

	for (int i = 0; i < 8; i++)
	{
		sprintf(temp, "led%d", i);
		AddPart(temp, ARDJACK_PART_TYPE_LED, i, i + 1);
	}

	for (int i = 0; i < 5; i++)
	{
		sprintf(temp, "button%d", i);
		AddPart(temp, ARDJACK_PART_TYPE_BUTTON, i, i + 1);
	}

	return true;
}


bool VellemanK8055Device::InitDLL()
{
	Log::LogInfo(PRM("VellemanK8055Device::InitDLL: '"), Name, "'");

	Velleman_OpenDevice = (t_func4)GetProcAddress(_hDLL, "OpenDevice");
	if (!Velleman_OpenDevice)
		return false;

	Velleman_CloseDevice = (t_func0)GetProcAddress(_hDLL, "CloseDevice");
	if (!Velleman_CloseDevice)
		return false;

	Velleman_ReadAnalogChannel = (t_func4)GetProcAddress(_hDLL, "ReadAnalogChannel");
	if (!Velleman_ReadAnalogChannel)
		return false;

	Velleman_ReadAllAnalog = (t_func2)GetProcAddress(_hDLL, "ReadAllAnalog");
	if (!Velleman_ReadAllAnalog)
		return false;

	Velleman_OutputAnalogChannel = (t_func3)GetProcAddress(_hDLL, "OutputAnalogChannel");
	if (!Velleman_OutputAnalogChannel)
		return false;

	Velleman_OutputAllAnalog = (t_func3)GetProcAddress(_hDLL, "OutputAllAnalog");
	if (!Velleman_OutputAllAnalog)
		return false;

	Velleman_ClearAnalogChannel = (t_func)GetProcAddress(_hDLL, "ClearAnalogChannel");
	if (!Velleman_ClearAnalogChannel)
		return false;

	Velleman_ClearAllAnalog = (t_func0)GetProcAddress(_hDLL, "ClearAllAnalog");
	if (!Velleman_ClearAllAnalog)
		return false;

	Velleman_SetAnalogChannel = (t_func)GetProcAddress(_hDLL, "SetAnalogChannel");
	if (!Velleman_SetAnalogChannel)
		return false;

	Velleman_SetAllAnalog = (t_func0)GetProcAddress(_hDLL, "SetAllAnalog");
	if (!Velleman_SetAllAnalog)
		return false;

	Velleman_WriteAllDigital = (t_func)GetProcAddress(_hDLL, "WriteAllDigital");
	if (!Velleman_WriteAllDigital)
		return false;

	Velleman_ClearDigitalChannel = (t_func)GetProcAddress(_hDLL, "ClearDigitalChannel");
	if (!Velleman_ClearDigitalChannel)
		return false;

	Velleman_ClearAllDigital = (t_func0)GetProcAddress(_hDLL, "ClearAllDigital");
	if (!Velleman_ClearAllDigital)
		return false;

	Velleman_SetDigitalChannel = (t_func)GetProcAddress(_hDLL, "SetDigitalChannel");
	if (!Velleman_SetDigitalChannel)
		return false;

	Velleman_SetAllDigital = (t_func0)GetProcAddress(_hDLL, "SetAllDigital");
	if (!Velleman_SetAllDigital)
		return false;

	Velleman_ReadDigitalChannel = (t_func5)GetProcAddress(_hDLL, "ReadDigitalChannel");
	if (!Velleman_ReadDigitalChannel)
		return false;

	Velleman_ReadAllDigital = (t_func1)GetProcAddress(_hDLL, "ReadAllDigital");
	if (!Velleman_ReadAllDigital)
		return false;

	Velleman_Version = (t_func0)GetProcAddress(_hDLL, "Version");
	if (!Velleman_Version)
		return false;

	return true;
}


bool VellemanK8055Device::Open()
{
	int _Address = 0;
	Log::LogInfoF(PRM("VellemanK8055Device::Open: '%s' at address %d"), Name, _Address);

	int h = Velleman_OpenDevice(_Address);
	if (h != 0)
	{
		Log::LogErrorF(PRM("VellemanK8055Device::Open: '%s' No board found at address %d"), Name, _Address);
		return false;
	}

	if (!Device::Open())
		return false;

	return true;
}


bool VellemanK8055Device::Read(Part* part, Dynamic* value)
{
	value->Clear();

	switch (part->Type)
	{
	case ARDJACK_PART_TYPE_ANALOG_INPUT:
	case ARDJACK_PART_TYPE_POTENTIOMETER:
		if (!ReadAnalog(part, value))
			return false;
		break;

	case ARDJACK_PART_TYPE_BUTTON:
	case ARDJACK_PART_TYPE_DIGITAL_INPUT:
		if (!ReadDigital(part, value))
			return false;
		break;

	default:
		value->Copy(&part->Value);
		return true;
	}

	if (!value->IsEmpty())
		part->Value.Copy(value);

	return true;
}


bool VellemanK8055Device::ReadAnalog(Part* part, Dynamic* value)
{
	return value->SetInt(Velleman_ReadAnalogChannel(part->Pin));
}


bool VellemanK8055Device::ReadDigital(Part* part, Dynamic* value)
{
	//return value->SetBool(false);

	return value->SetBool(Velleman_ReadDigitalChannel(part->Pin));

	//int pinValue = Velleman_ReadDigitalChannel(part->Pin);

	//return value->SetBool(pinValue != 0);
}


bool VellemanK8055Device::Write(Part* part, Dynamic* value)
{
	if (Globals::Verbosity > 4)
	{
		if (value->IsEmpty())
			Log::LogInfoF("VellemanK8055Device::Write: '%s', part '%s', no value", Name, part->Name);
		else
		{
			char temp[ARDJACK_MAX_PART_VALUE_LENGTH];
			value->AsString(temp);
			Log::LogInfoF("VellemanK8055Device::Write: '%s', part '%s', value '%s'", Name, part->Name, temp);
		}
	}

	switch (part->Type)
	{
	case ARDJACK_PART_TYPE_ANALOG_OUTPUT:
		return WriteAnalog(part, value);

	case ARDJACK_PART_TYPE_DIGITAL_OUTPUT:
	case ARDJACK_PART_TYPE_LED:
		return WriteDigital(part, value);
	}

	return false;
}


bool VellemanK8055Device::WriteAnalog(Part* part, Dynamic* value)
{
	if (value->IsEmpty())
		return true;

	int intValue = value->AsInt();

	if (Globals::Verbosity > 4)
		Log::LogInfoF(PRM("%s: Writing %d to '%s' (pin %d)"), Name, intValue, part->Name, part->Pin);

	Velleman_OutputAnalogChannel(part->Pin, intValue);

	return part->Value.SetInt(intValue);
}


bool VellemanK8055Device::WriteDigital(Part* part, Dynamic* value)
{
	bool newState = false;
	char temp[120];

	if (value->IsEmpty())
	{
		// No value supplied - toggle the state.
		if (Globals::Verbosity > 5)
			Log::LogInfoF(PRM("%s: Toggling '%s' (pin %d)"), Name, part->Name, part->Pin);

		bool curState = part->Value.AsBool();
		newState = !curState;
	}
	else
		newState = value->AsBool();

	if (Globals::Verbosity > 4)
	{
		sprintf(temp, PRM("%s: Writing %d to '%s' (pin %d)"), Name, newState, part->Name, part->Pin);
		Log::LogInfo(temp);
	}

	if (newState)
		Velleman_SetDigitalChannel(part->Pin);
	else
		Velleman_ClearDigitalChannel(part->Pin);

	part->Value.SetBool(newState);

	return true;
}

#endif
