/*
	FilterManager.cpp

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


#include "Filter.h"
#include "FilterManager.h"
#include "IoTManager.h"
#include "Log.h"
#include "Utils.h"




FilterManager::FilterManager()
	: IoTManager()
{
	if (Globals::Verbosity > 7)
		Log::LogInfo(PRM("FilterManager ctor"));

	ObjectType = ARDJACK_OBJECT_TYPE_FILTER;
}


FilterManager::~FilterManager()
{
}


bool FilterManager::Interact(const char* text)
{
	// E.g.

	if ((NULL == text) || (strlen(text) == 0))
		return true;

	StringList fields;

	int count = Utils::SplitText(text, ' ', &fields, ARDJACK_MAX_VALUES, ARDJACK_MAX_VALUE_LENGTH);
	if (count < 2)
		return false;

	// Get the Filter.
	char name[ARDJACK_MAX_VALUE_LENGTH];
	strcpy(name, fields.Get(0));

	Filter* filter = LookupFilter(name);
	if (NULL == filter)
	{
		Log::LogError(PRM("No such Filter: "), name);
		return false;
	}

	// Get the action.
	char action[ARDJACK_MAX_VALUE_LENGTH];
	strcpy(action, fields.Get(1));
	_strupr(action);

	//if (Utils::StringEquals(action, "HELP", true))
	//{
	//	Log::LogInfo(PRM("HELP"));
	//	Log::LogInfo(PRM("----"));

	//	Log::LogInfo(PRM("   net ni0 DNS hostname    Get the IP address for 'hostname'."));
	//	Log::LogInfo(PRM("(where 'ni0' is a Filter interface.)"));

	//	Log::LogInfo();
	//	Log::LogInfo(PRM("Examples"));

	//	Log::LogInfo(PRM("   net wifi0 dns 1and1.co.uk"));

	//	return true;
	//}

	//if (Utils::StringEquals(action, "PING", true))
	//{
	//	if (count < 3)
	//	{
	//		Log::LogError(PRM("Insufficient arguments for NET PING: "), text);
	//		return false;
	//	}

	//	return netInterface->Ping(fields.Get(2));
	//}

	//if (Utils::StringEquals(action, "SCAN", true))
	//{
	//	return netInterface->ScanWiFi();
	//}

	//if (Utils::StringEquals(action, "SPECIAL", true))
	//{
	//	int action = Utils::String2Int(fields.Get(2));
	//	netInterface->Special(action);

	//	return true;
	//}

	//if (Utils::StringEquals(action, "STATUS", true))
	//{
	//	netInterface->LogStatus();

	//	return true;
	//}

	//if (Utils::StringEquals(action, "TIME", true))
	//{
	//	DateTime dt;
	//	netInterface->GetFilterTime(&dt);

	//	char temp[42];

	//	Utils::TimeToString(&dt, temp);
	//	Log::LogInfo(PRM("WiFi time: "), temp);

	//	Utils::Now(&dt);
	//	Utils::TimeToString(&dt, temp);
	//	Log::LogInfo(PRM("RTC time: "), temp);

	//	return true;
	//}

	Log::LogError(PRM("Invalid Filter command: "), text);

	return false;
}


Filter* FilterManager::LookupFilter(const char* name)
{
	IoTObject* obj = Globals::ObjectRegister->LookupName(name);
	if (NULL == obj) return NULL;
	if (obj->Type != ARDJACK_OBJECT_TYPE_FILTER) return NULL;

	return (Filter*)obj;
}

