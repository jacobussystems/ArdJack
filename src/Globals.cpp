/*
	Globals.cpp

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
	#include <typeinfo>
#endif

#include "ArduinoDevice.h"
#include "Beacon.h"
#include "BeaconManager.h"
#include "Bridge.h"
#include "BridgeManager.h"
#include "CmdInterpreter.h"
#include "CommandSet.h"
#include "Connection.h"
#include "ConnectionManager.h"
#include "DataLogger.h"
#include "DataLoggerManager.h"
#include "Device.h"
#include "DeviceCodec1.h"
#include "DeviceManager.h"
#include "FifoBuffer.h"
#include "Filter.h"
#include "FilterManager.h"
#include "Globals.h"
#include "HttpConnection.h"
#include "Route.h"
#include "Log.h"
#include "Part.h"
#include "PartManager.h"
#include "Register.h"
#include "SerialConnection.h"
#include "ShieldManager.h"
#include "UdpConnection.h"
#include "Utils.h"

#ifdef ARDUINO
	#ifdef ARDJACK_RTC_AVAILABLE
		#include "RtcClock.h"
	#else
		#include "ArduinoClock.h"
	#endif
#else
	#include "ClipboardConnection.h"
	#include "WinClock.h"
	#include "WinDevice.h"
#endif

#ifdef ARDJACK_INCLUDE_PERSISTENCE
	#include "PersistentFile.h"
	#include "PersistentFileManager.h"
#endif

#ifdef ARDJACK_NETWORK_AVAILABLE
	#include "NetworkInterface.h"
	#include "NetworkManager.h"
#endif


char Globals::AppName[] = "";
//bool Globals::AutoClearCommandLine = false;
IoTClock* Globals::Clock = NULL;
FifoBuffer* Globals::CommandBuffer = NULL;
char Globals::CommandPrefix[] = "";
char Globals::CommandSeparator[] = "";
CommandSet* Globals::CommandSet0 = NULL;
char Globals::CommentPrefix[] = "";
char Globals::CompileDate[] = "";
char Globals::CompileTime[] = "";
char Globals::ComputerName[] = "";
ConnectionManager* Globals::ConnectionMgr = NULL;
FifoBuffer* Globals::DeviceBuffer = NULL;
DeviceCodec1* Globals::DeviceCodec = NULL;
DeviceManager* Globals::DeviceMgr = NULL;
char Globals::EmptyString[] = "";
bool Globals::EnableSound = false;
char Globals::ExecPrefix[] = "";
FilterManager* Globals::FilterMgr = NULL;
char Globals::FromName[] = "";
void* Globals::HeapBase = 0;
bool Globals::IncludeArduinoDHT = false;
bool Globals::IncludeArduinoMFShield = false;
bool Globals::IncludeArduinoNeoPixel = false;
bool Globals::IncludeBeacons = false;
bool Globals::IncludeBridges = false;
bool Globals::IncludeDataLoggers = false;
bool Globals::IncludePersistence = false;
bool Globals::IncludeShields = false;
bool Globals::IncludeWinDisk = false;
bool Globals::IncludeWinMemory = false;
bool Globals::InitialisedStatic = false;
CmdInterpreter* Globals::Interpreter = NULL;
char Globals::IpAddress[] = "";
Connection* Globals::LogTarget = NULL;
bool Globals::NetworkAvailable = false;
IoTManager* Globals::ObjectMgr = NULL;
Register* Globals::ObjectRegister = NULL;
char Globals::OneCommandPrefix[] = "";
PartManager* Globals::PartMgr = NULL;
//FifoBuffer *Globals::RequestBuffer = NULL;
bool Globals::RtcAvailable = false;
char Globals::SpecialFieldPrefix[] = "";
bool Globals::UserExit = false;
int Globals::Verbosity = 3;

#ifdef ARDUINO
	int Globals::SerialSpeed;
#else
	char Globals::AppDocsFolder[] = "";
	char Globals::HostName[] = "";
	bool Globals::WinsockStarted = false;
#endif

#ifdef ARDJACK_INCLUDE_BEACONS
	BeaconManager* Globals::BeaconMgr = NULL;
#endif

#ifdef ARDJACK_INCLUDE_BRIDGES
	BridgeManager* Globals::BridgeMgr = NULL;
#endif

#ifdef ARDJACK_INCLUDE_DATALOGGERS
	DataLoggerManager* Globals::DataLoggerMgr = NULL;
#endif

#ifdef ARDJACK_INCLUDE_PERSISTENCE
	PersistentFileManager* Globals::PersistentFileMgr = NULL;
#endif

#ifdef ARDJACK_INCLUDE_SHIELDS
	ShieldManager* Globals::ShieldMgr = NULL;
#endif

#ifdef ARDJACK_NETWORK_AVAILABLE
	NetworkManager* Globals::NetworkMgr = NULL;
#endif



bool Globals::ActivateObjects(const char* args, bool state)
{
	// Activate or Deactivate zero or more Objects.
	char action[20];
	char command[20];
	char name[ARDJACK_MAX_NAME_LENGTH];
	char work[ARDJACK_MAX_COMMAND_LENGTH];

	strcpy(work, args);
	strcpy(action, state ? PRM("Activating") : PRM("Deactivating"));
	strcpy(command, state ? PRM("ACTIVATE") : PRM("DEACTIVATE"));

	while (strlen(work) > 0)
	{
		if (!Utils::GetFirstField(work, ' ', name, ARDJACK_MAX_VALUE_LENGTH, true))
			break;

		IoTObject* obj = ObjectRegister->LookupName(name);
		if (NULL == obj)
		{
			Log::LogError(command, PRM(": No such object: '"), name, "'");
			return false;
		}

		if (state != obj->Active())
		{
			Log::LogInfo(action, PRM(" object '"), obj->Name, "' (a ", ObjectRegister->ObjectTypeName(obj->Type), ")");

			if (!obj->SetActive(state))
				return false;
		}
	}

	return true;
}


IoTObject* Globals::AddObject(const char* args)
{
	// add type name subtype
	// E.g.
	//		add connection udp0 udp
	//		add wifi wifi0

	StringList fields;
	int count = Utils::SplitText(args, ' ', &fields, ARDJACK_MAX_VALUES, ARDJACK_MAX_VALUE_LENGTH);

	if (count < 2)
	{
		Log::LogError(PRM("Insufficient arguments for ADD command: "), args);
		return NULL;
	}

	// Get the object type.
	char typeName[ARDJACK_MAX_NAME_LENGTH];
	strcpy(typeName, fields.Get(0));

	int type = ObjectRegister->LookupObjectType(typeName, ARDJACK_OBJECT_TYPE_UNKNOWN);
	if (type == ARDJACK_OBJECT_TYPE_UNKNOWN)
	{
		Log::LogError(PRM("Invalid object type: '"), fields.Get(0), "' in '", args, "'");
		return NULL;
	}

	// Get the object name.
	char name[ARDJACK_MAX_NAME_LENGTH];
	strcpy(name, fields.Get(1));

	// Get the object subtype.
	char subtypeName[ARDJACK_MAX_NAME_LENGTH];
	strcpy(subtypeName, (count < 3) ? "Basic" : fields.Get(2));
	int subtype;

	switch (type)
	{
	case ARDJACK_OBJECT_TYPE_CONNECTION:
		subtype = ConnectionMgr->LookupSubtype(subtypeName);
		break;

	case ARDJACK_OBJECT_TYPE_DEVICE:
		subtype = DeviceMgr->LookupSubtype(subtypeName);
		break;

#ifdef ARDJACK_NETWORK_AVAILABLE
	case ARDJACK_OBJECT_TYPE_NETWORKINTERFACE:
		subtype = NetworkMgr->LookupSubtype(subtypeName);
		break;
#endif

#ifdef ARDJACK_INCLUDE_SHIELDS
	case ARDJACK_OBJECT_TYPE_SHIELD:
		subtype = ShieldMgr->LookupSubtype(subtypeName);
		break;
#endif

	default:
		subtype = ARDJACK_OBJECT_SUBTYPE_BASIC;
		break;
	}

	if (subtype == ARDJACK_OBJECT_SUBTYPE_UNKNOWN)
	{
		Log::LogError(PRM("Invalid object subtype: "), args);
		return NULL;
	}

	return AddObject(type, subtype, name);
}

	
IoTObject* Globals::AddObject(int type, int subtype, const char* name)
{
	// Create the object.
	IoTObject* result = ObjectRegister->LookupName(name);

	if (NULL != result)
	{
		Log::LogWarning(PRM("AddObject: Object already exists: '"), name, "'");
		return result;
	}

	return ObjectRegister->CreateObject(type, subtype, name);
}


bool Globals::CheckCommandBuffer()
{
	if (CommandBuffer->IsEmpty())
		return true;

	CommandBufferItem item;

	if (!CommandBuffer->Pop(&item))
		return true;

	if (Globals::Verbosity > 7)
		Log::LogInfoF(PRM("Globals::CheckCommandBuffer: '%s'"), item.Text);

	return Interpreter->Execute(item.Text);
}


bool Globals::CheckDeviceBuffer()
{
	if (DeviceBuffer->IsEmpty())
		return true;

	CommandBufferItem item;

	if (!DeviceBuffer->Pop(&item))
		return true;

	if (NULL == item.Dev)
	{
		if (Globals::Verbosity > 7)
			Log::LogWarning(PRM("CheckDeviceBuffer: '"), item.Text, "' - NO DEVICE");
		return false;
	}

	if (Globals::Verbosity > 7)
		Log::LogInfo(PRM("CheckDeviceBuffer: Device '"), item.Dev->Name, "', '", item.Text, "'");

	return Globals::HandleDeviceRequest(item.Dev, item.Text);
}


bool Globals::DeleteObjects(const char* args)
{
	// Remove one or more 'IoTObjects'.
	char name[ARDJACK_MAX_NAME_LENGTH];
	char work[202];
	strcpy(work, args);

	while (strlen(work) > 0)
	{
		if (!Utils::GetFirstField(work, ' ', name, ARDJACK_MAX_NAME_LENGTH, true))
			break;

		if (Utils::StringEquals(name, "*"))
		{
			while (ObjectRegister->ObjectCount > 0)
			{
				IoTObject* obj = ObjectRegister->Objects[0];

				if (!DeleteSingleObject(obj))
					return false;
			}
		}
		else
		{
			IoTObject* obj = ObjectRegister->LookupName(name);

			if (NULL == obj)
			{
				Log::LogError(PRM("Globals::DeleteObjects: No such object: "), name);
				return false;
			}

			if (!DeleteSingleObject(obj))
				return false;
		}
	}

	return true;
}


bool Globals::DeleteSingleObject(IoTObject* obj)
{
	if (!obj->SetActive(false))
		return false;

	// Wait a bit for interrupts/polls to finish.
	Utils::DelayMs(500);

	// Delete the object.
	if (Globals::Verbosity > 3)
		Log::LogInfo(PRM("Globals::DeleteSingleObject: Deleting: "), obj->Name);

	return ObjectRegister->DeleteObject(obj);
}


bool Globals::HandleDeviceRequest(Device* dev, const char* line)
{
	// Create a request message to assist development.
	if (Globals::Verbosity > 5)
		Log::LogInfo(PRM("Globals::HandleDeviceRequest: "), dev->Name, ": '", line, "'");

	return DeviceMgr->HandleDeviceRequest(dev, line);
}


bool Globals::Init()
{
	Log::LogInfo(PRM("Globals::Init"));

	// TEMPORARY:
#ifdef ARDUINO
	strcpy(AppName, PRM("ArdJack"));
	strcpy(ComputerName, ARDJACK_BOARD_TYPE);
	_strlwr(ComputerName);
#else
	strcpy(AppName, PRM("ArdJackW"));
	Utils::GetAppDocsFolder(AppDocsFolder, MAX_PATH);
	GetEnvironmentVariable(PRM("COMPUTERNAME"), ComputerName, ARDJACK_MAX_NAME_LENGTH);

	// Modify the name to distinguish this program when used with IoTJack.
	strcat(ComputerName, PRM("_A"));
#endif

	CommandBuffer = new FifoBuffer(sizeof(CommandBufferItem), ARDJACK_MAX_COMMAND_BUFFER_ITEMS);
	strcpy(CommandPrefix, "$cc:");
	strcpy(CommandSeparator, ";");
	CommandSet0 = new CommandSet();
	strcpy(CommentPrefix, "#");
	ConnectionMgr = new ConnectionManager();
	DeviceBuffer = new FifoBuffer(sizeof(CommandBufferItem), ARDJACK_MAX_DEVICE_BUFFER_ITEMS);
	DeviceCodec = new DeviceCodec1();
	DeviceMgr = new DeviceManager();
	EnableSound = true;
	strcpy(ExecPrefix, "@");
	FilterMgr = new FilterManager();
	strcpy(FromName, AppName);
	Interpreter = new CmdInterpreter();
	ObjectMgr = new IoTManager();
	ObjectRegister = new Register();
	strcpy(OneCommandPrefix, "|");
	PartMgr = new PartManager();
	//RequestBuffer = new FifoBuffer(?, ?);
	strcpy(SpecialFieldPrefix, "$");

	// Create optional Managers.
#ifdef ARDJACK_INCLUDE_BEACONS
	BeaconMgr = new BeaconManager();
#endif

#ifdef ARDJACK_INCLUDE_BRIDGES
	BridgeMgr = new BridgeManager();
#endif

#ifdef ARDJACK_INCLUDE_DATALOGGERS
	DataLoggerMgr = new DataLoggerManager();
#endif

#ifdef ARDJACK_INCLUDE_PERSISTENCE
	PersistentFileMgr = new PersistentFileManager();
#endif

#ifdef ARDJACK_INCLUDE_SHIELDS
	ShieldMgr = new ShieldManager();
#endif

#ifdef ARDJACK_NETWORK_AVAILABLE
	NetworkMgr = new NetworkManager();
#endif

#ifdef ARDUINO
	SerialSpeed = 115200;
#else
	HostName[0] = NULL;
	WinsockStarted = false;
#endif

	// Set 'Include' global variables from #define conditionals.
#ifdef ARDJACK_INCLUDE_ARDUINO_DHT
	IncludeArduinoDHT = true;
#endif

#ifdef ARDJACK_INCLUDE_ARDUINO_MF_SHIELD
	IncludeArduinoMFShield = true;
#endif

#ifdef ARDJACK_INCLUDE_ARDUINO_NEOPIXEL
	IncludeArduinoNeoPixel = true;
#endif

#ifdef ARDJACK_INCLUDE_BEACONS
	IncludeBeacons = true;
#endif

#ifdef ARDJACK_INCLUDE_BRIDGES
	IncludeBridges = true;
#endif

#ifdef ARDJACK_INCLUDE_DATALOGGERS
	IncludeDataLoggers = true;
#endif

#ifdef ARDJACK_INCLUDE_PERSISTENCE
	IncludePersistence = true;
#endif

#ifdef ARDJACK_INCLUDE_SHIELDS
	IncludeShields = true;
#endif

#ifdef ARDJACK_INCLUDE_WINDISK
	IncludeWinDisk = true;
#endif

#ifdef ARDJACK_INCLUDE_WINMEMORY
	IncludeWinMemory = true;
#endif

#ifdef ARDJACK_NETWORK_AVAILABLE
	NetworkAvailable = true;
#endif

	// Create 'Clock'.
	RtcAvailable = false;

#ifdef ARDUINO
	#ifdef ARDJACK_RTC_AVAILABLE
		Clock = new RtcClock();
		RtcAvailable = true;
	#else
		Clock = new ArduinoClock();
	#endif
#else
	Clock = new WinClock();
#endif

	Interpreter->AddCommandSet(CommandSet0);

	return true;
}


bool Globals::QueueCommand(const char* line)
{
	// Store 'line' in a 'CommandBufferItem' created on the stack.
	// 'CommandBuffer' will copy the item.

	CommandBufferItem item;
	strcpy(item.Text, line);

	if (Globals::Verbosity > 4)
		Log::LogInfo(PRM("Globals::QueueCommand: "), line);

	CommandBuffer->Push(&item);

	return true;
}


bool Globals::ReactivateObjects(const char* args)
{
	// Reactivate (Deactivate + Activate) zero or more Objects.
	if (!ActivateObjects(args, false))
		return false;

	if (!ActivateObjects(args, true))
		return false;

	return true;
}


bool Globals::Set(const char* name, const char* value, bool* handled)
{
	// Set setting 'name' to 'value'.
	*handled = true;

	char useName[82];
	strcpy(useName, name);
	_strupr(useName);

	//if (Utils::StringEquals(useName, PRM("AUTOCLEARCOMMANDLINE"), false) || Utils::StringEquals(useName, PRM("ACCL"), false))
	//{
	//	Globals::AutoClearCommandLine = Utils::String2Bool(value, Globals::AutoClearCommandLine);
	//	return true;
	//}

	if (Utils::StringEquals(useName, PRM("COMMANDPREFIX"), false) || Utils::StringEquals(useName, PRM("CP"), false))
	{
		strcpy(Globals::CommandPrefix, value);
		return true;
	}

	if (Utils::StringEquals(useName, PRM("COMMANDSEPARATOR"), false) || Utils::StringEquals(useName, PRM("CS"), false))
	{
		strcpy(Globals::CommandSeparator, value);
		return true;
	}

	if (Utils::StringEquals(useName, PRM("COMMENTPREFIX"), false) || Utils::StringEquals(useName, PRM("COP"), false))
	{
		strcpy(Globals::CommentPrefix, value);
		return true;
	}

	if (Utils::StringEquals(useName, PRM("DATE"), false))
	{
		// Set the computer date.
		Utils::SetDate(value);
		return true;
	}

	// Used?
	//if (Utils::StringEquals(useName, PRM("EXECPREFIX"), false))
	//{
	//	strcpy(Globals::ExecPrefix, value);
	//	return true;
	//}

	if (Utils::StringEquals(useName, PRM("LOGMEMORY"), false))
	{
		Log::IncludeMemory = Utils::String2Bool(value, Log::IncludeMemory);
		return true;
	}

	if (Utils::StringEquals(useName, PRM("LOGPREFIX"), false))
	{
		strcpy(Log::Prefix, value);
		return true;
	}

	if (Utils::StringEquals(useName, PRM("LOGSERIAL"), false))
	{
		Log::UseSerial = Utils::String2Bool(value, Log::UseSerial);
		return true;
	}

	if (Utils::StringEquals(useName, PRM("LOGSERIALUSB"), false))
	{
		Log::UseSerialUSB = Utils::String2Bool(value, Log::UseSerialUSB);
		return true;
	}

	if (Utils::StringEquals(useName, PRM("LOGTIME"), false))
	{
		Log::IncludeTime = Utils::String2Bool(value, Log::IncludeTime);
		return true;
	}

	if (Utils::StringEquals(useName, PRM("LOGTO"), false))
	{
		Globals::LogTarget = Globals::ConnectionMgr->LookupConnection(value);

		if (Globals::Verbosity > 3)
		{
			char temp[220];

			if (NULL == Globals::LogTarget)
				sprintf(temp, PRM("LOGTO set to (null)"));
			else
				sprintf(temp, PRM("LOGTO set to '%s'"), Globals::LogTarget->Name);

			Log::LogInfo(temp);
		}

		return true;
	}

	if (Utils::StringEquals(useName, PRM("NAME"), false))
	{
		// Set the computer name.
		strcpy(Globals::ComputerName, value);
		return true;
	}

	if (Utils::StringEquals(useName, PRM("ONECOMMANDPREFIX"), false) || Utils::StringEquals(useName, PRM("OCP"), false))
	{
		strcpy(Globals:: OneCommandPrefix, value);
		return true;
	}

	if (Utils::StringEquals(useName, PRM("TIME"), false))
	{
		// Set the computer time.
		Utils::SetTime(value);
		return true;
	}

	if (Utils::StringEquals(useName, PRM("VERBOSITY"), false) || Utils::StringEquals(useName, PRM("V"), false))
	{
		Globals::Verbosity = Utils::String2Int(value, Globals::Verbosity);

		if (Globals::Verbosity > 3)
			Log::LogInfoF(PRM("VERBOSITY set to %d"), Globals::Verbosity);

		return true;
	}

	Log::LogError(PRM("Invalid SET option: "), useName);
	*handled = false;

	return true;
}


bool Globals::SetupStandardRoutes(Connection* pConn)
{
	if (NULL == pConn)
		return false;

	// Add a 'Commands' route.
	Route* route = pConn->AddRoute("Commands", ARDJACK_ROUTE_TYPE_COMMAND, CommandBuffer, CommandPrefix);

	return true;
}


#ifdef ARDJACK_INCLUDE_PERSISTENCE

bool Globals::LoadIniFile(const char* filename)
{
	// Load INI file 'filename'.
	if (Globals::Verbosity > 3)
		Log::LogInfo(PRM("Globals::LoadIniFile: File: "), filename);

	return PersistentFileMgr->LoadIniFile(filename);
}


bool Globals::ReadExecuteCommandFile(PersistentFile* file)
{
	// Read and execute command file 'file'.
	if (Verbosity > 3)
		Log::LogInfo(PRM("Globals::ReadExecuteCommandFile: "), file->Name);

	if (!file->Open("r"))
		return false;

	char line[ARDJACK_PERSISTED_LINE_LENGTH];
	int lineNum = 1;
	char temp[10];

	while (!file->Eof())
	{
		if (!file->Gets(line, ARDJACK_PERSISTED_LINE_LENGTH))
			break;

		if (Verbosity > 3)
		{
			sprintf(temp, PRM("   %2d: "), lineNum++);
			Log::LogInfo(temp, line);
		}

		Interpreter->Execute(line);
	}

	file->Close();

	return true;
}


bool Globals::ReadExecuteStartupFile()
{
	// Read and execute the Startup file (if any).
	PersistentFile* file = PersistentFileMgr->Lookup("startup");
	if (NULL == file) return false;

	Log::LogInfo(PRM("Reading startup file"));

	return ReadExecuteCommandFile(file);
}


bool Globals::SaveIniFile(const char* filename)
{
	// Load INI file 'filename'.
	if (Globals::Verbosity > 3)
		Log::LogInfo(PRM("Globals::SaveIniFile: File: "), filename);

	Globals::PersistentFileMgr->Clear();

	PersistentFile* file = Globals::PersistentFileMgr->Lookup(filename, true);
	if (NULL == file) return false;

	if (!file->Open("w"))
		return false;

	char temp[ARDJACK_PERSISTED_LINE_LENGTH];

	// Write a [General] section.
	file->Puts("[General]");
	file->PutNameValuePair("Name", Globals::ComputerName);
	file->PutNameValuePair("Verbosity", itoa(Globals::Verbosity, temp, 10));
	file->PutNameValuePair("Written", Utils::GetNow(temp));

	// Write an '[Object udp0]' section.
	IoTObject* udp0 = Globals::ObjectRegister->LookupName("udp0");
	if (NULL != udp0)
	{
		file->Puts("[Object udp0]");
		char temp[102];

		if (udp0->Config->GetAsString("InPort", temp))
			file->PutNameValuePair("InPort", temp);

		if (udp0->Config->GetAsString("OutIP", temp))
			file->PutNameValuePair("OutIP", temp);

		if (udp0->Config->GetAsString("OutPort", temp))
			file->PutNameValuePair("OutPort", temp);
	}

	// Write an '[Object wifi0]' section?
	IoTObject* wifi0 = Globals::ObjectRegister->LookupName("wifi0");
	if (NULL != wifi0)
	{
		file->Puts("[Object wifi0]");
		char temp[102];

		if (wifi0->Config->GetAsString("Password", temp))
			file->PutNameValuePair("Password", temp);

		if (wifi0->Config->GetAsString("SSID", temp))
			file->PutNameValuePair("SSID", temp);
	}

	file->Close();

	return true;
}

#endif
