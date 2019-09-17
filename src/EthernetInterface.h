/*
	EthernetInterface.h

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

#ifdef ARDJACK_ETHERNET_AVAILABLE

#include <arduino.h>

#include <Ethernet.h>

#include "DetectBoard.h"
#include "IoTObject.h"
#include "NetworkInterface.h"



class EthernetInterface : public NetworkInterface
{
protected:
	EthernetClient _Client;
	int _ConnectMode;													// connect mode enumeration: ARDJACK_WIFI_CONNECT_OPEN, etc.
	char _ConnectModeStr[20];
	int _CSPin;
	char _DHCPHostname[30];
	int _Status;

	virtual bool Connect();
	virtual bool Disconnect();
	virtual bool GetConnectionInfo();
	virtual char* GetStatusAsString(int status, char *out);

public:
	IPAddress DnsIp;
	IPAddress GatewayIp;
	IPAddress LocalIp;
	byte MacAddress[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	IPAddress SubnetMask;

	EthernetInterface(const char* name);

	virtual bool Activate() override;
	virtual bool AddConfig() override;
	virtual bool Deactivate() override;
	virtual DateTime* GetNetworkTime(DateTime* out) override;
	virtual void LogStatus(bool refresh = false) override;
	virtual bool LookupHost(const char* host, IPAddress& ipAddress) override;
	virtual bool Ping(const char* host) override;
};

#endif

#endif

