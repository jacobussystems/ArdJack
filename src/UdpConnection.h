/*
	UdpConnection.h

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
		#include <EthernetUdp.h>
	#endif

	#ifdef ARDJACK_WIFI_AVAILABLE
		#include <WiFiUdp.h>
		//#include "WiFiLibrary.h"
	#endif
#else
	#include "stdafx.h"
	#include <winsock.h>
#endif

#include "Connection.h"
#include "DateTime.h"



#ifdef ARDJACK_NETWORK_AVAILABLE

class UdpConnection : public Connection
{
protected:

#ifdef ARDUINO
	#ifdef ARDJACK_NETWORK_AVAILABLE
		//char _PacketBuffer[255];								// buffer to hold incoming packet

		#ifdef ARDJACK_ETHERNET_AVAILABLE
			EthernetUDP* _Udp;
		#endif

		#ifdef ARDJACK_WIFI_AVAILABLE
			WiFiUDP* _Udp;
		#endif
	#endif

	virtual void Log_PollInputs(int packetSize);
	virtual bool SendUdpReply(const char* buffer);
	virtual bool WriteText(const char* text);
#else
	struct sockaddr_in _InputAddress;
	SOCKET _Listener;
	struct sockaddr_in _OutputAddress;
	SOCKET _Talker;
#endif

	char _InputIp[20];
	int _InputPort;
	char _MulticastIp[20];
	char _OutputIp[20];
	int _OutputPort;
	bool _UseMulticast;

	virtual bool Activate() override;
	virtual bool Deactivate() override;

#ifdef ARDUINO
#else
	virtual bool StartListening();
	virtual bool StartTalking();
	virtual bool StopListening();
	virtual bool StopTalking();
#endif

public:
	UdpConnection(const char* name);
	~UdpConnection();

	virtual bool AddConfig() override;

#ifdef ARDUINO
	virtual bool OutputMessage(IoTMessage* msg) override;
	virtual bool OutputText(const char* text) override;
#endif

	virtual bool PollInputs(int maxCount = 5) override;
	virtual bool SendText(const char* text) override;
	virtual bool SendTextQuiet(const char* text) override;
};

#endif
