/*
	IoTManager.cpp

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

#include "Enumeration.h"
#include "Globals.h"
#include "IoTManager.h"
#include "IoTObject.h"
#include "Log.h"




IoTManager::IoTManager()
{
	if (Globals::Verbosity > 7)
		Log::LogInfo(PRM("IoTManager ctor"));

	ObjectType = ARDJACK_OBJECT_TYPE_UNKNOWN;
	Subtypes = NULL;

	_PollBusy = false;
}


IoTManager::~IoTManager()
{
	if (NULL != Subtypes)
	{
		delete Subtypes;
		Subtypes = NULL;
	}
}


bool IoTManager::Interact(const char* text)
{
	return false;
}


int IoTManager::LookupSubtype(const char* name)
{
	return ARDJACK_OBJECT_SUBTYPE_BASIC;
}


void IoTManager::Poll()
{
	if (_PollBusy) return;

	_PollBusy = true;

	if (ObjectType != ARDJACK_OBJECT_TYPE_UNKNOWN)
		PollObjectsOfType(ObjectType);

	_PollBusy = false;
}


void IoTManager::PollObjectsOfType(int type)
{
	// Poll the active objects of type 'type'.
	//if (Globals::Verbosity > 5)
	//	Log::LogInfoF(PRM("IoTManager::PollObjectsOfType: type %d"), type);

	for (int i = 0; i < Globals::ObjectRegister->ObjectCount; i++)
	{
		IoTObject* obj = Globals::ObjectRegister->Objects[i];

		if ((obj->Type == type) && obj->Active())
			obj->Poll();
	}
}


bool IoTManager::Remove(IoTObject* obj)
{
	return false;
}

