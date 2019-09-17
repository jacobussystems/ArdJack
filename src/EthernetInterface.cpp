/*
	EthernetInterface.cpp

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

#include <arduino.h>
#include "DetectBoard.h"

#ifdef ARDJACK_ETHERNET_AVAILABLE

//#include <SPI.h>
#include <Ethernet.h>

#include "EthernetInterface.h"
#include "Globals.h"
#include "Log.h"
#include "NetworkInterface.h"
#include "Table.h"
#include "Utils.h"




EthernetInterface::EthernetInterface(const char* name)
	: NetworkInterface(name)
{
	_ConnectModeStr[0] = NULL;
	_CSPin = 10;
	_DHCPHostname[0] = NULL;
	//_Status = WL_IDLE_STATUS;
	Ethernet.linkStatus() == LinkOFF;
	//BSSID = 0;
	//MacAddress = 0;
}


bool EthernetInterface::Activate()
{
	if (!IoTObject::Activate()) return false;

	Config.GetAsInteger("CSPin", &_CSPin);
	Config.GetAsString("DHCPHostname", _DHCPHostname);

	// Use Ethernet.init(pin) to configure the CS pin.
	//Ethernet.init(10);  // Most Arduino shields
	//Ethernet.init(5);   // MKR ETH shield
	//Ethernet.init(0);   // Teensy 2.0
	//Ethernet.init(20);  // Teensy++ 2.0
	//Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
	//Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet

	Ethernet.init(_CSPin);

	if (!Connect())
		return false;

	return true;
}


bool EthernetInterface::Connect()
{
	char lineBuffer[202];
	char temp[102];

	// Attempt to connect.
	sprintf(lineBuffer, PRM("Connecting..."));
	Log::LogInfo(lineBuffer);

	Log::LogInfo("Initialize Ethernet with DHCP");

	if (Ethernet.begin(MacAddress) == 0)
	{
		Log::LogError("Failed to configure Ethernet using DHCP");

		// Check for Ethernet hardware present
		if (Ethernet.hardwareStatus() == EthernetNoHardware)
		{
			Log::LogError("Ethernet hardware not found.");
			return false;
		}

		if (Ethernet.linkStatus() == LinkOFF)
		{
			Log::LogError("Ethernet cable is not connected");
			return false;
		}

		// Try to configure using IP address instead of DHCP.
		Ethernet.begin(MacAddress, LocalIp, DnsIp);
	}

	// Give the Ethernet interface a second to initialize.
	delay(1000);

	GetConnectionInfo();

	// Now connected.
	LogStatus(true);

	return true;
}


bool EthernetInterface::Deactivate()
{
	Disconnect();

	return IoTObject::Deactivate();
}


bool EthernetInterface::Disconnect()
{
	char temp[102];
	sprintf(temp, PRM("Disconnecting"));
	Log::LogInfo(temp);

	//Ethernet.end();

	return true;
}


bool EthernetInterface::GetConnectionInfo()
{
	if (Globals::Verbosity > 3)
		Log::LogInfo(PRM("EthernetInterface::GetConnectionInfo"));

	GatewayIp = Ethernet.gatewayIP();
	LocalIp = Ethernet.localIP();
	SubnetMask = Ethernet.subnetMask();

	return true;
}


DateTime* EthernetInterface::GetNetworkTime(DateTime* out)
{
	char temp[82];

	//// Get the time, as seconds since January 1st, 1970.
	//// (WiFi gets it periodically from an internet NNTP server.)
	//long seconds = WiFi.getTime();
	//Log::LogInfo("WiFi.getTime: ", ltoa(seconds, temp, 10));

	//Utils::SecondsToTime(seconds, out);

	return out;
}


char* EthernetInterface::GetStatusAsString(int status, char *out)
{
	sprintf(out, "%s: ", Name);

	switch (status)
	{

	default:
		sprintf(out, PRM("%s: UNRECOGNISED ETHERNET STATUS: %d"), Name, status);
		break;
	}

	return out;
}


bool EthernetInterface::AddConfig()
{
	if (!NetworkInterface::AddConfig())
		return false;

	if (Config.AddIntegerItem(PRM("CSPin"), PRM("CS pin number."), _CSPin) == NULL) return false;
	if (Config.AddStringItem(PRM("DHCPHostname"), PRM("DHCP hostname."), "") == NULL) return false;

	return Config.SortItems();
}


void EthernetInterface::LogStatus(bool refresh)
{
	if (refresh)
		GetConnectionInfo();

	char lineBuffer[102];
	char temp[40];

	// Log the connection info.
	Log::LogInfo(PRM("   Local IP:        "), Utils::IpAddressToString(LocalIp, temp));
	Log::LogInfo(PRM("   MAC address:     "), Utils::MacAddressToString(MacAddress, temp));
	//Log::LogInfo(PRM("   MAC (router):    "), Utils::MacAddressToString(BSSID, temp));
	Log::LogInfo(PRM("   Subnet mask:     "), Utils::IpAddressToString(SubnetMask, temp));
	Log::LogInfo(PRM("   Gateway IP:      "), Utils::IpAddressToString(GatewayIp, temp));
}


bool EthernetInterface::LookupHost(const char* host, IPAddress& ipAddress)
{
	return false;

	//char temp[82];
	//int result = Ethernet.hostByName(host, ipAddress);

	//if (result != 1)
	//{
	//	sprintf(temp, PRM("DNS LOOKUP FAILED: Host '%s', error code %d"), host, result);
	//	Log::LogError(temp);
	//	return false;
	//}

	//return true;
}


bool EthernetInterface::Ping(const char* host)
{
	return false;

	//char temp[82];
	//Log::LogInfo("Pinging ", host, "...");

	//int pingResult = Ethernet.ping(host);

	//if (pingResult < 0)
	//{
	//	sprintf(temp, PRM("PING FAILED: Error code = %d"), pingResult);
	//	Log::LogError(temp);
	//	return false;
	//}

	//sprintf(temp, PRM("PING SUCCESS: RTT = %d ms"), pingResult);
	//Log::LogInfo(temp);

	//return true;
}

#endif

#endif
