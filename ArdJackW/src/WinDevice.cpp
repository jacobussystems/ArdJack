/*
	WinDevice.cpp

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

#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")

#include "DeviceCodec1.h"
#include "Dynamic.h"
#include "Log.h"
#include "Part.h"
#include "Utils.h"
#include "WinDevice.h"



WinDevice::WinDevice(const char* name)
	: Device(name)
{
	CreateDefaultInventory();
}


WinDevice::~WinDevice()
{
}


bool WinDevice::CreateDefaultInventory()
{
	// Add Parts.
	char name[10];

	for (int i = 0; i < 8; i++)
	{
		strcpy(name, "disk");
		name[4] = 'A' + i;
		name[5] = NULL;
		AddPart(name, ARDJACK_PART_TYPE_USER, ARDJACK_USERPART_SUBTYPE_WINDISK, ARDJACK_WINDISK_DRIVE_A + i);
	}

	AddPart("memFreePhys", ARDJACK_PART_TYPE_USER, ARDJACK_USERPART_SUBTYPE_WINMEMORY, ARDJACK_WINMEMORY_FREE_PHYSICAL);
	AddPart("memTotalPhys", ARDJACK_PART_TYPE_USER, ARDJACK_USERPART_SUBTYPE_WINMEMORY, ARDJACK_WINMEMORY_TOTAL_PHYSICAL);

	// ? Strange results from:
	//AddPart("memFreeVirt", ARDJACK_PART_TYPE_USER, ARDJACK_USERPART_SUBTYPE_WINMEMORY, ARDJACK_WINMEMORY_FREE_VIRTUAL);

	return true;
}


bool WinDevice::DoBeep(int index, int freq_hz, int dur_ms)
{
	return Utils::DoBeep(freq_hz, dur_ms);
}


bool WinDevice::Open()
{
#ifdef ARDJACK_INCLUDE_SHIELDS
	Log::LogInfo(PRM("Opening Device '"), Name, "', shield '", _ShieldName, "'");
#else
	Log::LogInfo(PRM("Opening Device '"), Name, "'");
#endif

	if (!Device::Open())
		return false;

	return true;
}


bool WinDevice::SendResponse(int oper, const char* aName, const char* text, StringList* values)
{
	char response[120];
	DeviceCodec->EncodeResponse(response, oper, aName, text, values);

	Log::LogInfo("'", Name, PRM("': "), response);

	return true;
}


bool WinDevice::Write(Part* part, Dynamic* value)
{
	return value->Copy(&part->Value);
}

#endif

