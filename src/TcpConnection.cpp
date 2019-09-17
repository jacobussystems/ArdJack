/*
	TcpConnection.cpp

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

#include "Connection.h"
#include "Globals.h"
#include "Log.h"
#include "TcpConnection.h"
#include "Utils.h"



#ifdef ARDJACK_NETWORK_AVAILABLE

// Arduino + Windows.

bool TcpConnection::AddConfig()
{
	if (!Connection::AddConfig())
		return false;

	Config->SetFromString("CanOutput", "true");

	if (Config->AddIntegerProp(PRM("InPort"), PRM("Input port number."), _InputPort) == NULL) return false;
	if (Config->AddStringProp(PRM("OutIP"), PRM("Output IP address."), _OutputIp) == NULL) return false;
	if (Config->AddIntegerProp(PRM("OutPort"), PRM("Output port number."), _OutputPort) == NULL) return false;

	return Config->SortItems();
}


#ifdef ARDUINO

TcpConnection::TcpConnection(const char* name)
	: Connection(name)
{
	_CanInput = true;
	_CanOutput = true;
	_Client = NULL;
	_ClientConnected = false;
	_InputPort = 8000;
	strcpy(_OutputIp, "192.168.1.66");
	_OutputPort = 8001;
	_Server = NULL;
}


TcpConnection::~TcpConnection()
{
	if (NULL != _Client)
	{
		delete _Client;
		_Client = NULL;
	}

	if (NULL != _Server)
	{
		delete _Server;
		_Server = NULL;
	}
}


bool TcpConnection::Activate()
{
	if (!Connection::Activate()) return false;

	Config->GetAsInteger("InPort", &_InputPort);
	Config->GetAsString("OutIp", _OutputIp);
	Config->GetAsInteger("OutPort", &_OutputPort);

	if (_CanInput)
	{
		if (!StartListening())
			return false;
	}

	if (_CanOutput)
	{
		if (!StartTalking())
			return false;
	}

	if (Globals::Verbosity > 3)
		Log::LogInfo(Name, PRM(" activated"));

	return true;
}


bool TcpConnection::Deactivate()
{
	if (_CanInput)
		StopListening();

	if (_CanOutput)
		StopTalking();

	return Connection::Deactivate();
}


bool TcpConnection::PollInputs(int maxCount)
{
	if (!Active())
		return false;

	// Any requests for the server (listener)?
	if (_CanInput)
		PollRequests();

	// Any responses for the client (talker)?
	if (_CanOutput)
		PollResponses();

	return true;
}


bool TcpConnection::PollRequests()
{
#ifdef ARDJACK_WIFI_AVAILABLE
	// Any new request?
	WiFiClient client = _Server->available();
	if (NULL == client) return false;

	// Yes.
	if (Globals::Verbosity > 4)
		Log::LogInfo(Name, PRM(": New request"));

	char line[302];
	line[0] = NULL;

	// Read the current request (one line).
	while (client.connected())
	{
		if (!client.available())
		{
			// Treat this as 'end of line' for now.
			if (Globals::Verbosity > 6)
				Log::LogInfo(Name, PRM(" RX (no more available): line '"), line, "'");
			break;
		}

		char c = client.read();

		// If we've reached the end of a line (i.e. received a newline character) and the line is blank,
		// the TCP request has ended, so we can send a reply.
		if (c == '\n')
		{
			// End of line.
			if (NULL != line[0])
			{
				if (Globals::Verbosity > 6)
					Log::LogInfo(Name, PRM(" RX (end of line): line '"), line, "'");
			}

			break;
		}

		if (c != '\r')
		{
			// This is another character on the current line.
			int count = strlen(line);
			line[count++] = c;
			line[count] = NULL;
		}
	}

	// Close the connection.
	//client.stop();

	RxCount += strlen(line);
	RxEvents++;

	return ProcessRequest(line);
#else
	return false;
#endif
}


bool TcpConnection::PollResponses()
{
	if (!_ClientConnected)
		return false;

	// Any new request?
	char line[202];

	while (_Client->available())
	{
		int count = _Client->readBytesUntil('\n', line, 200);

		if (count > 0)
		{
			line[count] = NULL;

			if (Globals::Verbosity > 3)
				Log::LogInfo(Name, PRM(" TcpConnection::PollResponses: RX '"), line, "'");

			ProcessServerResponse(line);
		}
	}

	if (!_Client->connected())
	{
		// Our client is no longer connected.
		// Presumably the remote server has disconnected, so stop the client.
		if (Globals::Verbosity > 4)
			Log::LogInfo(Name, PRM(" TcpConnection::PollResponses: Disconnecting from server"));

		_Client->stop();
		_ClientConnected = false;
	}

	return true;
}


bool TcpConnection::ProcessRequest(const char* line)
{
	// Process the request.
	if (Globals::Verbosity > 2)
		Log::LogInfo(Name, PRM(" RX: '"), line, "'");

	ProcessInput(line);

	return true;
}


bool TcpConnection::ProcessServerResponse(const char* line)
{
	if (Globals::Verbosity > 4)
		Log::LogInfo(Name, PRM(" Server response: "), line);

	return true;
}


bool TcpConnection::SendText(const char* text)
{
	char temp[202];

	if (Globals::Verbosity > 3)
	{
		sprintf(temp, PRM("%s SendText: Connecting to remote server at %s, port %d"), Name, _OutputIp, _OutputPort);
		Log::LogInfo(temp);
	}

	if (!_Client->connect(_OutputIp, _OutputPort))
	{
		sprintf(temp, PRM("%s SendText: Can't connect to remote server at %s, port %d"), Name, _OutputIp, _OutputPort);
		Log::LogError(temp);
		return false;
	}

	_ClientConnected = true;

	if (Globals::Verbosity > 3)
	{
		sprintf(temp, PRM("%s SendText: Connected to remote server at %s, port %d"), Name, _OutputIp, _OutputPort);
		Log::LogInfo(temp);
	}

	// Make the TCP request.
	_Client->println(text);

	TxCount += strlen(text);
	TxEvents++;

	CheckAnnounce(true, text);

	if (Globals::Verbosity > 3)
	{
		sprintf(temp, PRM("%s SendText: Sent to remote server at %s, port %d: '"), Name, _OutputIp, _OutputPort);
		Log::LogInfo(temp, text, "'");
	}

	return true;
}


bool TcpConnection::StartListening()
{
	if (_CanInput)
	{
		if (Globals::Verbosity > 2)
		{
			char temp[120];
			sprintf(temp, PRM("Starting listener (local TCP server) on port %d"), _InputPort);
			Log::LogInfo(temp);
		}

		StopListening();

#ifdef ARDJACK_ETHERNET_AVAILABLE
		_Server = new EthernetServer(_InputPort);
		_Server->begin();
#endif

#ifdef ARDJACK_WIFI_AVAILABLE
		_Server = new WiFiServer(_InputPort);
		_Server->begin();
#endif
	}

	return true;
}


bool TcpConnection::StartTalking()
{
	_ClientConnected = false;

	if (_CanInput)
	{
		if (Globals::Verbosity > 2)
		{
			char temp[120];
			sprintf(temp, PRM("Starting talker (local TCP client) on port %d"), _OutputPort);
			Log::LogInfo(temp);
		}

		StopTalking();

#ifdef ARDJACK_ETHERNET_AVAILABLE
		_Client = new EthernetClient(_OutputPort);
		//_Client->begin();
#endif

#ifdef ARDJACK_WIFI_AVAILABLE
		_Client = new WiFiClient(_OutputPort);
		//_Client->begin();
#endif
	}

	return true;
}


bool TcpConnection::StopListening()
{
	if (_CanOutput)
	{
		if (NULL != _Server)
		{
			if (Globals::Verbosity > 2)
			{
				char temp[120];
				sprintf(temp, PRM("Stopping listener (local TCP server) on port %d"), _InputPort);
				Log::LogInfo(temp);
			}

#ifdef ARDJACK_ESP32
			_Server->stop();
#else
#endif

			delete _Server;
			_Server = NULL;
		}
	}

	return true;
}


bool TcpConnection::StopTalking()
{
	if (_CanInput)
	{
		if (NULL != _Client)
		{
			if (Globals::Verbosity > 2)
			{
				char temp[120];
				sprintf(temp, PRM("Stopping talker (local TCP client) on port %d"), _OutputPort);
				Log::LogInfo(temp);
			}

			_Client->stop();
			delete _Client;
			_Client = NULL;
		}
	}

	_ClientConnected = false;

	return true;
}


#else

TcpConnection::TcpConnection(const char* name)
	: Connection(name)
{
	_CanInput = true;
	_CanOutput = true;
	_InputPort = 8000;
	strcpy(_OutputIp, "192.168.1.66");
	_OutputPort = 8001;
}


TcpConnection::~TcpConnection()
{

}

#endif

#endif
