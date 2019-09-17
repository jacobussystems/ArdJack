/*
	BeaconManager.cpp

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
#else
	#include "stdafx.h"
#endif

#include "Beacon.h"
#include "BeaconManager.h"
#include "ConnectionManager.h"
#include "Globals.h"
#include "Log.h"
#include "Utils.h"



#ifdef ARDJACK_INCLUDE_BEACONS

BeaconManager::BeaconManager()
	: IoTManager()
{
	if (Globals::Verbosity > 7)
		Log::LogInfo(PRM("BeaconManager ctor"));

	ObjectType = ARDJACK_OBJECT_TYPE_BEACON;
}


BeaconManager::~BeaconManager()
{
}


Beacon* BeaconManager::LookupBeacon(const char* name)
{
	IoTObject* obj = Globals::ObjectRegister->LookupName(name);
	if (NULL == obj) return NULL;

	if (obj->Type != ARDJACK_OBJECT_TYPE_BEACON) return NULL;

	return (Beacon*)obj;
}


//int BeaconManager::LookupBeaconObj(Beacon* Beacon)
//{
//	Beacon* r;
//
//	for (int i = 0; i < BeaconCount; i++)
//	{
//		r = Beacons[i];
//
//		if (r == Beacon)
//			return i;
//	}
//
//	return -1;
//}


bool BeaconManager::RemoveBeacon(Beacon* Beacon)
{

	return true;
}

#endif
