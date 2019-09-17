/*
	Register.cpp

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
	#include "DetectBoard.h"
#else
	#include "stdafx.h"
#endif

#include "ArduinoDevice.h"
#include "ArrayHelpers.h"
#include "Beacon.h"
#include "Bridge.h"
#include "Connection.h"
#include "ConnectionManager.h"
#include "DataLogger.h"
#include "Device.h"
#include "DeviceManager.h"
#include "Enumeration.h"
#include "FilterManager.h"
#include "Globals.h"
#include "IoTObject.h"
#include "Log.h"
#include "LogConnection.h"
#include "NetworkManager.h"
#include "Register.h"
#include "SerialConnection.h"
#include "ShieldManager.h"
#include "Utils.h"

#ifdef ARDUINO
	#ifdef ARDJACK_ETHERNET_AVAILABLE
		#include "EthernetInterface.h"
	#endif

	#ifdef ARDJACK_WIFI_AVAILABLE
		#include "WiFiInterface.h"
	#endif
#else
	#include "ClipboardConnection.h"
	#include "VellemanK8055Device.h"
	#include "WinDevice.h"
#endif

#ifdef ARDJACK_NETWORK_AVAILABLE
	#include "HttpConnection.h"
	#include "TcpConnection.h"
	#include "UdpConnection.h"
#endif

Enumeration* Register::ObjectTypes = NULL;



Register::Register()
{
	if (Globals::Verbosity > 7)
		Log::LogInfo(PRM("Register ctor"));

	for (int i = 0; i < ARDJACK_MAX_OBJECTS; i++)
		Objects[i] = NULL;

	ObjectCount = 0;

	if (NULL == ObjectTypes)
	{
		ObjectTypes = new Enumeration(PRM("Object Types"));

#ifdef ARDJACK_INCLUDE_BEACONS
		ObjectTypes->Add(PRM("Beacon"), ARDJACK_OBJECT_TYPE_BEACON);
#endif

#ifdef ARDJACK_INCLUDE_BRIDGES
		ObjectTypes->Add(PRM("Bridge"), ARDJACK_OBJECT_TYPE_BRIDGE);
#endif

		ObjectTypes->Add(PRM("Connection"), ARDJACK_OBJECT_TYPE_CONNECTION);

#ifdef ARDJACK_INCLUDE_DATALOGGERS
		ObjectTypes->Add(PRM("DataLogger"), ARDJACK_OBJECT_TYPE_DATALOGGER);
#endif

		ObjectTypes->Add(PRM("Device"), ARDJACK_OBJECT_TYPE_DEVICE);
		ObjectTypes->Add(PRM("Filter"), ARDJACK_OBJECT_TYPE_FILTER);

#ifdef ARDJACK_NETWORK_AVAILABLE
		ObjectTypes->Add(PRM("NetworkInterface"), ARDJACK_OBJECT_TYPE_NETWORKINTERFACE);
		ObjectTypes->Add(PRM("Net"), ARDJACK_OBJECT_TYPE_NETWORKINTERFACE);
#endif

		ObjectTypes->Add(PRM("Object"), ARDJACK_OBJECT_TYPE_OBJECT);

#ifdef ARDJACK_INCLUDE_SHIELDS
		ObjectTypes->Add(PRM("Shield"), ARDJACK_OBJECT_TYPE_SHIELD);
#endif

		ObjectTypes->Add(PRM("(unknown)"), ARDJACK_OBJECT_TYPE_UNKNOWN);
	}
}


Register::~Register()
{
	for (int i = 0; i < ARDJACK_MAX_OBJECTS; i++)
	{
		if (NULL != Objects[i])
		{
			delete Objects[i];
			Objects[i] = NULL;
		}
	}

	if (NULL != ObjectTypes)
	{
		delete ObjectTypes;
		ObjectTypes = NULL;
	}
}


bool Register::AddObject(IoTObject *addObj)
{
	// Do we already have 'addObj'?
	if (LookupObject(addObj) >= 0)
		return true;

	// Is there already an object called 'addObj->Name'?
	IoTObject* obj = LookupName(addObj->Name);

	if (NULL != obj)
	{
		// An object called 'addObj->Name' is already present.

		// Is it of type 'addObj->Type'?
		if (obj->Type != addObj->Type)
		{
			// No - error.
			Log::LogErrorF(PRM("Register::AddObject: Can't add object '%s' (type '%s') - there's already one with type '%s'"),
				addObj->Name, ObjectTypeName(addObj->Type), ObjectTypeName(obj->Type));
			return false;
		}

		// TODO:
		// Replace the existing object (assuming this is OK!).
		int i = LookupObject(obj);
		Objects[i] = addObj;

		return true;
	}

	// Is there room to add more?
	if (ObjectCount >= ARDJACK_MAX_OBJECTS)
	{
		// No - error.
		Log::LogErrorF(PRM("Register::AddObject: No room to add '%s' (type '%s', subtype '%s') (%d objects)"),
			addObj->Name, ObjectTypeName(addObj->Type), ObjectSubtypeName(addObj->Type, addObj->Subtype), ObjectCount);

		return false;
	}

	// Add this object.
	Objects[ObjectCount++] = addObj;

	Log::LogInfoF(PRM("Register::AddObject: Added '%s' (type '%s', subtype '%s')  (%d objects)"),
		addObj->Name, ObjectTypeName(addObj->Type), ObjectSubtypeName(addObj->Type, addObj->Subtype), ObjectCount);

	return true;
}


IoTObject* Register::CreateObject(int type, int subtype, const char* name)
{
	IoTObject* result = NULL;

	Log::LogInfoF(PRM("Register::CreateObject: Create '%s' (type '%s', subtype '%s')"),
		name, ObjectTypeName(type), ObjectSubtypeName(type, subtype));

	switch (type)
	{
#ifdef ARDJACK_INCLUDE_BEACONS
	case ARDJACK_OBJECT_TYPE_BEACON:
		result = new Beacon(name);
		break;
#endif

#ifdef ARDJACK_INCLUDE_BRIDGES
	case ARDJACK_OBJECT_TYPE_BRIDGE:
		result = new Bridge(name);
		break;
#endif

	case ARDJACK_OBJECT_TYPE_CONNECTION:
	{
		switch (subtype)
		{
#ifdef ARDUINO
#else
		case ARDJACK_CONNECTION_SUBTYPE_CLIPBOARD:
			result = new ClipboardConnection(name);
			break;
#endif

		case ARDJACK_CONNECTION_SUBTYPE_LOG:
			result = new LogConnection(name);
			break;

		case ARDJACK_CONNECTION_SUBTYPE_SERIAL:
			result = new SerialConnection(name);
			break;

#ifdef ARDJACK_NETWORK_AVAILABLE
		case ARDJACK_CONNECTION_SUBTYPE_HTTP:
			result = new HttpConnection(name);
			break;

		case ARDJACK_CONNECTION_SUBTYPE_TCP:
			result = new TcpConnection(name);
			break;

		case ARDJACK_CONNECTION_SUBTYPE_UDP:
			result = new UdpConnection(name);
			break;
#endif
		}

		if (NULL != result)
		{
			Connection* conn = (Connection*)result;
			Globals::SetupStandardRoutes(conn);
		}
	}
		break;

#ifdef ARDJACK_INCLUDE_DATALOGGERS
	case ARDJACK_OBJECT_TYPE_DATALOGGER:
		result = new DataLogger(name);
		break;
#endif

	case ARDJACK_OBJECT_TYPE_DEVICE:
		switch (subtype)
		{
#ifdef ARDUINO
		case ARDJACK_DEVICE_SUBTYPE_ARDUINO:
			result = new ArduinoDevice(name);
			break;
#else
		case ARDJACK_DEVICE_SUBTYPE_VELLEMANK8055:
			result = new VellemanK8055Device(name);
			break;

		case ARDJACK_DEVICE_SUBTYPE_WINDOWS:
			result = new WinDevice(name);
			break;
#endif
		}
		break;

	case ARDJACK_OBJECT_TYPE_FILTER:
		result = new Filter(name);
		break;

#ifdef ARDJACK_NETWORK_AVAILABLE
	case ARDJACK_OBJECT_TYPE_NETWORKINTERFACE:
		switch (subtype)
		{
#ifdef ARDUINO
	#ifdef ARDJACK_ETHERNET_AVAILABLE
		case ARDJACK_NETWORK_SUBTYPE_ETHERNET:
			result = new EthernetInterface(name);
			break;
	#endif

	#ifdef ARDJACK_WIFI_AVAILABLE
		case ARDJACK_NETWORK_SUBTYPE_WIFI:
			result = new WiFiInterface(name);
			break;
	#endif
#endif
		}
		break;
#endif

	case ARDJACK_OBJECT_TYPE_OBJECT:
		result = new IoTObject(name);
		break;

#ifdef ARDJACK_INCLUDE_SHIELDS
	case ARDJACK_OBJECT_TYPE_SHIELD:
		result = (IoTObject*)Globals::ShieldMgr->CreateShield(name, subtype);
		break;
#endif
	}

	if (NULL == result)
	{
		Log::LogErrorF(PRM("Register::CreateObject: Unable to create '%s' (type '%s', subtype '%s')"),
			name, ObjectTypeName(type), ObjectSubtypeName(type, subtype));
		return NULL;
	}

	result->Type = type;
	result->Subtype = subtype;

	if (Globals::Verbosity > 3)
	{
		Log::LogInfoF(PRM("Register::CreateObject: Created '%s' (type '%s', subtype '%s')"),
			result->Name, ObjectTypeName(type), ObjectSubtypeName(type, subtype));
	}

	AddObject(result);

	result->AddConfig();

	return result;
}


bool Register::DeleteObject(IoTObject* obj)
{
	// Once added to the Register, the Register 'owns' all objects, so removal and deletion are combined here.
	char objName[ARDJACK_MAX_NAME_LENGTH];
	char objType[ARDJACK_MAX_NAME_LENGTH];

	strcpy(objName, obj->Name);
	strcpy(objType, ObjectTypeName(obj->Type));

	int i = LookupObject(obj);

	if (i < 0)
	{
		Log::LogErrorF(PRM("Register::DeleteObject: Object '%s' (type '%s') is missing"), objName, objType);
		return false;
	}

	ArrayHelpers::RemoveElement((void**)Objects, ObjectCount, i);
	ObjectCount--;

	delete obj;

	if (Globals::Verbosity > 3)
		Log::LogInfoF(PRM("Register::DeleteObject: Removed object '%s' (type '%s')"), objName, objType);

	return true;
}


IoTObject *Register::LookupName(const char* name)
{
	for (int i = 0; i < ObjectCount; i++)
	{
		IoTObject* obj = Objects[i];

		if (Utils::StringEquals(obj->Name, name))
			return obj;
	}

	return NULL;
}


int Register::LookupObject(IoTObject* object)
{
	for (int i = 0; i < ObjectCount; i++)
	{
		IoTObject* obj = Objects[i];

		if (obj == object)
			return i;
	}

	return -1;
}


int Register::LookupObjectType(const char* name, int defaultValue)
{
	return ObjectTypes->LookupName(name, defaultValue);
}


const char* Register::ObjectSubtypeName(int type, int subtype, const char* defaultName)
{
	switch (type)
	{
	case ARDJACK_OBJECT_TYPE_CONNECTION:
		return Globals::ConnectionMgr->Subtypes->LookupValue(subtype, defaultName);

	case ARDJACK_OBJECT_TYPE_DEVICE:
		return Globals::DeviceMgr->Subtypes->LookupValue(subtype, defaultName);

#ifdef ARDJACK_NETWORK_AVAILABLE
	case ARDJACK_OBJECT_TYPE_NETWORKINTERFACE:
		return Globals::NetworkMgr->Subtypes->LookupValue(subtype, defaultName);
#endif

#ifdef ARDJACK_INCLUDE_SHIELDS
	case ARDJACK_OBJECT_TYPE_SHIELD:
		return Globals::ShieldMgr->Subtypes->LookupValue(subtype, defaultName);
#endif
	}

	// ??
	return defaultName;
}


const char* Register::ObjectTypeName(int type, const char* defaultName)
{
	return ObjectTypes->LookupValue(type, defaultName);
}

