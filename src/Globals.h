/*
	Globals.h

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

#pragma once

#ifdef ARDUINO
	#include <arduino.h>
	#include "DetectBoard.h"

	#define _strlwr strlwr
	#define _strupr strupr
#else
	#include "stdafx.h"
#endif

#ifndef MAX_PATH
	#define MAX_PATH 260
#endif



// Include required Shields, UserParts and features.

#undef ARDJACK_INCLUDE_ARDUINO_DHT
#undef ARDJACK_INCLUDE_ARDUINO_MF_SHIELD
#undef ARDJACK_INCLUDE_ARDUINO_NEOPIXEL
#undef ARDJACK_INCLUDE_BEACONS
#undef ARDJACK_INCLUDE_BRIDGES
#undef ARDJACK_INCLUDE_DATALOGGERS
#undef ARDJACK_INCLUDE_MULTI_PARTS
#undef ARDJACK_INCLUDE_PERSISTENCE
#undef ARDJACK_INCLUDE_SHIELDS
#undef ARDJACK_INCLUDE_TESTS
#undef ARDJACK_INCLUDE_THINKER_SHIELD
#undef ARDJACK_INCLUDE_WINDISK
#undef ARDJACK_INCLUDE_WINMEMORY


#ifdef ARDUINO
	#define ARDJACK_INCLUDE_ARDUINO_DHT
	//#define ARDJACK_INCLUDE_ARDUINO_MF_SHIELD
	//#define ARDJACK_INCLUDE_ARDUINO_NEOPIXEL
	#define ARDJACK_INCLUDE_BEACONS
	#define ARDJACK_INCLUDE_BRIDGES
	#define ARDJACK_INCLUDE_DATALOGGERS
	//#define ARDJACK_INCLUDE_MULTI_PARTS
	//#define ARDJACK_INCLUDE_PERSISTENCE
	#define ARDJACK_INCLUDE_SHIELDS
	//#define ARDJACK_INCLUDE_TESTS
	#define ARDJACK_INCLUDE_THINKER_SHIELD
#else
	#define ARDJACK_INCLUDE_BEACONS
	#define ARDJACK_INCLUDE_BRIDGES
	#define ARDJACK_INCLUDE_DATALOGGERS
	//#define ARDJACK_INCLUDE_MULTI_PARTS
	#define ARDJACK_INCLUDE_PERSISTENCE
	#define ARDJACK_INCLUDE_SHIELDS
	#define ARDJACK_INCLUDE_TESTS
	#define ARDJACK_INCLUDE_THINKER_SHIELD
	#define ARDJACK_INCLUDE_WINDISK
	#define ARDJACK_INCLUDE_WINMEMORY

	#define ARDJACK_NETWORK_AVAILABLE
#endif


// Global limits / constants.

#define ARDJACK_MAX_COMMAND_BUFFER_ITEM_LENGTH 100					// max.no.of characters in a command
#define ARDJACK_MAX_COMMAND_BUFFER_ITEMS 10							// max.items in a command buffer
#define ARDJACK_MAX_COMMAND_LENGTH 110								// max.characters in a command
#define ARDJACK_MAX_COMMAND_SETS 1									// max.no.of CommandSets
#define ARDJACK_MAX_COMMANDS 24										// max.no.of commands
#define ARDJACK_MAX_CONFIG_PROPERTIES 30							// max.no.of items in a Configuration
#define ARDJACK_MAX_CONFIG_VALUE_LENGTH 40							// max.characters in a ConfigProp string
#define ARDJACK_MAX_CONNECTION_OUTPUT_BUFFER_ITEM_LENGTH 200		// max.characters in a Connection o/p buffer item
#define ARDJACK_MAX_CONNECTION_OUTPUT_BUFFER_ITEMS 10				// max.items in the Connection o/p buffer
#define ARDJACK_MAX_DATALOGGER_PARTS 10
#define ARDJACK_MAX_DESCRIPTION_LENGTH 60							// max.characters in a description
#define ARDJACK_MAX_DEVICE_BUFFER_ITEMS 10							// max.items in a device buffer
#define ARDJACK_MAX_DICTIONARY_ITEMS 6
#define ARDJACK_MAX_DICTIONARY_KEY_LENGTH 10
#define ARDJACK_MAX_DICTIONARY_VALUE_LENGTH 20
#define ARDJACK_MAX_DYNAMIC_STRING_LENGTH 20
#define ARDJACK_MAX_ENUMERATION_ITEM_LENGTH 20						// max.characters in an Enumeration item
#define ARDJACK_MAX_ENUMERATION_ITEMS 60							// max.no.of items in an Enumeration
#define ARDJACK_MAX_INPUT_ROUTES 4
// TEMPORARY:
#define ARDJACK_MAX_LOG_BUFFER_ITEM_LENGTH 4
#define ARDJACK_MAX_LOG_BUFFER_ITEMS 1
#define ARDJACK_MAX_MACRO_LENGTH 100								// max.characters in a Macro
#define ARDJACK_MAX_MACROS 30										// max.no.of Macros
#define ARDJACK_MAX_MESSAGE_PATH_LENGTH 30							// max.characters in an IoTMessage path
#define ARDJACK_MAX_MESSAGE_TEXT_LENGTH 160							// max.characters in IoTMessage 'Text'
#define ARDJACK_MAX_MESSAGE_WIRETEXT_LENGTH 220						// max.characters in IoTMessage 'WireText'
#define ARDJACK_MAX_MULTI_PART_ITEMS 1								// max.no.of Items in a 'Multi' Part
#define ARDJACK_MAX_NAME_LENGTH 32									// max.characters in a name
#define ARDJACK_MAX_OBJECTS 20										// max.no.of Objects in Register
#define ARDJACK_MAX_PART_VALUE_LENGTH 10							// max.characters in a Part's value
#define ARDJACK_MAX_PARTS 30										// max.no.of Parts
#define ARDJACK_MAX_PERSISTED_FILES 2
#define ARDJACK_MAX_PERSISTED_LINES 40
#define ARDJACK_MAX_TABLE_COLUMNS 10
#define ARDJACK_MAX_VALUE_LENGTH 120
#define ARDJACK_MAX_VALUES 10
#define ARDJACK_MAX_VERB_LENGTH 12

#define ARDJACK_PERSISTED_LINE_LENGTH 256

#ifdef ARDUINO
	#define ARDJACK_MAX_COMMAND_BUFFER_ITEMS 4							// max.items in a command buffer
	#define ARDJACK_MAX_CONNECTION_OUTPUT_BUFFER_ITEM_LENGTH 200		// max.characters in a Connection o/p buffer item
	#define ARDJACK_MAX_CONNECTION_OUTPUT_BUFFER_ITEMS 4				// max.items in the Connection o/p buffer
	#define ARDJACK_MAX_DEVICE_BUFFER_ITEMS 4							// max.items in a device buffer

	#ifdef __arm__
		#define ARDJACK_MAX_DATALOGGER_PARTS 8
		#define ARDJACK_MAX_MACROS 6
		#define ARDJACK_MAX_NAME_LENGTH 24
	#else
		#define ARDJACK_MAX_CONFIG_PROPERTIES 30
		//#define ARDJACK_MAX_CONFIG_VALUE_LENGTH 24
		#define ARDJACK_MAX_DATALOGGER_PARTS 6
		#define ARDJACK_MAX_DESCRIPTION_LENGTH 60
		#define ARDJACK_MAX_DICTIONARY_ITEMS 6
		#define ARDJACK_MAX_ENUMERATION_ITEMS 34
		#define ARDJACK_MAX_MACRO_LENGTH 80
		#define ARDJACK_MAX_MACROS 6
		//#define ARDJACK_MAX_MESSAGE_TEXT_LENGTH 160
		//#define ARDJACK_MAX_MESSAGE_WIRETEXT_LENGTH 160
		#define ARDJACK_MAX_MULTI_PART_ITEMS 1
		#define ARDJACK_MAX_NAME_LENGTH 24
		#define ARDJACK_MAX_OBJECTS 12
		#define ARDJACK_MAX_VALUE_LENGTH 60
		#define ARDJACK_MAX_VALUES 10
	#endif
#endif

#define ARDJACK_BYTES_PER_KB 1024
#define ARDJACK_BYTES_PER_MB (1024 * 1024)
#define ARDJACK_BYTES_PER_GB (1024 * 1024 * 1024)


#include "DateTime.h"
#include "FifoBuffer.h"
#include "Register.h"



// Forward declarations.
#ifdef ARDJACK_INCLUDE_BEACONS
	class Beacon;
	class BeaconManager;
#endif

#ifdef ARDJACK_INCLUDE_BRIDGES
	class Bridge;
	class BridgeManager;
#endif

class CmdInterpreter;
class CommandSet;
class Connection;
class ConnectionManager;

#ifdef ARDJACK_INCLUDE_DATALOGGERS
	class DataLogger;
	class DataLoggerManager;
#endif

class Device;
class DeviceCodec1;
class DeviceManager;
class Filter;
class FilterManager;
class IoTClock;
class IoTManager;
class IoTMessage;

#ifdef ARDJACK_NETWORK_AVAILABLE
	class NetworkInterface;
	class NetworkManager;
#endif

class Part;
class PartManager;

#ifdef ARDJACK_INCLUDE_PERSISTENCE
	class PersistentFile;
	class PersistentFileManager;
#endif

class ShieldManager;
class UserPart;


// Setup string storage and RTC.
#ifdef ARDUINO
	#define PRM(string_literal) ((const PROGMEM char *)(string_literal))

	#ifdef ARDJACK_RTC_AVAILABLE
		#include "RtcClock.h"
	#else
		#include "ArduinoClock.h"
	#endif
#else
	#define PRM(string_literal) ((const char* )(string_literal))
#endif


// Arduino DHT model types.
const static int ARDJACK_ARDUINO_DHT_MODEL_DHT11 = 0;
const static int ARDJACK_ARDUINO_DHT_MODEL_DHT22 = 1;

// Arduino DHT variable types.
const static int ARDJACK_ARDUINO_DHT_VAR_TEMPERATURE = 0;
const static int ARDJACK_ARDUINO_DHT_VAR_HUMIDITY = 1;

// Bridge Direction types.
const static int ARDJACK_BRIDGE_DIRECTION_BIDIRECTIONAL = 0;
const static int ARDJACK_BRIDGE_DIRECTION_UNIDIRECTIONAL_1TO2 = 1;
const static int ARDJACK_BRIDGE_DIRECTION_UNIDIRECTIONAL_2TO1 = 2;

// Connection subtypes.
#ifdef ARDUINO
#else
	const static int ARDJACK_CONNECTION_SUBTYPE_CLIPBOARD = 0;
#endif
const static int ARDJACK_CONNECTION_SUBTYPE_HTTP = 1;
const static int ARDJACK_CONNECTION_SUBTYPE_LOG = 2;
const static int ARDJACK_CONNECTION_SUBTYPE_SERIAL = 3;
const static int ARDJACK_CONNECTION_SUBTYPE_TCP = 4;
const static int ARDJACK_CONNECTION_SUBTYPE_UDP = 5;

// Data types for 'ConfigProp' and 'Dynamic'.
const static int ARDJACK_DATATYPE_EMPTY = 0;
const static int ARDJACK_DATATYPE_BOOLEAN = 1;
const static int ARDJACK_DATATYPE_DATETIME = 2;
const static int ARDJACK_DATATYPE_INTEGER = 3;
const static int ARDJACK_DATATYPE_REAL = 4;
const static int ARDJACK_DATATYPE_STRING = 5;

// Device subtypes.
const static int ARDJACK_DEVICE_SUBTYPE_ARDUINO = 0;
const static int ARDJACK_DEVICE_SUBTYPE_VELLEMANK8055 = 1;
const static int ARDJACK_DEVICE_SUBTYPE_WINDOWS = 2;

// Horizontal Alignment types.
const static int ARDJACK_HORZ_ALIGN_CENTRE = 0;
const static int ARDJACK_HORZ_ALIGN_LEFT = 1;
const static int ARDJACK_HORZ_ALIGN_RIGHT = 2;

// Device Info types.
const static int ARDJACK_INFO_DEVICE_CLASS = 0;
const static int ARDJACK_INFO_DEVICE_TYPE = 1;
const static int ARDJACK_INFO_DEVICE_VERSION = 2;

// Message Format type.
const static int ARDJACK_MESSAGE_FORMAT_0 = 0;
const static int ARDJACK_MESSAGE_FORMAT_1 = 1;

// Message Path type.
const static int ARDJACK_MESSAGE_PATH_DEVICE = 0;
const static int ARDJACK_MESSAGE_PATH_NONE = 1;

// Message type.
const static int ARDJACK_MESSAGE_TYPE_COMMAND = 0;
const static int ARDJACK_MESSAGE_TYPE_DATA = 1;
const static int ARDJACK_MESSAGE_TYPE_HEARTBEAT = 2;
const static int ARDJACK_MESSAGE_TYPE_NONE = 3;
const static int ARDJACK_MESSAGE_TYPE_NOTIFICATION = 4;
const static int ARDJACK_MESSAGE_TYPE_REQUEST = 5;
const static int ARDJACK_MESSAGE_TYPE_RESPONSE = 6;

// Time constants.
const static int ARDJACK_MILLISECONDS_PER_SECOND = 1000;
const static int ARDJACK_SECONDS_PER_MINUTE = 60;
const static int ARDJACK_MINUTES_PER_HOUR = 60;
const static int ARDJACK_HOURS_PER_DAY = 24;

// Network Interface subtypes.
const static int ARDJACK_NETWORK_SUBTYPE_ETHERNET = 0;
const static int ARDJACK_NETWORK_SUBTYPE_UNKNOWN = 1;
const static int ARDJACK_NETWORK_SUBTYPE_WIFI = 2;

// Object subtypes, other than for Connections, Devices, Network Interfaces and Shields (which have specific subtypes).
const static int ARDJACK_OBJECT_SUBTYPE_BASIC = 0;
const static int ARDJACK_OBJECT_SUBTYPE_UNKNOWN = 127;

// Object types.
const static int ARDJACK_OBJECT_TYPE_BEACON = 0;
const static int ARDJACK_OBJECT_TYPE_BRIDGE = 1;
const static int ARDJACK_OBJECT_TYPE_CELL = 2;									// NOT IMPLEMENTED
const static int ARDJACK_OBJECT_TYPE_CONNECTION = 3;
const static int ARDJACK_OBJECT_TYPE_DATALOGGER = 4;
const static int ARDJACK_OBJECT_TYPE_DEVICE = 5;
const static int ARDJACK_OBJECT_TYPE_FILTER = 6;
const static int ARDJACK_OBJECT_TYPE_NETWORKINTERFACE = 7;
const static int ARDJACK_OBJECT_TYPE_OBJECT = 8;
const static int ARDJACK_OBJECT_TYPE_SHIELD = 9;
const static int ARDJACK_OBJECT_TYPE_TRIGGER = 10;								// NOT IMPLEMENTED
const static int ARDJACK_OBJECT_TYPE_UNKNOWN = 127;

// Device Operation types.
const static int ARDJACK_OPERATION_ACTIVATE = 0;					// activate the Device
const static int ARDJACK_OPERATION_ADD = 1;							// add a Part to the Device
const static int ARDJACK_OPERATION_BEEP = 2;						// make the Device beep
const static int ARDJACK_OPERATION_CLEAR = 3;						// clear the inventory on the Device (remove all Parts)
const static int ARDJACK_OPERATION_CONFIGURE = 4;					// configure the Device
const static int ARDJACK_OPERATION_CONFIGUREPART = 5;				// configure a Part or Part type on the Device
const static int ARDJACK_OPERATION_DEACTIVATE = 6;					// activate the Device
const static int ARDJACK_OPERATION_ERROR = 7;						// an error occurred
const static int ARDJACK_OPERATION_FLASH = 8;						// flash a Part on the Device
const static int ARDJACK_OPERATION_GET_CONFIG = 9;					// get one property of the Device's configuration
const static int ARDJACK_OPERATION_GET_COUNT = 10;					// get the count of a Part type on the Device
const static int ARDJACK_OPERATION_GET_GLOBAL = 11;					// get a global setting (of the computer)
const static int ARDJACK_OPERATION_GET_INFO = 12;					// get Device info
const static int ARDJACK_OPERATION_GET_INVENTORY = 13;				// get the Device inventory
const static int ARDJACK_OPERATION_GET_PART_CONFIG = 14;			// get all of the configuration of a Part on the Device
const static int ARDJACK_OPERATION_NONE = 15;
const static int ARDJACK_OPERATION_REACTIVATE = 16;					// reactivate = deactivate + activate
const static int ARDJACK_OPERATION_READ = 17;						// read a Part on the Device
const static int ARDJACK_OPERATION_SET_GLOBAL = 18;					// set a global setting (of the computer)
const static int ARDJACK_OPERATION_SUBSCRIBE = 19;					// subscribe to change notifications from the Device
const static int ARDJACK_OPERATION_SUBSCRIBED = 20;					// the Device signals that a Part or Part type is 'subscribed' to
const static int ARDJACK_OPERATION_UNSUBSCRIBE = 21;				// unsubscribe to change notifications from the Device
const static int ARDJACK_OPERATION_UNSUBSCRIBED = 22;				// the Device signals that a Part or Part type is 'unsubscribed'
const static int ARDJACK_OPERATION_UPDATE = 23;						// update the Device
const static int ARDJACK_OPERATION_WRITE = 24;						// write to a Part on the Device

// Part types.
const static int ARDJACK_PART_TYPE_ACCELEROMETER = 0;
const static int ARDJACK_PART_TYPE_ANALOG_INPUT = 1;
const static int ARDJACK_PART_TYPE_ANALOG_OUTPUT = 2;
const static int ARDJACK_PART_TYPE_BUTTON = 3;
const static int ARDJACK_PART_TYPE_DEVICE = 4;
const static int ARDJACK_PART_TYPE_DIGITAL_INPUT = 5;
const static int ARDJACK_PART_TYPE_DIGITAL_OUTPUT = 6;
const static int ARDJACK_PART_TYPE_DISK_SPACE = 7;
const static int ARDJACK_PART_TYPE_GRAPHIC_DISPLAY = 8;
const static int ARDJACK_PART_TYPE_GYROSCOPE = 9;
const static int ARDJACK_PART_TYPE_HUMIDITY_SENSOR = 10;
const static int ARDJACK_PART_TYPE_I2C = 11;
const static int ARDJACK_PART_TYPE_KEYBOARD = 12;
const static int ARDJACK_PART_TYPE_KEYPAD = 13;
const static int ARDJACK_PART_TYPE_LED = 14;
const static int ARDJACK_PART_TYPE_LIGHT_SENSOR = 15;
const static int ARDJACK_PART_TYPE_MAGNETIC_SENSOR = 16;
const static int ARDJACK_PART_TYPE_MEMORY = 17;
const static int ARDJACK_PART_TYPE_MOUSE = 18;
const static int ARDJACK_PART_TYPE_MULTI = 19;
const static int ARDJACK_PART_TYPE_NONE = 20;
const static int ARDJACK_PART_TYPE_ORIENTATION = 21;
const static int ARDJACK_PART_TYPE_POTENTIOMETER = 22;
const static int ARDJACK_PART_TYPE_PRESSURE_SENSOR = 23;
const static int ARDJACK_PART_TYPE_PROXIMITY_SENSOR = 24;
const static int ARDJACK_PART_TYPE_SOUND = 25;
const static int ARDJACK_PART_TYPE_SPECIAL = 26;
const static int ARDJACK_PART_TYPE_SWITCH = 27;
const static int ARDJACK_PART_TYPE_TEMPERATURE_SENSOR = 28;
const static int ARDJACK_PART_TYPE_TEXT_DISPLAY = 29;
const static int ARDJACK_PART_TYPE_TOUCHSCREEN = 30;
const static int ARDJACK_PART_TYPE_TTS = 31;
const static int ARDJACK_PART_TYPE_USER = 32;

const static int ARDJACK_MAX_PART_TYPE = ARDJACK_PART_TYPE_USER;
const static int ARDJACK_MIN_PART_TYPE = ARDJACK_PART_TYPE_ACCELEROMETER;

// Route types.
const static int ARDJACK_ROUTE_TYPE_NONE = 0;
const static int ARDJACK_ROUTE_TYPE_COMMAND = 1;
const static int ARDJACK_ROUTE_TYPE_REQUEST = 2;
const static int ARDJACK_ROUTE_TYPE_RESPONSE = 3;

// Shield subtypes.
const static int ARDJACK_SHIELD_SUBTYPE_ARDUINO_MF_SHIELD = 0;
const static int ARDJACK_SHIELD_SUBTYPE_THINKERSHIELD = 1;

// String Comparison type.
const static int ARDJACK_STRING_COMPARE_CONTAINS = 0;
const static int ARDJACK_STRING_COMPARE_ENDS_WITH = 1;
const static int ARDJACK_STRING_COMPARE_EQUALS = 2;
const static int ARDJACK_STRING_COMPARE_FALSE = 3;										// result is always false
const static int ARDJACK_STRING_COMPARE_IN = 4;
const static int ARDJACK_STRING_COMPARE_NOT_CONTAINS = 5;
const static int ARDJACK_STRING_COMPARE_NOT_ENDS_WITH = 6;
const static int ARDJACK_STRING_COMPARE_NOT_EQUALS = 7;
const static int ARDJACK_STRING_COMPARE_NOT_IN = 8;
const static int ARDJACK_STRING_COMPARE_NOT_STARTS_WITH = 9;
const static int ARDJACK_STRING_COMPARE_STARTS_WITH = 10;
const static int ARDJACK_STRING_COMPARE_TRUE = 11;										// result is always true

// UserPart subtypes.
const static int ARDJACK_USERPART_SUBTYPE_NONE = 0;
const static int ARDJACK_USERPART_SUBTYPE_ARDUINO_DHT = 1;
const static int ARDJACK_USERPART_SUBTYPE_ARDUINO_NEOPIXEL = 2;
const static int ARDJACK_USERPART_SUBTYPE_WINDISK = 3;
const static int ARDJACK_USERPART_SUBTYPE_WINMEMORY = 4;

// WiFi connect types.
const static int ARDJACK_WIFI_CONNECT_OPEN = 0;
const static int ARDJACK_WIFI_CONNECT_WEP = 1;
const static int ARDJACK_WIFI_CONNECT_WPA = 2;

// WinDisk subtypes.
const static int ARDJACK_WINDISK_DRIVE_A = 0;
const static int ARDJACK_WINDISK_DRIVE_B = 1;
const static int ARDJACK_WINDISK_DRIVE_C = 2;
const static int ARDJACK_WINDISK_DRIVE_D = 3;
const static int ARDJACK_WINDISK_DRIVE_E = 4;
const static int ARDJACK_WINDISK_DRIVE_F = 5;
const static int ARDJACK_WINDISK_DRIVE_G = 6;
const static int ARDJACK_WINDISK_DRIVE_H = 7;

// WinMemory subtypes.
const static int ARDJACK_WINMEMORY_FREE_PHYSICAL = 0;
const static int ARDJACK_WINMEMORY_TOTAL_PHYSICAL = 1;
const static int ARDJACK_WINMEMORY_FREE_VIRTUAL = 2;


struct CommandBufferItem
{
	Device* Dev = NULL;
	char Text[ARDJACK_MAX_COMMAND_BUFFER_ITEM_LENGTH] = "";
};

struct ConnectionOutputBufferItem
{
	Connection* Conn = NULL;
	char Text[ARDJACK_MAX_CONNECTION_OUTPUT_BUFFER_ITEM_LENGTH] = "";
};

struct MacroDef
{
	char Content[ARDJACK_MAX_MACRO_LENGTH] = "";
	char Name[ARDJACK_MAX_NAME_LENGTH] = "";
};



class Globals
{
protected:

public:
	static char AppName[ARDJACK_MAX_NAME_LENGTH];
	//static bool AutoClearCommandLine;
#ifdef ARDJACK_INCLUDE_BEACONS
	static BeaconManager* BeaconMgr;
#endif
#ifdef ARDJACK_INCLUDE_BRIDGES
	static BridgeManager* BridgeMgr;
#endif
	static IoTClock* Clock;
	static FifoBuffer* CommandBuffer;
	static char CommandPrefix[12];
	static char CommandSeparator[6];
	static CommandSet* CommandSet0;
	static char CommentPrefix[4];
	static char CompileDate[14];
	static char CompileTime[14];
	static char ComputerName[ARDJACK_MAX_NAME_LENGTH];
	static ConnectionManager* ConnectionMgr;
#ifdef ARDJACK_INCLUDE_DATALOGGERS
	static DataLoggerManager* DataLoggerMgr;
#endif
	static FifoBuffer* DeviceBuffer;								// shared by Devices
	static DeviceCodec1* DeviceCodec;
	static DeviceManager* DeviceMgr;
	static char EmptyString[2];
	static bool EnableSound;
	static char ExecPrefix[4];										// prefix for executing a command file
	static FilterManager* FilterMgr;
	static char FromName[20];										// 'from' name to use in messages sent
	static void* HeapBase;
	static bool IncludeArduinoDHT;
	static bool IncludeArduinoMFShield;
	static bool IncludeArduinoNeoPixel;
	static bool IncludeBeacons;
	static bool IncludeBridges;
	static bool IncludeDataLoggers;
	static bool IncludePersistence;
	static bool IncludeShields;
	static bool IncludeThinkerShield;
	static bool IncludeWinDisk;
	static bool IncludeWinMemory;
	static bool InitialisedStatic;									// has static initialzation finished?
	static CmdInterpreter* Interpreter;
	static char IpAddress[20];
	static Connection* LogTarget;									// a Connection to log to
	static bool NetworkAvailable;
#ifdef ARDJACK_NETWORK_AVAILABLE
	static NetworkManager* NetworkMgr;
#endif
	static IoTManager* ObjectMgr;
	static Register* ObjectRegister;
	static char OneCommandPrefix[4];
	static PartManager* PartMgr;
#ifdef ARDJACK_INCLUDE_PERSISTENCE
	static PersistentFileManager* PersistentFileMgr;
#endif
	//static FifoBuffer *RequestBuffer;
	static bool RtcAvailable;
#ifdef ARDJACK_INCLUDE_SHIELDS
	static ShieldManager* ShieldMgr;
#endif
	static char SpecialFieldPrefix[10];
	static bool UserExit;											// user wishes to exit this program
	static int Verbosity;

#ifdef ARDUINO
	static int SerialSpeed;											// baud rate
#else
	static char AppDocsFolder[MAX_PATH];
	static char HostName[ARDJACK_MAX_NAME_LENGTH];
	static bool WinsockStarted;
#endif


	static bool ActivateObjects(const char* args, bool state);
	static IoTObject* AddObject(const char* args);
	static IoTObject* AddObject(int type, int subtype, const char* name);
	static bool CheckCommandBuffer();
	static bool CheckDeviceBuffer();
	static bool DeleteObjects(const char* args);
	static bool DeleteSingleObject(IoTObject* obj);
	static bool HandleDeviceRequest(Device *dev, const char* line);
	static bool Init();
	static bool QueueCommand(const char* line);
	static bool ReactivateObjects(const char* args);
	static bool Set(const char* name, const char* value, bool* handled);
	static bool SetupStandardRoutes(Connection *pConn);

#ifdef ARDJACK_INCLUDE_PERSISTENCE
	static bool LoadIniFile(const char* name);
	static bool ReadExecuteCommandFile(PersistentFile* file);
	static bool ReadExecuteStartupFile();
	static bool SaveIniFile(const char* filename);
#endif
};
