/*
	NetworkManager.cpp

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



#ifdef ARDJACK_NETWORK_AVAILABLE

#include "Enumeration.h"
#include "IoTManager.h"
#include "Log.h"
#include "NetworkInterface.h"
#include "NetworkManager.h"
#include "Utils.h"



NetworkManager::NetworkManager()
	: IoTManager()
{
	if (Globals::Verbosity > 7)
		Log::LogInfo(PRM("NetworkManager ctor"));

	ObjectType = ARDJACK_OBJECT_TYPE_NETWORKINTERFACE;

	if (NULL == Subtypes)
	{
		Subtypes = new Enumeration(PRM("Subtypes"));

#ifdef ARDUINO
	#ifdef ARDJACK_ETHERNET_AVAILABLE
		Subtypes->Add(PRM("ETHERNET"), ARDJACK_NETWORK_SUBTYPE_ETHERNET);
	#endif

	#ifdef ARDJACK_WIFI_AVAILABLE
		Subtypes->Add(PRM("WIFI"), ARDJACK_NETWORK_SUBTYPE_WIFI);
	#endif
#endif
	}
}


NetworkManager::~NetworkManager()
{
}


bool NetworkManager::Interact(const char* text)
{
	// E.g.
	//		wifi0 dns jacobus.co.uk
	//		wifi0 ping jacobus.co.uk
	//		wifi0 scan
	//		wifi0 special
	//		wifi0 status
	//		wifi0 time

	if (Utils::StringIsNullOrEmpty(text))
		return true;

	StringList fields;

	int count = Utils::SplitText(text, ' ', &fields, ARDJACK_MAX_VALUES, ARDJACK_MAX_VALUE_LENGTH);
	if (count < 2)
		return false;

	// Get the Network Interface.
	char netName[ARDJACK_MAX_VALUE_LENGTH];
	strcpy(netName, fields.Get(0));

	NetworkInterface *netInterface = LookupNetwork(netName);
	if (NULL == netInterface)
	{
		Log::LogError(PRM("No such Network Interface: "), netName);
		return false;
	}

	// Get the action.
	char action[ARDJACK_MAX_VALUE_LENGTH];
	strcpy(action, fields.Get(1));
	_strupr(action);

#ifdef ARDUINO
#ifdef ARDJACK_NETWORK_AVAILABLE
	if (Utils::StringEquals(action, PRM("DNS"), true))
	{
		if (count < 3)
		{
			Log::LogError(PRM("Insufficient arguments for NET DNS: "), text);
			return false;
		}

		IPAddress ipAddress;
		netInterface->LookupHost(fields.Get(2), ipAddress);

		char work[20];
		Log::LogInfoF(PRM("NET DNS: Host '%s' -> %s"), fields.Get(2), Utils::IpAddressToString(ipAddress, work));

		return true;
	}
#endif
#endif

	if (Utils::StringEquals(action, PRM("HELP"), true))
	{
		Log::LogInfo(PRM("HELP"));
		Log::LogInfo(PRM("----"));

		Log::LogInfo(PRM("   net ni0 DNS hostname    Get the IP address for 'hostname'."));
		Log::LogInfo(PRM("   net ni0 HELP            Display this help."));
		Log::LogInfo(PRM("   net ni0 PING hostname   Ping 'hostname'."));
		Log::LogInfo(PRM("   net ni0 SCAN            Scan the WiFi network, if 'ni0' is a WiFi interface."));
		Log::LogInfo(PRM("   net ni0 STATUS          Show the status of 'ni0', e.g. WiFi info."));
		Log::LogInfo(PRM("   net ni0 TIME            Get the network time."));
		Log::LogInfo(PRM("(where 'ni0' is a network interface.)"));

		Log::LogInfo();
		Log::LogInfo(PRM("Examples"));

		Log::LogInfo(PRM("   net wifi0 dns 1and1.co.uk"));
		Log::LogInfo(PRM("   net wifi0 help"));
		Log::LogInfo(PRM("   net wifi0 ping 1and1.co.uk"));
		Log::LogInfo(PRM("   net wifi0 scan"));
		Log::LogInfo(PRM("   net wifi0 status"));
		Log::LogInfo(PRM("   net wifi0 time"));

		return true;
	}

	if (Utils::StringEquals(action, PRM("PING"), true))
	{
		if (count < 3)
		{
			Log::LogError(PRM("Insufficient arguments for NET PING: "), text);
			return false;
		}

		return netInterface->Ping(fields.Get(2));
	}

	if (Utils::StringEquals(action, PRM("SCAN"), true))
	{
		return netInterface->ScanWiFi();
	}

	if (Utils::StringEquals(action, PRM("SPECIAL"), true))
	{
		int action = Utils::String2Int(fields.Get(2));
		netInterface->Special(action);

		return true;
	}

	if (Utils::StringEquals(action, PRM("STATUS"), true))
	{
		netInterface->LogStatus();

		return true;
	}

	if (Utils::StringEquals(action, PRM("TIME"), true))
	{
		DateTime dt;
		netInterface->GetNetworkTime(&dt);

		char temp[42];

		Utils::TimeToString(&dt, temp);
		Log::LogInfo(PRM("WiFi time: "), temp);

		Utils::Now(&dt);
		Utils::TimeToString(&dt, temp);
		Log::LogInfo(PRM("RTC time: "), temp);

		return true;
	}

	Log::LogError(PRM("Invalid Network command: "), text);

	return false;
}


NetworkInterface* NetworkManager::LookupNetwork(const char* name)
{
	IoTObject* obj = Globals::ObjectRegister->LookupName(name);
	if (NULL == obj) return NULL;
	if (obj->Type != ARDJACK_OBJECT_TYPE_NETWORKINTERFACE) return NULL;

	return (NetworkInterface*)obj;
}


int NetworkManager::LookupSubtype(const char* name)
{
	return Subtypes->LookupName(name, ARDJACK_NETWORK_SUBTYPE_UNKNOWN);
}

#endif
