// pch.h

/*
	Problems with headers in compilation?
		Try commenting-out everything below.
*/


#pragma once

#include "DetectBoard.h"


// Utility classes.

#include "ArrayHelpers.h"
#include "ConfigProp.h"
#include "Configuration.h"
#include "cppQueue.h"
#include "DateTime.h"
#include "DeviceCodec1.h"
#include "Dictionary.h"
#include "Dynamic.h"
#include "Enumeration.h"
#include "FieldReplacer.h"
#include "FifoBuffer.h"
#include "Int8List.h"
#include "IoTMessage.h"
#include "Log.h"
#include "MemoryFreeExt.h"
#include "MessageFilter.h"
#include "MessageFilterItem.h"
#include "Route.h"
#include "StringList.h"
#include "Utils.h"


// Globals.

#include "Globals.h"


// User Objects.

#include "IoTObject.h"

#include "Beacon.h"
#include "Bridge.h"
#include "Connection.h"
#include "DataLogger.h"
#include "Device.h"
#include "Filter.h"
#include "Part.h"
#include "Shield.h"


// User Object Managers.

#include "IoTManager.h"

#include "BeaconManager.h"
#include "BridgeManager.h"
#include "ConnectionManager.h"
#include "DataLoggerManager.h"
#include "DeviceManager.h"
#include "NetworkManager.h"
#include "PartManager.h"
#include "ShieldManager.h"


// Subclasses of User Objects.

#include "WiFiLibrary.h"

#include "ArduinoDevice.h"
#include "ArduinoDHT.h"
#include "ArduinoMFShield.h"
#include "ArduinoNeoPixel.h"
#include "EthernetInterface.h"
#include "HttpConnection.h"
#include "LogConnection.h"
#include "NetworkInterface.h"
#include "SerialConnection.h"
#include "ThinkerShield.h"
#include "UdpConnection.h"
#include "UserPart.h"
#include "WiFiInterface.h"


// Clocks.

#include "IoTClock.h"

#include "ArduinoClock.h"
#include "RtcClock.h"


// Commands and Command Interpreter.

#include "CmdInterpreter.h"
#include "CommandSet.h"


// The rest.

#include "Displayer.h"
#include "PersistentFile.h"
#include "PersistentFileManager.h"
#include "Register.h"
#include "Tests.h"

