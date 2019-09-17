/*
	HttpConnection.cpp

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
#include "HttpConnection.h"
#include "Log.h"
#include "Utils.h"



#ifdef ARDJACK_NETWORK_AVAILABLE

#ifdef ARDUINO

HttpConnection::HttpConnection(const char* name)
	: TcpConnection(name)
{
}


HttpConnection::~HttpConnection()
{

}


bool HttpConnection::PollRequests(DateTime* now)
{
#ifdef ARDJACK_WIFI_AVAILABLE
	// Any new request?
	WiFiClient client = _Server->available();
	if (NULL == client) return false;

	// Yes.
	if (Globals::Verbosity > 4)
		Log::LogInfo(Name, PRM(": New request"));

	char firstLine[102];
	firstLine[0] = NULL;

	char line[302];
	line[0] = NULL;

	bool curLineIsBlank = true;

	// Read as much as we can from the current request.
	// An HTTP request ends with a blank line.
	while (client.connected())
	{
		if (client.available())
		{
			char c = client.read();

			// If we've reached the end of a line (i.e. received a newline character) and the line is blank,
			// the HTTP request has ended, so we can send a reply.
			if (c == '\n' && curLineIsBlank)
			{
				// Send a standard HTTP response header.
				SendServerResponse(client, firstLine);
				break;
			}

			if (c == '\n')
			{
				// Start of a new line.
				if (NULL != line[0])
				{
					if (Globals::Verbosity > 4)
						Log::LogInfo(Name, PRM(" RX: line '"), line, "'");
				}

				if (NULL == firstLine[0])
				{
					// Keep the first line.
					strcpy(firstLine, line);

					if (Globals::Verbosity > 4)
						Log::LogInfo(Name, PRM(" RX: firstLine '"), firstLine, "'");
				}

				line[0] = NULL;
				curLineIsBlank = true;
			}
			else if (c != '\r')
			{
				// This is another character on the current line.
				curLineIsBlank = false;

				int count = strlen(line);
				line[count++] = c;
				line[count] = NULL;
			}
		}
	}

	// Give the browser time to receive the data.
	delay(1);

	// Close the connection.
	client.stop();

	return ProcessRequest(firstLine);
#else
	return false;
#endif
}


bool HttpConnection::PollResponses(DateTime* now)
{
#ifdef ARDJACK_NETWORK_AVAILABLE
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
				Log::LogInfo(Name, PRM(" HttpConnection::PollResponses: RX '"), line, "'");

			ProcessServerResponse(line);
		}
	}

	// If the server's disconnected, stop the client.
	if (!_Client->connected())
	{
		if (Globals::Verbosity > 4)
			Log::LogInfo(Name, PRM(" HttpConnection::PollResponses: Disconnecting from server"));

		_Client->stop();
		_ClientConnected = false;
	}

	return true;
#else
	return false;
#endif
}


bool HttpConnection::ProcessRequest(const char* line)
{
	// Parse the first line of the request.
	// A GET request's first line will be similar to:
	//		'GET /$cc:display%20memory HTTP/1.1'

	if (Globals::Verbosity > 4)
		Log::LogInfo(PRM("HttpConnection::ProcessRequest: '"), line, "'");

	char fields[2][ARDJACK_MAX_VALUE_LENGTH];
	int count = Utils::SplitText2Array(line, ' ', fields, 2, ARDJACK_MAX_VALUE_LENGTH);

	if (count != 3)
	{
		Log::LogWarning(Name, PRM(": Badly-formed HTTP request ignored: "), line);
		return false;
	}

	if (!Utils::StringEquals(fields[0], "GET"))
	{
		Log::LogWarning(Name, PRM(": Not a GET request - ignored: "), line);
		return false;
	}

	if (!Utils::StringStartsWith(fields[1], "/"))
	{
		Log::LogWarning(Name, PRM(": Invalid GET request ignored: "), line);
		return false;
	}

	if (Utils::StringEquals(fields[1], "/favicon.ico"))
	{
		// Ignore this.
		if (Globals::Verbosity > 4)
			Log::LogInfo(Name, PRM(": 'favicon' GET request ignored: "), line);
		return false;
	}

	char temp[102];
	strcpy(temp, fields[1] + 1);

	char command[102];
	Utils::StringReplace(command, "%20", " ", false, temp, 100);
	Utils::Trim(command);

	if (Globals::Verbosity > 2)
		Log::LogInfo(Name, PRM(" RX: '"), command, "'");

	ProcessInput(command);

	return true;
}


#ifdef ARDJACK_WIFI_AVAILABLE

bool HttpConnection::SendServerResponse(WiFiClient client, char* line)
{
	client.println(PRM("HTTP/1.1 200 OK"));
	client.println(PRM("Content-Type: text/html"));
	client.println(PRM("Connection: close"));				// the connection will be closed after completion of the response
	//client.println(PRM("Refresh: 5"));					// refresh the page automatically every 5 sec
	client.println();
	client.println(PRM("<!DOCTYPE HTML>"));
	client.println(PRM("<html>"));
	client.println(PRM("<h1>Thank you!</h1>"));
	client.print(PRM("<p>You sent: <b>"));
	client.print(line);
	client.println(PRM("</b></p>"));
	client.println(PRM("</html>"));

	return true;
}

#endif


bool HttpConnection::SendText(const char* text)
{
#ifdef ARDJACK_NETWORK_AVAILABLE
	char temp[202];

	if (Globals::Verbosity > 3)
	{
		sprintf(temp, PRM("%s SendText: Connecting to server at %s, port %d"), Name, _OutputIp, _OutputPort);
		Log::LogInfo(temp);
	}

	if (!_Client->connect(_OutputIp, _OutputPort))
	{
		sprintf(temp, PRM("%s SendText: Can't connect to server at %s, port %d"), Name, _OutputIp, _OutputPort);
		Log::LogError(temp);
		return false;
	}

	_ClientConnected = true;

	if (Globals::Verbosity > 3)
	{
		sprintf(temp, PRM("%s SendText: Connected to server at %s, port %d"), Name, _OutputIp, _OutputPort);
		Log::LogInfo(temp);
	}

	// Make an HTTP request.
	char useText[202];
	Utils::StringReplace(text, " ", "%20", false, useText, 200);

	sprintf(temp, PRM("GET /%s HTTP/1.1"), useText);
	_Client->println(temp);

	// 'Host: 192.168.1.69:8000'
	sprintf(temp, PRM("Host: %s:%d"), _OutputIp, _OutputPort);
	_Client->println(temp);

	_Client->println("Connection: close");
	_Client->println();

	if (Globals::Verbosity > 3)
	{
		sprintf(temp, PRM("%s SendText: Sent to server at %s, port %d: '"), Name, _OutputIp, _OutputPort);
		Log::LogInfo(temp, text, "'");
	}

	return true;
#else
	return false;
#endif
}


#else


HttpConnection::HttpConnection(const char* name)
	: TcpConnection(name)
{
}


HttpConnection::~HttpConnection()
{

}

#endif

#endif
