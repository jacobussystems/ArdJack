/*
	TcpConnection.h

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

	#ifdef ARDJACK_ETHERNET_AVAILABLE
		#include <Ethernet.h>
	#endif

	#ifdef ARDJACK_WIFI_AVAILABLE
		#include <WiFiClient.h>
		#include <WiFiServer.h>
	#endif
#else
#endif

#include "Connection.h"
#include "DateTime.h"



#ifdef ARDJACK_NETWORK_AVAILABLE

class TcpConnection : public Connection
{
protected:
	bool _ClientConnected;
	int _InputPort;
	char _OutputIp[20];
	int _OutputPort;

#ifdef ARDUINO
	#ifdef ARDJACK_ETHERNET_AVAILABLE
		EthernetClient* _Client;
		EthernetServer* _Server;
	#endif

	#ifdef ARDJACK_WIFI_AVAILABLE
		WiFiClient* _Client;											// local TCP client for 'talking' (sending output)
		WiFiServer* _Server;											// local TCP server for 'listening' (receiving input)
	#endif

	virtual bool Activate() override;
	virtual bool Deactivate() override;
	virtual bool PollInputs(int maxCount) override;
	virtual bool PollRequests();
	virtual bool PollResponses();
	virtual bool ProcessRequest(const char* line);
	virtual bool ProcessServerResponse(const char* line);
	virtual bool StartListening();
	virtual bool StartTalking();
	virtual bool StopListening();
	virtual bool StopTalking();
#endif

public:
	TcpConnection(const char* name);
	~TcpConnection();

	virtual bool AddConfig() override;

#ifdef ARDUINO
	virtual bool SendText(const char* text) override;
#endif

};

#endif
