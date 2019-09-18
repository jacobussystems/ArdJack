/*
	UdpConnection.cpp

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
	#ifdef ARDJACK_ETHERNET_AVAILABLE
		#include <Ethernet.h>
		#include <EthernetUdp.h>
	#endif

	#ifdef ARDJACK_WIFI_AVAILABLE
		#include "WiFiLibrary.h"
		#include <WiFiUdp.h>
	#endif
#else
	#include "stdafx.h"

	// need link with Ws2_32.lib
	#pragma comment(lib, "Ws2_32.lib")
#endif

#include "Globals.h"
#include "IoTMessage.h"
#include "Log.h"
#include "UdpConnection.h"
#include "Utils.h"



#ifdef ARDJACK_NETWORK_AVAILABLE

// BOTH ARDUINO AND WINDOWS code.

bool UdpConnection::AddConfig()
{
	if (!Connection::AddConfig())
		return false;

	Config->SetFromString("CanOutput", "true");

	if (Config->AddStringProp(PRM("InIP"), PRM("Input IP address."), _InputIp) == NULL) return false;
	if (Config->AddIntegerProp(PRM("InPort"), PRM("Input port number."), _InputPort) == NULL) return false;
	if (Config->AddStringProp(PRM("MulticastIP"), PRM("Multicast IP address."), _MulticastIp) == NULL) return false;
	if (Config->AddStringProp(PRM("OutIP"), PRM("Output IP address."), _OutputIp) == NULL) return false;
	if (Config->AddIntegerProp(PRM("OutPort"), PRM("Output port number."), _OutputPort) == NULL) return false;
	if (Config->AddBooleanProp(PRM("UseMulticast"), PRM("Use multicast?"), _UseMulticast) == NULL) return false;

	return Config->SortItems();
}



#ifdef ARDUINO

UdpConnection::UdpConnection(const char* name)
	: Connection(name)
{
	_CanInput = true;
	_CanOutput = true;

	strcpy(_InputIp, "192.168.1.66");											// usually, the IP of 'this computer'
	_InputPort = 5000;

	strcpy(_MulticastIp, "");
	strcpy(_OutputIp, "192.168.1.66");
	_OutputPort = 5001;
	_Udp = NULL;
	_UseMulticast = false;
}


UdpConnection::~UdpConnection()
{
	if (NULL != _Udp)
	{
		_Udp->stop();
		delete _Udp;
		_Udp = NULL;
	}
}


bool UdpConnection::Activate()
{
	if (!Connection::Activate()) return false;

	Config->GetAsString("InIp", _InputIp);
	Config->GetAsInteger("InPort", &_InputPort);

	Config->GetAsString("OutIp", _OutputIp);
	Config->GetAsInteger("OutPort", &_OutputPort);
	Config->GetAsBoolean("UseMulticast", &_UseMulticast);

	if (NULL == _Udp)
	{
#ifdef ARDJACK_ETHERNET_AVAILABLE
		_Udp = new EthernetUDP();
#endif

#ifdef ARDJACK_WIFI_AVAILABLE
		_Udp = new WiFiUDP();
#endif
	}

	if (_CanInput)
	{
		if (Globals::Verbosity > 2)
			Log::LogInfoF(PRM("Activating UDP listener on port %d"), _InputPort);

		_Udp->begin(_InputPort);
	}

	return true;
}


bool UdpConnection::Deactivate()
{
	if (Globals::Verbosity > 2)
		Log::LogInfo(PRM("Deactivating UDP listener"));

	if (NULL != _Udp)
		_Udp->stop();

	return true;
}


void UdpConnection::Log_PollInputs(int packetSize)
{
	IPAddress remoteIp = _Udp->remoteIP();
	int remotePort = _Udp->remotePort();

	char temp[20];

	Log::LogInfoF(PRM("%s: RX %d bytes from '%s', port %d"),
		Name, packetSize, Utils::IpAddressToString(remoteIp, temp), remotePort);
}


bool UdpConnection::OutputMessage(IoTMessage* msg)
{
	// Output 'msg'.
	return OutputText(msg->WireText());
}


bool UdpConnection::OutputText(const char* text)
{
	// Not buffering currently.
	return SendText(text);
}


bool UdpConnection::PollInputs(int maxCount)
{
	int packetSize = _Udp->parsePacket();
	if (packetSize < 1)
		return false;

	if (Globals::Verbosity > 4)
		Log_PollInputs(packetSize);

	// Read the packet.
	char packetBuffer[255];

	//int len = 0;

	//for (int i = 0; i < packetSize; i++)
	//{
	//	packetBuffer[len++] = _Udp->read();
	//	break;
	//}

	int len = _Udp->read(packetBuffer, 255);
	if (len == 0) return true;

	packetBuffer[len] = 0;
	Utils::Trim(packetBuffer);

	RxCount += len;
	RxEvents++;

	CheckAnnounce(false, packetBuffer);

	if (Globals::Verbosity > 2)
		Log::LogInfoF(PRM("%s: RX '%s' (%d chars)"), Name, packetBuffer, len);

	bool result = ProcessInput(packetBuffer);

	return result;
}


bool UdpConnection::SendText(const char* text)
{
	// Send 'text' to the output IP address/port number.
	if (!_Active)
	{
		Log::LogErrorF(PRM("%s: UDP SendText: Not active"), Name);
		return false;
	}

	if (Globals::Verbosity > 4)
	{
		Log::LogInfoF(PRM("%s: UDP SendText: Sending '%s' (%d chars) -> ip %s, port %d"),
			Name, text, strlen(text), _OutputIp, _OutputPort);
	}

	if (_Udp->beginPacket(_OutputIp, _OutputPort) != 1)
	{
		Log::LogErrorF(PRM("%s: UDP SendText: Sending '%s' (%d chars) -> ip %s, port %d - beginPacket FAILED"),
			Name, text, strlen(text), _OutputIp, _OutputPort);
		return false;
	}

	if (!WriteText(text))
		return false;

	if (_Udp->endPacket() != 1)
	{
		Log::LogErrorF(PRM("%s: UDP SendText: Sending '%s' (%d chars) -> ip %s, port %d - endPacket FAILED"),
			Name, text, strlen(text), _OutputIp, _OutputPort);
		return false;
	}

	TxCount += strlen(text);
	TxEvents++;

	CheckAnnounce(true, text);

	if (Globals::Verbosity > 2)
		Log::LogInfo(Name, PRM(": TX '"), text, "'");

	return true;
}


bool UdpConnection::SendTextQuiet(const char* text)
{
	// Send 'text' quietly, i.e. with no logging.
	if (_Udp->beginPacket(_OutputIp, _OutputPort) != 1)
	{
		Log::LogError(PRM("UDP SendTextQuiet: beginPacket FAILED"));
		return false;
	}

	if (!WriteText(text))
		return false;

	if (_Udp->endPacket() != 1)
	{
		Log::LogError(PRM("UDP SendTextQuiet: endPacket FAILED"));
		return false;
	}

	return true;
}


bool UdpConnection::SendUdpReply(const char* buffer)
{
	// Send a UDP reply to the source IP address and port.
	if (_Udp->beginPacket(_Udp->remoteIP(), _Udp->remotePort()) != 1)
	{
		Log::LogError(PRM("UDP SendUdpReply: beginPacket FAILED"));
		return false;
	}

	if (!WriteText(buffer))
		return false;

	if (_Udp->endPacket() != 1)
	{
		Log::LogError(PRM("UDP SendUdpReply: endPacket FAILED"));
		return false;
	}

	return true;
}


bool UdpConnection::WriteText(const char* text)
{
#ifdef ARDJACK_ESP32
	int count = _Udp->println(text);
#else
	int count = _Udp->write(text);
#endif

	if (count < strlen(text))
	{
		Log::LogErrorF(PRM("UDP WriteText: Only %d of %d bytes written"), count, strlen(text));
		return false;
	}

	return true;
}


#else


// From:  https://stackoverflow.com/a/32471539/7013913
//  and:  https://stackoverflow.com/questions/15941005/making-recv-from-function-non-blocking
//  and:  https://msdn.microsoft.com/en-us/library/windows/desktop/ms740476%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396


UdpConnection::UdpConnection(const char* name)
	: Connection(name)
{
	_InputIp[0] = NULL;
	_Listener = NULL;
	_OutputIp[0] = NULL;
	_Talker = NULL;
}


UdpConnection::~UdpConnection()
{
	StopListening();
	StopTalking();
	Utils::TerminateWinsock();
}


bool UdpConnection::Activate()
{
	if (!Connection::Activate()) return false;

	Config->GetAsString("InIP", _InputIp);
	Config->GetAsInteger("InPort", &_InputPort);

	Config->GetAsString("MulticastIP", _MulticastIp);
	Config->GetAsString("OutIP", _OutputIp);
	Config->GetAsInteger("OutPort", &_OutputPort);
	Config->GetAsBoolean("UseMulticast", &_UseMulticast);

	char temp[120];

	if (_CanInput)
	{
		if (Globals::Verbosity > 2)
		{
			sprintf(temp, PRM("Activating UDP listener on port %d"), _InputPort);
			Log::LogInfo(temp);
		}

		if (!StartListening())
			return false;
	}

	if (_CanOutput)
	{
		if (Globals::Verbosity > 2)
		{
			sprintf(temp, PRM("Activating UDP talker on port %d"), _OutputPort);
			Log::LogInfo(temp);
		}

		if (!StartTalking())
			return false;
	}

	return true;
}


bool UdpConnection::Deactivate()
{
	if (_CanInput)
	{
		if (Globals::Verbosity > 2)
			Log::LogInfo("Deactivating UDP listener");

		StopListening();
	}

	if (_CanOutput)
	{
		if (Globals::Verbosity > 2)
			Log::LogInfo("Deactivating UDP talker");

		StopTalking();
	}

	Utils::TerminateWinsock();

	return true;
}


bool UdpConnection::PollInputs(int maxCount)
{
	if (!_Active || !_CanInput)
		return false;

	char line[200];
	int addrlen = sizeof(_InputAddress);

	int msglen = recvfrom(_Listener, line, sizeof(line), 0, (struct sockaddr *)&_InputAddress, &addrlen);
	if (msglen == SOCKET_ERROR)
	{
		// No UDP message has been received (or some other error...).
		int err = WSAGetLastError();

		if (err != 10060)
			Log::LogInfoF(PRM("UDP message not received, ERROR CODE: %d"), err);

		return false;
	}

	// A UDP message has been received.
	line[msglen] = NULL;

	RxCount += Utils::StringLen(line);
	RxEvents++;

	CheckAnnounce(false, line);

	if (Globals::Verbosity > 2)
	{
		char ip[32];
		strcpy(ip, inet_ntoa(_InputAddress.sin_addr));

		Log::LogInfoF(PRM("%s: RX from %s port %d: '%s'"), Name, ip, _InputAddress.sin_port, line);
	}

	ProcessInput(line);

	return true;
}


bool UdpConnection::SendText(const char* text)
{
	if (!_Active)
	{
		Log::LogErrorF(PRM("%s: UDP SendText: Not active"), Name);
		return false;
	}

	if (!_CanOutput)
		return false;

	if (sendto(_Talker, text, Utils::StringLen(text), 0, (struct sockaddr *)&_OutputAddress, sizeof(_OutputAddress))
		== SOCKET_ERROR)
	{
		Log::LogErrorF(PRM("%s: UDP SendText failed, ERROR CODE: %d"), Name, WSAGetLastError());
		return false;
	}

	TxCount += Utils::StringLen(text);
	TxEvents++;

	CheckAnnounce(true, text);

	if (Globals::Verbosity > 2)
		Log::LogInfo(Name, PRM(": TX '"), text, "'");

	return true;
}


bool UdpConnection::SendTextQuiet(const char* text)
{
	if (!_Active || !_CanOutput)
		return false;

	if (sendto(_Talker, text, Utils::StringLen(text), 0, (struct sockaddr *)&_OutputAddress, sizeof(_OutputAddress))
		== SOCKET_ERROR)
		return false;

	return true;
}


bool UdpConnection::StartListening()
{
	// Setup a UDP listener.
	char temp[120];

	memset(&_InputAddress, 0, sizeof(_InputAddress));
	_InputAddress.sin_family = AF_INET;
	_InputAddress.sin_addr.s_addr = Utils::HostID_To_Address(_InputIp);
	_InputAddress.sin_port = htons(_InputPort);

	// Initialize winsock.
	if (!Utils::InitializeWinsock())
		return false;

	//UDP Socket creation
	_Listener = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (_Listener == INVALID_SOCKET)
	{
		sprintf(temp, PRM("UDP socket creation failed ERROR CODE: %d"), WSAGetLastError());
		Log::LogError(temp);
		closesocket(_Listener);
		Utils::TerminateWinsock();
		return false;
	}

	// Reuse the address.
	int enable = 1;
	if (setsockopt(_Listener, SOL_SOCKET, SO_REUSEADDR, (char *)&enable, sizeof(int)) == SOCKET_ERROR)
	{
		sprintf(temp, PRM("setsockopt(SO_REUSEADDR) - ERROR CODE: %d"), WSAGetLastError());
		Log::LogError(temp);
		closesocket(_Listener);
		Utils::TerminateWinsock();
		return false;
	}

	//struct timeval read_timeout;
	//read_timeout.tv_sec = 0;
	//read_timeout.tv_usec = 10;
	//int status = setsockopt(_Listener, SOL_SOCKET, SO_RCVTIMEO, (char *)&read_timeout, sizeof read_timeout);

	DWORD recv_timeout = 10;		// 10 ms
	int status = setsockopt(_Listener, SOL_SOCKET, SO_RCVTIMEO, (char *)&recv_timeout, sizeof recv_timeout);

	if (status != 0)
	{
		sprintf(temp, PRM("setsockopt(SO_RCVTIMEO) - ERROR CODE: %d"), WSAGetLastError());
		Log::LogError(temp);
		closesocket(_Listener);
		Utils::TerminateWinsock();
		return false;
	}

	//UDP SOCKET Binding 
	if (bind(_Listener, (sockaddr *)&_InputAddress, sizeof(_InputAddress)) == SOCKET_ERROR)
	{
		sprintf(temp, PRM("UDP socket binding - ERROR CODE: %d"), WSAGetLastError());
		Log::LogError(temp);
		closesocket(_Listener);
		Utils::TerminateWinsock();
		return false;
	}

	//if (_UseMulticast)
	//{
	//	// use setsockopt() to request joining a multicast group
	//	struct ip_mreq mreq;
	//	mreq.imr_multiaddr.s_addr = Utils::HostID_To_Address(_MulticastIp);
	//	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	//	if (setsockopt(_Listener, SOL_SOCKET, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) < 0)
	//	{
	//		sprintf(temp, PRM("setsockopt(IP_ADD_MEMBERSHIP) - ERROR CODE: %d"), WSAGetLastError());
	//		Log::LogError(temp);
	//		closesocket(_Listener);
	//		Utils::TerminateWinsock();
	//		return false;
	//	}
	//}

	//if (_UseMulticast)
	sprintf(temp, PRM("Started UDP listener on %s, port %d"), _InputIp, _InputPort);
	Log::LogInfo(temp);

	return true;
}


bool UdpConnection::StartTalking()
{
	// Setup a UDP talker.
	char temp[120];

	memset(&_OutputAddress, 0, sizeof(_OutputAddress));
	_OutputAddress.sin_family = AF_INET;
	_OutputAddress.sin_port = htons(_OutputPort);

	if (_UseMulticast)
		_OutputAddress.sin_addr.s_addr = Utils::HostID_To_Address(_MulticastIp);
	else
		_OutputAddress.sin_addr.s_addr = Utils::HostID_To_Address(_OutputIp);

	// Initialize winsock.
	if (!Utils::InitializeWinsock())
		return false;

	//UDP Socket Creation
	_Talker = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (_Talker == INVALID_SOCKET)
	{
		sprintf(temp, PRM("UDP Socket not created - ERROR CODE: %d"), WSAGetLastError());
		Log::LogError(temp);
		Utils::TerminateWinsock();
		return false;
	}

	// Reuse the address.
	//int enable = 1;
	//if (setsockopt(_Talker, SOL_SOCKET, SO_REUSEADDR, (char *)&enable, sizeof(int)) == SOCKET_ERROR)
	//{
	//	sprintf(temp, PRM("setsockopt(SO_REUSEADDR) - ERROR CODE: %d"), WSAGetLastError());
	//	Log::LogError(temp);
	//	closesocket(_Talker);
	//	Utils::TerminateWinsock();
	//	return false;
	//}

	if (_UseMulticast)
		sprintf(temp, PRM("Started UDP talker on %s (multicast), port %d"), _MulticastIp, _OutputPort);
	else
		sprintf(temp, PRM("Started UDP talker on %s, port %d"), _OutputIp, _OutputPort);

	Log::LogInfo(temp);

	return true;
}


bool UdpConnection::StopListening()
{
	if (NULL != _Listener)
	{
		closesocket(_Listener);
		_Listener = NULL;
	}

	return true;
}


bool UdpConnection::StopTalking()
{
	if (NULL != _Talker)
	{
		closesocket(_Talker);
		_Talker = NULL;
	}

	return true;
}

#endif

#endif
