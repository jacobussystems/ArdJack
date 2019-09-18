/*
	SerialConnection.cpp

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

#include "Globals.h"
#include "IoTMessage.h"
#include "Log.h"
#include "SerialConnection.h"
#include "Utils.h"



SerialConnection::SerialConnection(const char* name)
	: Connection(name)
{
}


SerialConnection::~SerialConnection()
{
}


#ifdef ARDUINO

bool SerialConnection::Activate()
{
	if (!Connection::Activate()) return false;

	Config->GetAsInteger("Speed", &_Speed);

	if (Globals::Verbosity > 2)
	{
		char temp[120];
		sprintf(temp, PRM("Activating Serial Connection '%s' at %d baud"), Name, _Speed);
		Log::LogInfo(temp);
	}

	SERIAL_PORT_MONITOR.begin(_Speed);

	Globals::SerialSpeed = _Speed;

	Utils::DelayMs(100);

#ifdef ARDJACK_ARDUINO_MKR
	Utils::DelayMs(500);
#else
	//while (!Serial)
	//	;
#endif

	return true;
}


bool SerialConnection::AddConfig()
{
	if (!Connection::AddConfig())
		return false;

	Config->AddIntegerProp(PRM("Speed"), PRM("Speed (baud rate)."), 57600);

	//Config->AddIntegerProp(PRM("DataBits"), PRM("Number of data bits."), 8);
	//Config->AddIntegerProp(PRM("Parity"), PRM("Parity."), Parity.None);
	//Config->AddIntegerProp(PRM("Port"), PRM("Port."), ComPort.COM6);
	//Config->AddIntegerProp(PRM("StopBits"), PRM("Number of stop bits."), 1);

	return Config->SortItems();
}


bool SerialConnection::Deactivate()
{
	// We probably don't want to do this!!
	//SERIAL_PORT_MONITOR.end();

	return Connection::Deactivate();
}

#endif


bool SerialConnection::OutputMessage(IoTMessage* msg)
{
	return SendText(msg->WireText());
}


bool SerialConnection::PollInputs(int maxCount)
{
#ifdef ARDUINO
	char line[202];
	line[0] = NULL;

	if (SERIAL_PORT_MONITOR.available())
	{
		int count = SERIAL_PORT_MONITOR.readBytesUntil('\n', line, 200);
		line[count] = NULL;
	}

	if (strlen(line) > 0)
	{
		RxCount += strlen(line);
		RxEvents++;

		CheckAnnounce(false, line);

		if (Globals::Verbosity > 2)
			Log::LogInfo(Name, PRM(": RX '"), line, "'");

		ProcessInput(line);
	}
#endif

	return true;
}


bool SerialConnection::SendText(const char* text)
{
#ifdef ARDUINO
	SERIAL_PORT_MONITOR.println(text);

	//#if defined(ARDJACK_ARDUINO_DUE)
	//	SERIAL_PORT_MONITOR.println(text);
	//#endif
#else
	printf(text);
#endif

	TxCount += Utils::StringLen(text);
	TxEvents++;

	CheckAnnounce(true, text);

	if (Globals::Verbosity > 2)
		Log::LogInfo(Name, PRM(": TX '"), text, "'");

	return true;
}


bool SerialConnection::SendTextQuiet(const char* text)
{
	// Send 'text' quietly, i.e. with no logging.
#ifdef ARDUINO
	SERIAL_PORT_MONITOR.println(text);

	//#ifdef ARDJACK_ARDUINO_DUE
	//	SERIAL_PORT_MONITOR.println(text);
	//#endif
#else
	printf(text);
#endif

	TxCount += Utils::StringLen(text);
	TxEvents++;

	return true;
}

