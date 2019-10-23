/*
	ArdJack.ino

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

//-----------------------------------------------------
// SEE IMPORTANT NOTE re. WiFi includes further down.
//-----------------------------------------------------


// ARDUINO ONLY.

#include <arduino.h>

#include "DetectBoard.h"
#include "Globals.h"


#ifdef ARDJACK_INCLUDE_ARDUINO_NEOPIXEL
	#include <Adafruit_NeoPixel.h>
#endif

#ifdef ARDJACK_INCLUDE_ARDUINO_DHT
	#include <SimpleDHT.h>
#endif

#ifdef ARDJACK_INCLUDE_SHIELDS
	#include <MFShield.h>
#endif

#ifdef ARDJACK_ETHERNET_AVAILABLE
	#include <SPI.h>
	#include <Ethernet.h>
#endif

#ifdef ARDJACK_RTC_AVAILABLE
	#include "RtcClock.h"

	//#include <RTCDue.h>
	//#include <RTCZero.h>
#endif

#include "ArduinoDevice.h"
#include "BeaconManager.h"
#include "BridgeManager.h"
#include "CmdInterpreter.h"
#include "ConnectionManager.h"
#include "DataLogger.h"
#include "DataLoggerManager.h"
#include "DateTime.h"
#include "DeviceManager.h"
#include "Displayer.h"
#include "Log.h"
#include "Route.h"
#include "SerialConnection.h"
#include "ShieldManager.h"

#ifdef ARDJACK_INCLUDE_TESTS
  #include "Tests.h"
#endif

//#include "UdpConnection.h"
#include "Utils.h"

#ifdef ARDJACK_FLASH_AVAILABLE
	#include "PersistentFileManager.h"
#endif


//--------------------------------------------------------------------------------------------------------------------------
// IMPORTANT:
//--------------------------------------------------------------------------------------------------------------------------

// FOR VISUAL STUDIO + VISUAL MICRO, YOU NEED TO COMMENT/UNCOMMENT THE FOLLOWING THREE #includes TO MATCH THE BOARD IN USE.
// Tried using the ArdJack conditionals, but weird errors when compiling in Visual Studio + Visual Micro.
//		- Comment all 3 #includes for Arduino Due.
//		- Repeat this in WiFiLibrary.h

// MKR1010 etc.
#include <WiFiNINA.h>

// ESP32 boards.
//#include <WiFi.h>

// Other boards with WiFi.
//#include <WiFi101.h>

//--------------------------------------------------------------------------------------------------------------------------


// Global data.
long _NextCheckNetwork;
DateTime _Now;
Connection* _Ser0;


// Forward declarations.
void CommandCallback(void* caller, Route *route, IoTMessage* msg);
void PollSerial();
bool SetupSerial(int speed);
bool StartupCommands();

#ifdef ARDJACK_NETWORK_AVAILABLE
	bool CheckNetwork();
	void PollNetwork();
#endif



	void setup()
	{
		Globals::InitialisedStatic = true;

		if (!SetupSerial(115200))
			return;

		void* ptr = malloc(2);
		strcpy((char*)ptr, "?");
		Globals::HeapBase = ptr;

		Log::Init();

		Globals::Verbosity = 4;

		// TEMPORARY: for development
		//Globals::Verbosity = 8;
		Log::IncludeMemory = true;

		Globals::Init();
		strcpy(Globals::CompileDate, __DATE__);
		strcpy(Globals::CompileTime, __TIME__);

		// Board-specific setup.
#ifdef ARDJACK_FEATHER_M0
	#ifdef ARDJACK_WIFI_AVAILABLE
		// Configure pins for Adafruit ATWINC1500 Feather.
		WiFi.setPins(8, 7, 4, 2);
	#endif
#endif

		// Create Serial Connection 'ser0'.
		_Ser0 = (Connection*)Globals::AddObject(ARDJACK_OBJECT_TYPE_CONNECTION, ARDJACK_CONNECTION_SUBTYPE_SERIAL, "ser0");
		_Ser0->Config->SetFromString("Speed", Utils::Int2String(Globals::SerialSpeed));

		_Ser0->SetActive(true);

		Globals::Clock->Start(1, 9, 2019, 0, 0, 0);

		if (!Globals::RtcAvailable)
		{
			// No RTC, so set the date and time from the compile time.
			bool handled;
			Globals::Set("date", Globals::CompileDate, &handled);
			Globals::Set("time", Globals::CompileTime, &handled);
		}

		Log::LogInfo("");
		Log::LogInfo(Globals::AppName);
		Log::LogInfo("");
		Log::LogInfo(PRM("Built for board:   "), ARDJACK_BOARD_TYPE);
		Log::LogInfo(PRM("Compiled at:       "), Globals::CompileDate, " ", Globals::CompileTime);
		Log::LogInfo(PRM("Network available: "), Utils::Bool2yesno(Globals::NetworkAvailable));
		Log::LogInfo(PRM("RTC available:     "), Utils::Bool2yesno(Globals::RtcAvailable));
		Log::LogInfo(PRM("millis():          "), Utils::Int2String(millis()));
		char temp[30];
		Log::LogInfo(PRM("Time:              "), Utils::GetNow(temp));
		Log::LogInfo(PRM("Verbosity:         "), Utils::Int2String(Globals::Verbosity));
		Log::LogInfo();

		// Setup predefined Objects.
		Log::LogInfo(PRM("Setting up Arduino Device 'ard'..."));

		Device* ard = (Device*)Globals::AddObject(ARDJACK_OBJECT_TYPE_DEVICE, ARDJACK_DEVICE_SUBTYPE_ARDUINO, "ard");
		ard->OutputConnection = _Ser0;
		ard->SetActive(true);

		Globals::Interpreter->Execute(PRM("set name mkr2"));

#ifdef ARDJACK_FLASH_AVAILABLE
		// Load the Configuration file (if any) from Flash memory.
		Globals::PersistentFileMgr->Scan();
		Globals::LoadIniFile("configuration");
#endif

#ifdef ARDJACK_NETWORK_AVAILABLE
		_NextCheckNetwork = 0;											// in case we need to keep checking for a connection
		CheckNetwork();
#endif

#ifdef ARDJACK_FLASH_AVAILABLE
		// Read the startup file (if any) from Flash memory.
		Globals::ReadExecuteStartupFile();
#endif

		ard->SetActive(false);

		StartupCommands();

		ard->SetActive(true);

		Log::LogInfo();
		Log::LogInfo(PRM("--- READY ---"));
		Log::LogInfo();
	}


	void loop()
	{
		Globals::Clock->Poll();
		Log::Poll();

#ifdef ARDJACK_NETWORK_AVAILABLE
		if (CheckNetwork())
			PollNetwork();
#endif

		PollSerial();
		Globals::CheckCommandBuffer();
		Globals::CheckDeviceBuffer();

#ifdef ARDJACK_INCLUDE_BEACONS
		Globals::BeaconMgr->Poll();
#endif

#ifdef ARDJACK_INCLUDE_BRIDGES
		//Globals::BridgeMgr->Poll();
#endif

		Globals::ConnectionMgr->Poll();

#ifdef ARDJACK_INCLUDE_DATALOGGERS
		Globals::DataLoggerMgr->Poll();
#endif

		Globals::DeviceMgr->Poll();

#ifdef ARDJACK_INCLUDE_SHIELDS
		Globals::ShieldMgr->Poll();
#endif

		Utils::DelayMs(10);
	}


#ifdef ARDJACK_NETWORK_AVAILABLE
	bool CheckNetwork()
	{
		long nowMs = Globals::Clock->NowMs();
		if (nowMs <= _NextCheckNetwork) return true;

		if (_NextCheckNetwork == 0)
			_NextCheckNetwork = nowMs;

		_NextCheckNetwork += 10000;														// check every 10 seconds

		// TODO: Inefficient...
		IoTObject* wifi0 = Globals::ObjectRegister->LookupName("wifi0", true);
		if (NULL == wifi0) return false;

		if (wifi0->Active())
			return true;

		// 'wifi0' exists but it's inactive - has it got an SSID?
		char ssid[ARDJACK_MAX_NAME_LENGTH];
		if (!wifi0->Config->GetAsString("ssid", ssid)) return false;
		if (NULL == ssid[0]) return false;

		// Try to activate the WiFi connection.
		wifi0->SetActive(true);

		return wifi0->Active();
	}
#endif


	void CommandCallback(void* caller, Route* route, IoTMessage* msg)
	{
		if (Globals::Verbosity > 2)
			Log::LogInfo(PRM("CommandCallback: '"), msg->Text(), "'");

		Globals::Interpreter->ExecuteCommand(msg->Text());
	}


#ifdef ARDJACK_NETWORK_AVAILABLE
	void PollNetwork()
	{
		// TODO: Inefficient...
		Connection* udp0 = (Connection*)Globals::ObjectRegister->LookupName("udp0", true);
		if (NULL == udp0) return;

		udp0->Poll();
	}
#endif


	void PollSerial()
	{
		if (NULL != _Ser0)
			_Ser0->PollInputs();
	}


//// DEVELOPMENT:
//void PollSerial()
//{
//	if (SERIAL_PORT_MONITOR.available())
//	{
//		char line[202];
//		int count = SERIAL_PORT_MONITOR.readBytesUntil('\n', line, 200);
//		line[count] = NULL;

//		if (NULL != line[0])
//		{
//			Log::LogInfo("RX: ", line);
//			Globals::Interpreter->Execute(line);
//		}
//	}
//}


	bool SetupSerial(int speed)
	{
		SERIAL_PORT_MONITOR.begin(speed);

		Globals::SerialSpeed = speed;
		Utils::DelayMs(1000);

		SERIAL_PORT_MONITOR.println(PRM("# Serial OK"));

		return true;
	}


	bool StartupCommands()
	{
		Log::LogInfo(PRM("StartupCommands"));

		// TEMPORARY:
		//Globals::Interpreter->Execute(PRM("device ard configurepart multi0 ""items=led0 led1 led2 led3 ld4 led5"""));

#ifdef ARDJACK_INCLUDE_ARDUINO_DHT
		Globals::Interpreter->Execute(PRM("configure ard.dht0 model=dht22 variable=temperature"));
		Globals::Interpreter->Execute(PRM("ard.dht0"));
#endif

		//Globals::Interpreter->Execute(PRM("define thin configure ard shield=thinker"));
		//Globals::Interpreter->Execute(PRM("define notify device ard subscribe button0"));

#ifdef ARDJACK_ETHERNET_AVAILABLE
		Globals::Interpreter->Execute(PRM("add net eth0 ethernet"));
		//Globals::Interpreter->Execute(PRM("configure eth0 ??"));
		Globals::Interpreter->Execute(PRM("activate eth0"));
		Globals::Interpreter->Execute(PRM("eth0"));
#endif

#ifdef ARDJACK_WIFI_AVAILABLE
		Globals::Interpreter->Execute(PRM("add net wifi0 wifi"));
		// TEMPORARY:
		Globals::Interpreter->Execute(PRM("configure wifi0 ssid=JSL password=x73c66ocs"));
		Globals::Interpreter->Execute(PRM("activate wifi0"));
		Globals::Interpreter->Execute(PRM("wifi0"));
#endif

#ifdef ARDJACK_NETWORK_AVAILABLE
		//Globals::Interpreter->Execute(PRM("add connection http0 http"));
		//Globals::Interpreter->Execute(PRM("configure http0 ???"));
		//Globals::Interpreter->Execute(PRM("activate http0"));

		//Globals::Interpreter->Execute(PRM("add connection tcp0 tcp"));
		//Globals::Interpreter->Execute(PRM("configure tcp0 ???"));

		Globals::Interpreter->Execute(PRM("add connection udp0 udp"));
		//Globals::Interpreter->Execute(PRM("configure udp0 inport=? outport=? outip=?"));
		//Globals::Interpreter->Execute(PRM("activate udp0"));

		Globals::Interpreter->Execute(PRM("configure ard input=udp0 output=udp0"));
		Globals::Interpreter->Execute(PRM("configure ard messageformat=0"));
		Globals::Interpreter->Execute(PRM("configure ard messageprefix=$rem0:"));
		Globals::Interpreter->Execute(PRM("configure ard messageto=\\rem0"));
#else
		Globals::Interpreter->Execute(PRM("configure ard input=ser0 output=ser0"));
		Globals::Interpreter->Execute(PRM("configure ard messageformat=0"));
		Globals::Interpreter->Execute(PRM("configure ard messageprefix=$rem0:"));
		Globals::Interpreter->Execute(PRM("configure ard messageto=\\rem0"));
#endif

#ifdef ARDJACK_INCLUDE_BEACONS
		Globals::Interpreter->Execute(PRM("add beacon beacon0"));
		Globals::Interpreter->Execute(PRM("configure beacon0 target=udp0 'text=[ip] [timems] beacon0!'"));
		Globals::Interpreter->Execute(PRM("configure beacon0 'command=device ard write led0'"));
#endif

#ifdef ARDJACK_INCLUDE_DATALOGGERS
		Globals::Interpreter->Execute(PRM("add datalogger data0"));
		Globals::Interpreter->Execute(PRM("configure data0 input=ard output=udp0 'prefix=[ip] [timems]'"));
		Globals::Interpreter->Execute(PRM("configure data0 interval=1000 'inparts=ai0 ai1 dht0'"));
#endif

#ifdef ARDJACK_NETWORK_AVAILABLE
		//Globals::Interpreter->Execute(PRM("add connection udp1 udp"));
		//Globals::Interpreter->Execute(PRM("configure udp0 inip=192.168.1.66 inport=5000 inannounce=no outip=192.168.1.66 outport=5001 outannounce=no"));
		//Globals::Interpreter->Execute(PRM("configure udp1 inip=192.168.1.66 inport=6000 inannounce=no outip=192.168.1.66 outport=6001 outannounce=no"));

		//Globals::Interpreter->Execute(PRM("add bridge bridge0"));
		//Globals::Interpreter->Execute(PRM("configure bridge0 object1=udp0 object2=udp1 direction=bidirectional"));
#endif

		// Setup a signal filter.
		Globals::Interpreter->Execute(PRM("add filter filter0"));
		Globals::Interpreter->Execute(PRM("configure ard.ai filter=filter0"));
		Globals::Interpreter->Execute(PRM("configure filter0 mindiff=100"));
		//Globals::Interpreter->Execute(PRM("device ard subscribe ai"));

		// NOTES
		//
		// The default UDP Connection, 'udp0', is not activated above.
		// If you want it activated at startup, uncomment this line above (or here):
		//
		//		Globals::Interpreter->Execute(PRM("activate udp0"));
		//
		// Otherwise, it can be activated manually at runtime using the ArdJack command:
		//		activate udp0
		//
		// or, assuming defaults, send the following from anywhere over an active Serial Connection:
		//		$cc: activate udp0

		return true;
	}
