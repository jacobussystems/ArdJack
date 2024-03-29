/*
	ArdJackW.cpp

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

// ArdJackW.cpp

#include "pch.h"
#include "stdafx.h"

#include <stdio.h>
//#include <Windows.h>
//#include <ConsoleApi.h>

//#include "npipe.h"

#include "Beacon.h"
#include "BeaconManager.h"
#include "Bridge.h"
#include "BridgeManager.h"
#include "ClipboardConnection.h"
#include "CmdInterpreter.h"
#include "CommandSet.h"
#include "ConnectionManager.h"
#include "DataLogger.h"
#include "DataLoggerManager.h"
#include "DateTime.h"
#include "DeviceManager.h"
#include "Displayer.h"
#include "Route.h"
#include "Log.h"
#include "PersistentFileManager.h"
#include "UdpConnection.h"
#include "Utils.h"
#include "WinDevice.h"

#ifdef ARDJACK_INCLUDE_TESTS
	//#include "Tests.h"
#endif


// Forward declarations.
//void CommandCallback(void *caller, Route* route, IoTMessage* msg);

VOID CALLBACK Timer1Callback(PVOID lpParameter, BOOLEAN TimerOrWaitFired);
VOID CALLBACK Timer2Callback(PVOID lpParameter, BOOLEAN TimerOrWaitFired);

HANDLE _timer1 = NULL;
bool _timer1_busy = true;
HANDLE _timer2 = NULL;
bool _timer2_busy = true;

//extern BOOL WINAPI WriteConsole(
//	_In_             HANDLE  hConsoleOutput,
//	_In_       const VOID* lpBuffer,
//	_In_             DWORD   nNumberOfCharsToWrite,
//	_Out_            LPDWORD lpNumberOfCharsWritten,
//	_Reserved_       LPVOID  lpReserved
//);


int main()
{
	Globals::InitialisedStatic = true;
	Globals::Init();

	// Setup Windows queue timers.
	DWORD period = 20;				// ms
	DWORD dueTime = period;			// ms

	CreateTimerQueueTimer(&_timer1, NULL, Timer1Callback, NULL, dueTime, period, 0);
	CreateTimerQueueTimer(&_timer2, NULL, Timer2Callback, NULL, dueTime, period, 0);

	Log::LogInfo("ArdJackW (ArdJack, Windows)");

#ifdef ARDJACK_NETWORK_AVAILABLE
	Utils::InitializeWinsock();
#endif

	Utils::GetComputerInfo(Globals::HostName, Globals::IpAddress);

	if (Globals::ComputerName[0] == NULL)
		strcpy(Globals::ComputerName, Globals::HostName);

	Displayer::DisplayMemory();

	// Create the predefined objects.

	// Create the Clipboard Connection 'clip'.
	Connection* clip = (Connection*)Globals::AddObject(ARDJACK_OBJECT_TYPE_CONNECTION, ARDJACK_CONNECTION_SUBTYPE_CLIPBOARD, "clip");

	// Create the Windows Device 'win'.
	Device* win = (Device*)Globals::AddObject(ARDJACK_OBJECT_TYPE_DEVICE, ARDJACK_DEVICE_SUBTYPE_WINDOWS, "win");

#ifdef ARDJACK_NETWORK_AVAILABLE
	// Create the UDP Connection 'udp0'.
	Connection* udp0 = (Connection*)Globals::AddObject(ARDJACK_OBJECT_TYPE_CONNECTION, ARDJACK_CONNECTION_SUBTYPE_UDP, "udp0");

	if (NULL != udp0)
		udp0->Config->SetFromString("InIP", Globals::IpAddress);
#endif

#ifdef ARDJACK_INCLUDE_PERSISTENCE
	// Load the configuration file (if any).
	Globals::PersistentFileMgr->Scan(Globals::AppDocsFolder);
	Globals::LoadIniFile("configuration");
#endif

	Displayer::DisplayMemory();

#ifdef ARDJACK_INCLUDE_PERSISTENCE
	// Read the startup file (if any).
	Globals::ReadExecuteStartupFile();
#endif

	Displayer::DisplayMemory();

	printf("\nREADY\n\n");

	_timer1_busy = false;
	_timer2_busy = false;

	// Loop waiting for User commands from the keyboard.
	char commandLine[122];
	commandLine[0] = NULL;

	char temp[20];

	HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);

	while (!Globals::UserExit)
	{
		// Display a command prompt.
		Utils::GetTimeString(temp);
		strcat(temp, " > ");
		printf(temp);

		//if (!Globals::AutoClearCommandLine)
		//{
		//	//DWORD bytesWritten;
		//	//WriteConsole(hStdIn, commandLine, strlen(commandLine), &bytesWritten, NULL);
		//}

		// Wait for User input.
		fgets(commandLine, 120, stdin);
		Utils::Trim(commandLine);

		if (strlen(commandLine) > 0)
		{
			// Execute this command.
			Globals::QueueCommand(commandLine);
			Utils::DelayMs(100);
		}
	}

	system("pause");

	return 0;
}


//void CommandCallback(void* caller, Route* route, IoTMessage* msg)
//{
//	if (Globals::Verbosity > 2)
//		Log::LogInfo(PRM("CommandCallback: '"), msg->Text, "'");
//
//	Globals::Interpreter->Execute(msg->Text);
//}


VOID CALLBACK Timer1Callback(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
	if (_timer1_busy)
		return;

	_timer1_busy = true;

	try
	{
		//Log::Output("Timer1Callback!");

		if (NULL != Globals::ConnectionMgr)
			Globals::ConnectionMgr->Poll();
	}
	catch (...)
	{
		Log::LogException(PRM("Timer1Callback"));
	}

	_timer1_busy = false;
}


VOID CALLBACK Timer2Callback(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
	if (_timer2_busy)
		return;

	_timer2_busy = true;

	try
	{
		//Log::Output("Timer2Callback!");

		if (NULL != Globals::DeviceMgr)
			Globals::DeviceMgr->Poll();

#ifdef ARDJACK_INCLUDE_BEACONS
		if (NULL != Globals::BeaconMgr)
			Globals::BeaconMgr->Poll();
#endif

#ifdef ARDJACK_INCLUDE_DATALOGGERS
		if (NULL != Globals::DataLoggerMgr)
			Globals::DataLoggerMgr->Poll();
#endif

		Globals::CheckCommandBuffer();
		Globals::CheckDeviceBuffer();

		Log::Poll();
	}
	catch (...)
	{
		Log::LogException(PRM("Timer2Callback"));
	}

	_timer2_busy = false;
}

