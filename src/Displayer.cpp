/*
	Displayer.cpp

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

#include "ArrayHelpers.h"
#include "ArduinoDevice.h"
#include "Beacon.h"
#include "BeaconManager.h"
#include "Bridge.h"
#include "BridgeManager.h"
#include "CmdInterpreter.h"
#include "CommandSet.h"
#include "ConfigProp.h"
#include "Configuration.h"
#include "Connection.h"
#include "ConnectionManager.h"
#include "DataLogger.h"
#include "DataLoggerManager.h"
#include "Device.h"
#include "DeviceCodec1.h"
#include "DeviceManager.h"
#include "Dictionary.h"
#include "Displayer.h"
#include "Dynamic.h"
#include "Enumeration.h"
#include "FieldReplacer.h"
#include "Filter.h"
#include "FilterManager.h"
#include "Globals.h"
#include "HttpConnection.h"
#include "Route.h"
#include "Log.h"
#include "Part.h"
#include "PartManager.h"
#include "SerialConnection.h"
#include "Shield.h"
#include "StringList.h"
#include "Table.h"
#include "TcpConnection.h"
#include "UdpConnection.h"
#include "UserPart.h"
#include "Utils.h"

#ifdef ARDUINO
	#ifdef ARDJACK_WIFI_AVAILABLE
		#include "WiFiLibrary.h"
		#include "WiFiInterface.h"
	#endif

	extern caddr_t __data_start__;
	extern caddr_t __data_end__;
	extern caddr_t __bss_start__;
	extern caddr_t __bss_end__;
#else
//	#include "MemoryHelpers.h"
#endif

#ifdef ARDJACK_INCLUDE_PERSISTENCE
	#include "PersistentFile.h"
#endif

#ifdef ARDJACK_NETWORK_AVAILABLE
	#include "NetworkInterface.h"
	#include "NetworkManager.h"
#endif

bool Displayer::SaveLogIncludeMemory = false;
bool Displayer::SaveLogIncludeTime = false;



bool Displayer::DisplayActive(IoTObject* obj)
{
	Log::LogInfo(PRM("Active: "), Utils::Bool2yesno(obj->Active()));

	return true;
}


#ifdef ARDJACK_INCLUDE_BEACONS
	bool Displayer::DisplayBeacon(Beacon* beacon)
	{
		DisplayHeader(PRM("BEACON"), beacon->Name);

		Log::LogInfo(PRM("Active: "), Utils::Bool2yesno(beacon->Active()));
		Log::LogInfoF(PRM("Events: %d"), beacon->Events);

		beacon->Config->LogIt();

		return true;
	}


	bool Displayer::DisplayBeacons()
	{
		DisplayHeader(PRM("BEACONS"));

		return DisplayObjectsOfType(ARDJACK_OBJECT_TYPE_BEACON);
	}
#endif


#ifdef ARDJACK_INCLUDE_BRIDGES
	bool Displayer::DisplayBridge(Bridge* bridge)
	{
		DisplayHeader(PRM("BRIDGE"), bridge->Name);

		Log::LogInfo(PRM("Active: "), Utils::Bool2yesno(bridge->Active()));
		Log::LogInfoF(PRM("Object1 Events: %d"), bridge->Object1Events);
		Log::LogInfoF(PRM("Object2 Events: %d"), bridge->Object2Events);

		bridge->Config->LogIt();

		return true;
	}


	bool Displayer::DisplayBridges()
	{
		DisplayHeader(PRM("BRIDGES"));

		for (int i = 0; i < Globals::ObjectRegister->ObjectCount; i++)
		{
			IoTObject* obj = Globals::ObjectRegister->Objects[i];

			if (obj->Type == ARDJACK_OBJECT_TYPE_BRIDGE)
			{
				Bridge* bridge = (Bridge*)obj;
				Log::LogInfo("  ", bridge->Name, PRM(" active="), Utils::Bool210(bridge->Active()));
			}
		}

		return true;
	}
#endif


bool Displayer::DisplayConnection(Connection* conn)
{
	DisplayHeader(PRM("CONNECTION"), conn->Name);

	Log::LogInfo("Active: ", Utils::Bool2yesno(conn->Active()));

	Log::LogInfoF(PRM("RX: %d events (%d chars)"), conn->RxEvents, conn->RxCount);
	Log::LogInfoF(PRM("TX: %d events (%d chars)"), conn->TxEvents, conn->TxCount);

	conn->Config->LogIt();

	Log::LogInfo(PRM("ROUTES"));
	char temp[120];

	for (int i = 0; i < conn->RouteCount; i++)
	{
		Route* route = conn->Routes[i];

		Log::LogInfoF(PRM("   Route '%s':%s%s%s"), route->Name,
			(NULL != route->Buffer) ? PRM(" buffer set,") : "",
			(NULL != route->Callback) ? PRM(" callback set,") : "",
			(route == conn->DefaultRoute) ? PRM(" DEFAULT ROUTE") : "",
			(route->AlwaysUse) ? PRM(" ALWAYS USE") : "");

		Log::LogInfo(PRM("      Message Format 0 filters:"));

		if (route->Filter.TextFilter.Active())
			Log::LogInfo(PRM("         Text filter: "), route->Filter.TextFilter.ToString(temp));

		Log::LogInfo(PRM("      Message Format 1 filters:"));

		if (route->Filter.FromFilter.Active())
			Log::LogInfo(PRM("         FromPath filter: "), route->Filter.FromFilter.ToString(temp));

		if (route->Filter.ToFilter.Active())
			Log::LogInfo(PRM("         ToPath filter: "), route->Filter.ToFilter.ToString(temp));

		if (route->Filter.TypeFilter.Active())
			Log::LogInfo(PRM("         Type filter: "), route->Filter.TypeFilter.ToString(temp));

		if (route->Filter.BodyFilter.Active())
			Log::LogInfo(PRM("         Body filter: "), route->Filter.BodyFilter.ToString(temp));

		Log::LogInfo("");
	}

	Log::LogInfo(PRM(""));

	return true;
}


bool Displayer::DisplayConnections()
{
	DisplayHeader(PRM("CONNECTIONS"));

	for (int i = 0; i < Globals::ObjectRegister->ObjectCount; i++)
	{
		IoTObject* obj = Globals::ObjectRegister->Objects[i];

		if (obj->Type == ARDJACK_OBJECT_TYPE_CONNECTION)
		{
			Connection* conn = (Connection*)obj;

			Log::LogInfo("  ", conn->Name, PRM(" active="), Utils::Bool210(conn->Active()));
		}
	}

	return true;
}


#ifdef ARDJACK_INCLUDE_DATALOGGERS
	bool Displayer::DisplayDataLogger(DataLogger* logger)
	{
		DisplayHeader(PRM("DATALOGGER"), logger->Name);

		Log::LogInfo(PRM("Active: "), Utils::Bool2yesno(logger->Active()));
		Log::LogInfoF(PRM("Events: %d"), logger->Events);

		logger->Config->LogIt();

		return true;
	}


	bool Displayer::DisplayDataLoggers()
	{
		DisplayHeader(PRM("DATALOGGERS"));

		for (int i = 0; i < Globals::ObjectRegister->ObjectCount; i++)
		{
			IoTObject* obj = Globals::ObjectRegister->Objects[i];

			if (obj->Type == ARDJACK_OBJECT_TYPE_DATALOGGER)
			{
				DataLogger* logger = (DataLogger*)obj;

				Log::LogInfo("  ", logger->Name, PRM(" active="), Utils::Bool210(logger->Active()));
			}
		}

		return true;
	}
#endif


bool Displayer::DisplayDevice(Device* dev)
{
	DisplayHeader(PRM("DEVICE"), dev->Name);

	Log::LogInfo(PRM("Active:       "), Utils::Bool2yesno(dev->Active()));
	Log::LogInfoF(PRM("Read Events:  %d"), dev->ReadEvents);
	Log::LogInfoF(PRM("Write Events: %d"), dev->WriteEvents);

#ifdef ARDJACK_INCLUDE_SHIELDS
	char shield[ARDJACK_MAX_NAME_LENGTH];

	if (NULL == dev->DeviceShield)
		strcpy(shield, PRM("(none)"));
	else
		sprintf(shield, "'%s'", dev->DeviceShield->Name);

	Log::LogInfo(PRM("Shield:       "), shield);
#endif

	dev->Config->LogIt();

	// Get the current value of each Part.
	Dynamic value;

	for (int i = 0; i < dev->PartCount; i++)
	{
		Part* part = dev->Parts[i];
		int nRead = 1;
		dev->Read(part, &value);
	}

	Log::LogInfoF(PRM("%d Parts:"), dev->PartCount);

	Table table;
	table.AddColumn("Part", 18);
	table.AddColumn("Type", 10);
	table.AddColumn("Subtype", 12);
	table.AddColumn("Pin", 6);
	table.AddColumn("Value", 18);
	table.AddColumn("Notifying?", 12);
	table.AddColumn("Filter", 18);

	char line[202];
	Log::LogInfo(table.HorizontalLine(line));
	Log::LogInfo(table.Header(line));
	Log::LogInfo(table.HorizontalLine(line));

	char notify[6];
	char pin[6];
	char strValue[ARDJACK_MAX_DYNAMIC_STRING_LENGTH];
	char subtypeName[ARDJACK_MAX_NAME_LENGTH];

	for (int i = 0; i < dev->PartCount; i++)
	{
		Part* part = dev->Parts[i];

		if (part->Type == ARDJACK_PART_TYPE_USER)
			strcpy(subtypeName, Globals::PartMgr->PartSubtypeName(part->Subtype));
		else
			strcpy(subtypeName, "");

		part->Value.AsString(strValue);

		Log::LogInfo(table.Row(line, part->Name, Globals::PartMgr->PartTypeName(part->Type), subtypeName,
			itoa(part->Pin, pin, 10), strValue, Utils::Bool2yesno(part->Notify, notify), part->FilterName));
	}

	Log::LogInfo(table.HorizontalLine(line));
	Log::LogInfo();

	return true;
}


bool Displayer::DisplayDevices()
{
	DisplayHeader(PRM("DEVICES"));

	return DisplayObjectsOfType(ARDJACK_OBJECT_TYPE_DEVICE);
}


bool Displayer::DisplayFilter(Filter* filter)
{
	DisplayHeader(PRM("FILTER"), filter->Name);

	filter->Config->LogIt();

	return true;
}


bool Displayer::DisplayFilters()
{
	DisplayHeader(PRM("FILTERS"));

	return DisplayObjectsOfType(ARDJACK_OBJECT_TYPE_FILTER);
}


bool Displayer::DisplayHeader(const char* heading, const char* name)
{
	char temp[82];
	strcpy(temp, heading);

	if (strlen(name) > 0)
	{
		strcat(temp, " ");
		strcat(temp, name);
	}

	int count = Utils::StringLen(temp);
	char line[82];
	Utils::RepeatChar(line, '_', count);

	Log::LogInfo(line);
	Log::LogInfo(temp);
	Log::LogInfo(line);

	return true;
}


bool Displayer::DisplayItem(const char* args)
{
	SaveLogIncludeMemory = Log::IncludeMemory;
	SaveLogIncludeTime = Log::IncludeTime;
	Log::IncludeMemory = false;
	Log::IncludeTime = false;

	if (Utils::StringIsNullOrEmpty(args))
	{
		DisplayStatus();
		Log::IncludeMemory = SaveLogIncludeMemory;
		Log::IncludeTime = SaveLogIncludeTime;

		return true;
	}

	char item[20];
	const char* remainder = NULL;

	Utils::GetArgs(args, item, &remainder);

	if (strlen(item) == 0)
		DisplayStatus();
	else
	{
		_strupr(item);

		if (Utils::StringEquals(item, "ACTIVE"))
			DisplayObjects(true);

#ifdef ARDJACK_INCLUDE_BEACONS
		else if (Utils::StringEquals(item, "BEACONS"))
			DisplayBeacons();
#endif

#ifdef ARDJACK_INCLUDE_BRIDGES
		else if (Utils::StringEquals(item, "BRIDGES"))
			DisplayBridges();
#endif

		else if (Utils::StringEquals(item, "CONNECTIONS"))
			DisplayConnections();

#ifdef ARDJACK_INCLUDE_DATALOGGERS
		else if (Utils::StringEquals(item, "DATALOGGERS"))
			DisplayDataLoggers();
#endif

		else if (Utils::StringEquals(item, "DEVICES"))
			DisplayDevices();

		else if (Utils::StringEquals(item, "FILTERS"))
			DisplayFilters();

		else if (Utils::StringEquals(item, "MACROS"))
			DisplayMacros();

		else if (Utils::StringEquals(item, "MEMORY"))
			DisplayMemory();

		else if (Utils::StringEquals(item, "NET") || Utils::StringEquals(item, "NETWORK"))
			DisplayNetwork();

		else if (Utils::StringEquals(item, "OBJECTS"))
			DisplayObjects(false);

		else if (Utils::StringEquals(item, "SIZES"))
			DisplaySizes();

		else if (Utils::StringEquals(item, "STATUS"))
			DisplayStatus();

		else
		{
			// Maybe it's an Object name, or 'Device.Part' syntax?
			if (!Displayer::DisplayObject2(item, true))
				Log::LogError("Invalid DISPLAY item: ", item);
		}
	}

	Log::IncludeMemory = SaveLogIncludeMemory;
	Log::IncludeTime = SaveLogIncludeTime;

	return true;
}


bool Displayer::DisplayMacros()
{
	DisplayHeader(PRM("MACROS"));

	const char* content;
	char name[ARDJACK_MAX_NAME_LENGTH];

	for (int i = 0; i < Globals::Interpreter->Macros.Count(); i++)
	{
		content = Globals::Interpreter->Macros.Get(i, name);
		Log::LogInfoF(PRM("  %s = '%s'"), name, content);
	}

	return true;
}


bool Displayer::DisplayMemory()
{
#ifdef ARDUINO
	DisplayHeader(PRM("MEMORY"));

	MemoryInfo memInfo;
	Utils::GetMemoryInfo(&memInfo);

	char temp[10];

	Log::LogInfoF(PRM("  RAM size:    %d bytes"), memInfo.RAMSize);
	Log::LogInfoF(PRM("  Stack start: 0x%x"), (uint32_t)memInfo.StackStart);
	Log::LogInfoF(PRM("  SP:          0x%x"), (uint32_t)memInfo.SP);
	Log::LogInfoF(PRM("  Heap base:   0x%x"), (uint32_t)Globals::HeapBase);
	Log::LogInfoF(PRM("  RAM base:    0x%x"), (uint32_t)memInfo.RAMBase);
	Log::LogInfoF(PRM("  Heap max:    %d bytes"), memInfo.HeapMax);
	Log::LogInfoF(PRM("  Heap used:   %d bytes"), memInfo.HeapUsed);
	Log::LogInfoF(PRM("  Heap free:   %d bytes"), memInfo.HeapFree);
	Log::LogInfoF(PRM("  Static data: %d bytes"), (uint32_t)Globals::HeapBase - (uint32_t)memInfo.RAMBase);
	Log::LogInfoF(PRM("  Stack used:  %d bytes"), (uint32_t)memInfo.StackStart - (uint32_t)memInfo.SP);
	Log::LogInfoF(PRM("  Free RAM:    %d bytes"), memInfo.FreeRAM);

	if ((memInfo.FreeRAM > 0) && (memInfo.FreeRAM < 1024))
		Log::LogInfo(PRM("  --- WARNING: Free RAM < 1024 bytes ---"));

	//Log::LogInfoF(PRM("  DATA end:    0x%x"), (int)&__data_end__);
	//Log::LogInfoF(PRM("  DATA start:  0x%x"), (int)&__data_start__);
	//Log::LogInfoF(PRM("  BSS end:     0x%x"), (int)&__bss_end__);
	//Log::LogInfoF(PRM("  BSS start:   0x%x"), (int)&__bss_start__);

	Log::LogInfo("");
#else
	//DisplayHeader(PRM("MEMORY"));

	//long heapUsed = ArrayHelpers::HeapUsed();
	//Log::LogInfoF(PRM("  Heap used:   %d bytes"), heapUsed);
#endif

	return true;
}


bool Displayer::DisplayNetwork()
{
	DisplayHeader(PRM("NETWORK"));

#ifdef ARDJACK_NETWORK_AVAILABLE
	Log::LogInfo(PRM("Networking included."));

	#ifdef ARDUINO
	#else
		Log::LogInfo(PRM("  Host:     "), Globals::HostName);
		Log::LogInfo(PRM("  Local IP: "), Globals::IpAddress);
	#endif

	// Display all network interfaces, e.g. WiFi.
	for (int i = 0; i < Globals::ObjectRegister->ObjectCount; i++)
	{
		IoTObject* obj = Globals::ObjectRegister->Objects[i];

		switch (obj->Type)
		{
		case ARDJACK_OBJECT_TYPE_NETWORKINTERFACE:
			NetworkInterface* net = (NetworkInterface*)obj;
			Log::LogInfo(net->Name, ":");
			net->LogStatus();
			break;
		}
	}
#else
	Log::LogInfo(PRM("Networking excluded."));
#endif

	return true;
}


#ifdef ARDJACK_NETWORK_AVAILABLE
bool Displayer::DisplayNetworkInterface(NetworkInterface* net)
{
	DisplayHeader(PRM("NETWORK INTERFACE"), net->Name);

	net->Config->LogIt();

	return true;
}
#endif


bool Displayer::DisplayObject(IoTObject* obj)
{
	switch (obj->Type)
	{
#ifdef ARDJACK_INCLUDE_BEACONS
	case ARDJACK_OBJECT_TYPE_BEACON:
		return DisplayBeacon((Beacon*)obj);
#endif

#ifdef ARDJACK_INCLUDE_BRIDGES
	case ARDJACK_OBJECT_TYPE_BRIDGE:
		return DisplayBridge((Bridge*)obj);
#endif

	case ARDJACK_OBJECT_TYPE_CONNECTION:
		return DisplayConnection((Connection*)obj);

#ifdef ARDJACK_INCLUDE_DATALOGGERS
	case ARDJACK_OBJECT_TYPE_DATALOGGER:
		return DisplayDataLogger((DataLogger*)obj);
#endif

	case ARDJACK_OBJECT_TYPE_DEVICE:
		return DisplayDevice((Device*)obj);

	case ARDJACK_OBJECT_TYPE_FILTER:
		return DisplayFilter((Filter*)obj);

	case ARDJACK_OBJECT_TYPE_NETWORKINTERFACE:
		Log::LogInfo(PRM("NETWORK INTERFACE "), obj->Name);
		DisplayActive(obj);
		obj->Config->LogIt();
		return true;

	case ARDJACK_OBJECT_TYPE_UNKNOWN:
		Log::LogInfo(PRM("UNKNOWN "), obj->Name);
		DisplayActive(obj);
		obj->Config->LogIt();
		return true;
	}

	return false;
}


bool Displayer::DisplayObject2(char* text, bool quiet)
{
	// If 'text' is an Object name, or 'Device.Part' syntax, display the corresponding item.
	char fields2[2][ARDJACK_MAX_VALUE_LENGTH];
	int count2 = Utils::SplitText2Array(text, '.', fields2, 2, ARDJACK_MAX_VALUE_LENGTH);

	char objName[ARDJACK_MAX_NAME_LENGTH];
	char partName[ARDJACK_MAX_NAME_LENGTH];
	bool partSyntax = (count2 > 1);

	if (partSyntax)
	{
		strcpy(objName, fields2[0]);
		strcpy(partName, fields2[1]);
	}
	else
	{
		strcpy(objName, fields2[0]);
		partName[0] = NULL;
	}

	// Check the Object name.
	IoTObject* obj = Globals::ObjectRegister->LookupName(objName);
	if (NULL == obj)
	{
		if (!quiet)
			Log::LogError(PRM("DisplayObject2: Not an Object: '"), objName, "'");
		return false;
	}

	if (partSyntax)
	{
		if (obj->Type != ARDJACK_OBJECT_TYPE_DEVICE)
		{
			if (!quiet)
				Log::LogError(PRM("DisplayObject2: '"), obj->Name, PRM("' is not a Device"));
			return false;
		}

		Device* dev = (Device*)obj;

		Part* part = dev->LookupPart(partName, true);
		if (NULL == part)
		{
			if (!quiet)
				Log::LogError(PRM("DisplayObject2: '"), partName, PRM("' is not a Part of Device '"), dev->Name, "'");
			return false;
		}

		return DisplayPart(dev, part);
	}

	return DisplayObject(obj);
}



bool Displayer::DisplayObjects(bool activeOnly)
{
	DisplayHeader(PRM("OBJECTS"));

	Table table;
	table.AddColumn("Name", 16);
	table.AddColumn("Type", 20);
	table.AddColumn("Subtype", 20);
	table.AddColumn("Active", 8);

	char line[202];
	Log::LogInfo(table.HorizontalLine(line));
	Log::LogInfo(table.Header(line));
	Log::LogInfo(table.HorizontalLine(line));

	char temp[10];

	for (int i = 0; i < Globals::ObjectRegister->ObjectCount; i++)
	{
		IoTObject* obj = Globals::ObjectRegister->Objects[i];

		if (obj->Active() || !activeOnly)
		{
			Log::LogInfo(table.Row(line, obj->Name, Globals::ObjectRegister->ObjectTypeName(obj->Type),
				Globals::ObjectRegister->ObjectSubtypeName(obj->Type, obj->Subtype),
				Utils::Bool2yesno(obj->Active(), temp)));
		}
	}

	Log::LogInfo(table.HorizontalLine(line));
	Log::LogInfo();

	return true;
}


bool Displayer::DisplayObjectsOfType(int type)
{
	for (int i = 0; i < Globals::ObjectRegister->ObjectCount; i++)
	{
		IoTObject* obj = Globals::ObjectRegister->Objects[i];

		if (obj->Type == type)
			Log::LogInfo("  ", obj->Name, PRM(" active="), Utils::Bool210(obj->Active()));
	}

	return true;
}


bool Displayer::DisplayPart(Device* dev, Part* part)
{
	DisplayHeader(PRM("PART"), part->Name);

	// Get the current value.
	Dynamic value;
	dev->Read(part, &value);

	Table table;
	table.AddColumn("Item", 16);
	table.AddColumn("Value", 20);

	char line[202];
	Log::LogInfo(table.HorizontalLine(line));
	//Log::LogInfo(table.Header(line));
	//Log::LogInfo(table.HorizontalLine(line));

	char strValue[ARDJACK_MAX_DYNAMIC_STRING_LENGTH];
	part->Value.AsString(strValue);

	Log::LogInfo(table.Row(line, "Device", dev->Name));
	Log::LogInfo(table.Row(line, "Part Type", Globals::PartMgr->PartTypeName(part->Type)));
	Log::LogInfo(table.Row(line, "Pin", Utils::Int2String(part->Pin)));
	Log::LogInfo(table.Row(line, "Value", strValue));

	Log::LogInfo(table.HorizontalLine(line));

	if (NULL != part->Config)
	{
		Log::LogInfo();
		part->Config->LogIt();
	}

	return true;
}


bool Displayer::DisplaySize(const char* caption, int size)
{
	char temp[10];
	Log::LogInfo(caption, itoa(size, temp, 10));

	return true;
}


bool Displayer::DisplaySizes()
{
	DisplayHeader(PRM("OBJECT SIZES"));

#ifdef ARDUINO
	DisplaySize(PRM("  ArduinoDevice:      "), sizeof(ArduinoDevice));
#endif
#ifdef ARDJACK_INCLUDE_BEACONS
	DisplaySize(PRM("  Beacon:             "), sizeof(Beacon));
#endif
#ifdef ARDJACK_INCLUDE_BRIDGES
	DisplaySize(PRM("  Bridge:             "), sizeof(Bridge));
#endif
	DisplaySize(PRM("  CmdInterpreter:     "), sizeof(CmdInterpreter));
	DisplaySize(PRM("  CommandSet:         "), sizeof(CommandSet));
	DisplaySize(PRM("  ConfigProp:         "), sizeof(ConfigProp));
	DisplaySize(PRM("  Configuration:      "), sizeof(Configuration));
	DisplaySize(PRM("  Connection:         "), sizeof(Connection));
#ifdef ARDJACK_INCLUDE_DATALOGGERS
	DisplaySize(PRM("  DataLogger:         "), sizeof(DataLogger));
#endif
	DisplaySize(PRM("  DateTime:           "), sizeof(DateTime));
	DisplaySize(PRM("  Device:             "), sizeof(Device));
	DisplaySize(PRM("  Dictionary:         "), sizeof(Dictionary));
	DisplaySize(PRM("  Dynamic:            "), sizeof(Dynamic));
	DisplaySize(PRM("  Enumeration:        "), sizeof(Enumeration));
	DisplaySize(PRM("  FieldReplacer:      "), sizeof(FieldReplacer));
	DisplaySize(PRM("  FifoBuffer:         "), sizeof(FifoBuffer));
	DisplaySize(PRM("  Filter:             "), sizeof(Filter));
#ifdef ARDJACK_NETWORK_AVAILABLE
	DisplaySize(PRM("  HttpConnection:     "), sizeof(HttpConnection));
#endif
	DisplaySize(PRM("  Route:         "), sizeof(Route));
	DisplaySize(PRM("  Int8List:           "), sizeof(Int8List));
	DisplaySize(PRM("  IoTManager:         "), sizeof(IoTManager));
	DisplaySize(PRM("  IoTMessage:         "), sizeof(IoTMessage));
	DisplaySize(PRM("  IoTObject:          "), sizeof(IoTObject));
	DisplaySize(PRM("  MacroDef:           "), sizeof(MacroDef));
	DisplaySize(PRM("  MessageFilter:      "), sizeof(MessageFilter));
	DisplaySize(PRM("  Part:               "), sizeof(Part));
#ifdef ARDJACK_INCLUDE_PERSISTENCE
	DisplaySize(PRM("  PersistentFile:     "), sizeof(PersistentFile));
	DisplaySize(PRM("  PersistentFileLine: "), sizeof(PersistentFileLine));
#endif
	DisplaySize(PRM("  Register:           "), sizeof(Register));
	DisplaySize(PRM("  SerialConnection:   "), sizeof(SerialConnection));
	DisplaySize(PRM("  StringList:         "), sizeof(StringList));
	DisplaySize(PRM("  Table:              "), sizeof(Table));
#ifdef ARDJACK_NETWORK_AVAILABLE
	DisplaySize(PRM("  TcpConnection:      "), sizeof(TcpConnection));
	DisplaySize(PRM("  UdpConnection:      "), sizeof(UdpConnection));
#endif
	DisplaySize(PRM("  UserPart:           "), sizeof(UserPart));

#ifdef ARDUINO
	#ifdef ARDJACK_WIFI_AVAILABLE
		DisplaySize(PRM("  WiFiClient:         "), sizeof(WiFiClient));
		DisplaySize(PRM("  WiFiInterface:      "), sizeof(WiFiInterface));
		DisplaySize(PRM("  WiFiServer:         "), sizeof(WiFiServer));
		DisplaySize(PRM("  WiFiUDP:            "), sizeof(WiFiUDP));
	#endif
#endif

	return true;
}


bool Displayer::DisplayStatus()
{
	DisplayHeader(PRM("STATUS"));

	char temp[202];
	temp[0] = NULL;

	for (int i = 0; i < Globals::ObjectRegister->ObjectCount; i++)
	{
		IoTObject* obj = Globals::ObjectRegister->Objects[i];

		if (obj->Active())
		{
			strcat(temp, obj->Name);
			strcat(temp, " ");
		}
	}

	Log::LogInfo(PRM("  Active objects:       "), temp);
#ifdef ARDUINO
	Log::LogInfo(PRM("  Board type:           "), ARDJACK_BOARD_TYPE);
#endif
	Log::LogInfo(PRM("  Command prefix:       '"), Globals::CommandPrefix, "'");
	Log::LogInfo(PRM("  Command separator:    '"), Globals::CommandSeparator, "'");
	Log::LogInfo(PRM("  Comment prefix:       '"), Globals::CommentPrefix, "'");
	Log::LogInfo(PRM("  Compile time:         "), Globals::CompileDate, " ", Globals::CompileTime);
	Log::LogInfo(PRM("  Computer name:        '"), Globals::ComputerName, "'");
	//Log::LogInfo(PRM("  Exec prefix:          '"), Globals::ExecPrefix, "'");
	Log::LogInfo(PRM("  IP address:           "), Globals::IpAddress);
#ifdef ARDUINO
	Log::LogInfo(PRM("  Log.IncludeMemory:    "), Utils::Bool2yesno(SaveLogIncludeMemory));
#endif
	Log::LogInfo(PRM("  Log.IncludeTime:      "), Utils::Bool2yesno(SaveLogIncludeTime));
	Log::LogInfo(PRM("  Log.Prefix:           '"), Log::Prefix, "'");
#ifdef ARDUINO
	Log::LogInfo(PRM("  Log.UseSerial:        "), Utils::Bool2yesno(Log::UseSerial));
	//Log::LogInfo(PRM("  Log.UseSerialUSB:     "), Utils::Bool2yesno(Log::UseSerialUSB));
#endif
	Log::LogInfo(PRM("  Network available:    "), Utils::Bool2yesno(Globals::NetworkAvailable));
	Log::LogInfo(PRM("  'One command' prefix: '"), Globals::OneCommandPrefix, "'");
	Log::LogInfo(PRM("  RTC available:        "), Utils::Bool2yesno(Globals::RtcAvailable));
#ifdef ARDUINO
	Log::LogInfoF(PRM("  Serial baud rate:     %d"), Globals::SerialSpeed);
	Log::LogInfoF(PRM("  millis():             %d"), millis());
#endif
	Log::LogInfoF(PRM("  Time:                 %s (%d ms)"), Utils::GetNow(temp), Utils::NowMs());
	Log::LogInfoF(PRM("  Verbosity:            %d"), Globals::Verbosity);

	Log::LogInfo();
	Log::LogInfo(PRM("BUILD INCLUDES:"));
	Log::LogInfo(PRM("  Arduino DHT (UserPart)      "), Utils::Bool2yesno(Globals::IncludeArduinoDHT));
	Log::LogInfo(PRM("  Arduino MF Shield           "), Utils::Bool2yesno(Globals::IncludeArduinoMFShield));
	Log::LogInfo(PRM("  Arduino NeoPixel (UserPart) "), Utils::Bool2yesno(Globals::IncludeArduinoNeoPixel));
	Log::LogInfo(PRM("  Beacons                     "), Utils::Bool2yesno(Globals::IncludeBeacons));
	Log::LogInfo(PRM("  Bridges                     "), Utils::Bool2yesno(Globals::IncludeBridges));
	Log::LogInfo(PRM("  DataLoggers                 "), Utils::Bool2yesno(Globals::IncludeDataLoggers));
	Log::LogInfo(PRM("  Persistence                 "), Utils::Bool2yesno(Globals::IncludePersistence));
	Log::LogInfo(PRM("  Shields                     "), Utils::Bool2yesno(Globals::IncludeShields));
	Log::LogInfo(PRM("  Thinker Shield              "), Utils::Bool2yesno(Globals::IncludeThinkerShield));
	Log::LogInfo(PRM("  WinDisk (UserPart)          "), Utils::Bool2yesno(Globals::IncludeWinDisk));
	Log::LogInfo(PRM("  WinMemory (UserPart)        "), Utils::Bool2yesno(Globals::IncludeWinMemory));

	return true;
}

