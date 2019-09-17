/*
	WiFiInterface.h

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
#include "Globals.h"


#ifdef ARDJACK_WIFI_AVAILABLE

#include "IoTObject.h"
#include "NetworkInterface.h"
#include "WiFiLibrary.h"



class WiFiInterface : public NetworkInterface
{
protected:
	int _ConnectMode;													// connect mode enumeration: ARDJACK_WIFI_CONNECT_OPEN, etc.
	Enumeration* _ConnectModes;
	char _ConnectModeStr[20];
	char _DHCPHostname[30];
	int _KeyIndex;														// key index (for WEP only)
	char _Password[30];
	char _SSID[30];
	int _Status;
	int _Timeout;														// timeout (ms)

	virtual bool Connect();
	virtual bool Disconnect();
	virtual bool GetConnectionInfo();
	virtual char* GetStatusAsString(int status, char *out);

public:
	char BSSIDStr[20];
	int Channel;
	int Encryption;														// encryption type enumeration: ENC_TYPE_AUTO, etc.
	IPAddress GatewayIp;
	IPAddress LocalIp;
	char MacStr[20];
	IPAddress SubnetMask;

	WiFiInterface(const char* name);

	virtual bool Activate() override;
	virtual bool AddConfig() override;
	virtual bool Deactivate() override;
	static char* GetEncryptionTypeName(int type, char* out);
	virtual DateTime* GetNetworkTime(DateTime* out) override;
	virtual void LogStatus(bool refresh = false) override;
	virtual bool LookupHost(const char* host, IPAddress& ipAddress) override;
	virtual bool Ping(const char* host) override;
	virtual bool ScanWiFi() override;
	virtual int SignalStrength();
};

#endif

#endif

