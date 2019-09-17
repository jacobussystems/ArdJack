/*
	WiFiInterface.cpp

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


#ifdef ARDJACK_WIFI_AVAILABLE

#include "Enumeration.h"
#include "Globals.h"
#include "Log.h"
#include "NetworkInterface.h"
#include "Table.h"
#include "Utils.h"
#include "WiFiInterface.h"
#include "WiFiLibrary.h"




WiFiInterface::WiFiInterface(const char* name)
	: NetworkInterface(name)
{
	_ConnectModeStr[0] = NULL;
	_DHCPHostname[0] = NULL;
	_KeyIndex = 0;
	_Password[0] = NULL;
	_SSID[0] = NULL;
	_Status = WL_IDLE_STATUS;
	_Timeout = 10000;

	_ConnectModes = new Enumeration(PRM("ConnectModes"));
	_ConnectModes->Add(PRM("Open"), ARDJACK_WIFI_CONNECT_OPEN);
	_ConnectModes->Add(PRM("WPA"), ARDJACK_WIFI_CONNECT_WPA);
	_ConnectModes->Add(PRM("WEP"), ARDJACK_WIFI_CONNECT_WEP);

	//BSSID = 0;
	BSSIDStr[0] = NULL;
	Channel = -1;
	//MacAddress = 0;

#ifdef ARDJACK_ESP32
	Encryption = WIFI_AUTH_OPEN;
#else
	Encryption = ENC_TYPE_NONE;
#endif
}


bool WiFiInterface::Activate()
{
	if (!IoTObject::Activate()) return false;

	char temp[20];
	Config->GetAsString(PRM("ConnectMode"), temp);
	_ConnectMode = _ConnectModes->LookupName(temp, ARDJACK_WIFI_CONNECT_WPA);
	strcpy(_ConnectModeStr, _ConnectModes->LookupValue(_ConnectMode));

	Config->GetAsString(PRM("DHCPHostname"), _DHCPHostname);
	Config->GetAsInteger(PRM("KeyIndex"), &_KeyIndex);
	Config->GetAsString(PRM("Password"), _Password);
	Config->GetAsString(PRM("SSID"), _SSID);
	Config->GetAsInteger(PRM("Timeout"), &_Timeout);

	//WiFi.setHostname(_DHCPHostname);

#ifdef ARDJACK_ESP32
#else
	WiFi.setTimeout(_Timeout);
#endif

	if (NULL == _SSID[0])
	{
		Log::LogError(PRM("WiFiInterface::Activate: No WiFi SSID"));
		return false;
	}

	if (NULL == _Password[0])
	{
		Log::LogError(PRM("WiFiInterface::Activate: No WiFi password"));
		return false;
	}

	if (!Connect())
		return false;

	return true;
}


bool WiFiInterface::AddConfig()
{
	if (!NetworkInterface::AddConfig())
		return false;

	if (Config->AddStringItem(PRM("ConnectMode"), PRM("Connect mode (Open | WPA | WEP)."), "WPA") == NULL) return false;
	if (Config->AddStringItem(PRM("DHCPHostname"), PRM("DHCP hostname."), "") == NULL) return false;
	if (Config->AddIntegerItem(PRM("KeyIndex"), PRM("Key index (0-3, WEP only)."), _KeyIndex) == NULL) return false;
	if (Config->AddStringItem(PRM("Password"), PRM("Password/passphrase."), "") == NULL) return false;
	if (Config->AddStringItem(PRM("SSID"), PRM("SSID."), "") == NULL) return false;
	if (Config->AddIntegerItem(PRM("Timeout"), PRM("Timeout."), _Timeout, "ms") == NULL) return false;

	return Config->SortItems();
}


bool WiFiInterface::Connect()
{
	char temp[102];

#ifdef ARDJACK_ESP32
	WiFi.mode(WIFI_STA);
#endif

	_Status = WiFi.status();
	Log::LogInfo(Name, ": ", GetStatusAsString(_Status, temp));

#ifdef ARDJACK_ESP32
#else
	// Check for the presence of WiFi h/w.
	if (_Status == WL_NO_SHIELD)
	{
		Log::LogError(PRM("WiFi hardware not present"));
		return false;
	}
#endif

	// Attempt to connect to the WiFi network.
	for (int i = 0; i < 4; i++)
	{
		Log::LogInfo(PRM("Attempting to connect to: "), _SSID);

		switch (_ConnectMode)
		{
		case ARDJACK_WIFI_CONNECT_OPEN:
			// Connect to an open network.
			_Status = WiFi.begin(_SSID);
			break;

		case ARDJACK_WIFI_CONNECT_WEP:
			// Connect to a WEP network.
#ifdef ARDJACK_ESP32
			Log::LogError(PRM("Sorry, WEP connections are not yet supported for ESP32"));
			return false;
#else
			_Status = WiFi.begin(_SSID, _KeyIndex, _Password);
			break;
#endif

		case ARDJACK_WIFI_CONNECT_WPA:
			// Connect to a WPA/WPA2 network.
			_Status = WiFi.begin(_SSID, _Password);
			break;
		}

		// Wait a while for a connection.
		for (int j = 0; j < 20; j++)
		{
			delay(500);

			if (WiFi.status() == WL_CONNECTED)
				break;
		}

		_Status = WiFi.status();

		char temp2[82];
		sprintf(temp, PRM("%s: %s (%d)"), Name, GetStatusAsString(_Status, temp2), _Status);
		Log::LogInfo(temp);

		if (_Status == WL_CONNECTED)
			break;
	}

	if (_Status != WL_CONNECTED)
	{
		sprintf(temp, PRM("%s: Failed to connect to ssid '%s' (mode %s) - timed out"), Name, _SSID, _ConnectModeStr);
		Log::LogError(temp);

		return false;
	}

	// Connected.
#ifdef ARDJACK_ESP32
#else
	Log::LogInfo(PRM("WiFi firmware version: "), WiFi.firmwareVersion());
#endif

	LogStatus(true);

	return true;
}


bool WiFiInterface::Deactivate()
{
	Disconnect();

	return IoTObject::Deactivate();
}


bool WiFiInterface::Disconnect()
{
	Log::LogInfo(PRM("Disconnecting from: %s"), _SSID);

#ifdef ARDJACK_ESP32
	WiFi.disconnect();
#else
	WiFi.end();
#endif

	return true;
}


bool WiFiInterface::GetConnectionInfo()
{
	if (Globals::Verbosity > 3)
		Log::LogInfo(PRM("WiFiInterface::GetConnectionInfo"));

	// Get the MAC address of this WiFi device.
#ifdef ARDJACK_ESP32
	strcpy(MacStr, WiFi.macAddress().c_str());
#else
	uint8_t macAddress[6];
	WiFi.macAddress(macAddress);
	Utils::MacAddressToString(macAddress, MacStr);
#endif

	// Get the BSSID (MAC address of the router).
#ifdef ARDJACK_ESP32
	strcpy(BSSIDStr, WiFi.BSSIDstr().c_str());
#else
	uint8_t bssid[6];
	WiFi.BSSID(bssid);
	Utils::MacAddressToString(bssid, BSSIDStr);
#endif

	// Get the encryption type.
#ifdef ARDJACK_ESP32
	//Encryption = WiFi.encryptionType();
#else
	Encryption = WiFi.encryptionType();
#endif

	//Channel = (int)WiFi.channel();

	GatewayIp = WiFi.gatewayIP();
	LocalIp = WiFi.localIP();
	SubnetMask = WiFi.subnetMask();

	// Set the global IP address.
	Utils::IpAddressToString(LocalIp, Globals::IpAddress);

	return true;
}


#ifdef ARDJACK_ESP32

	char* WiFiInterface::GetEncryptionTypeName(int type, char* out)
	{
		// Get the name of encryption type 'type'.
		switch (type)
		{
		case WIFI_AUTH_OPEN:
			strcpy(out, "Open");
			break;

		case WIFI_AUTH_WPA_PSK:
			strcpy(out, "WPA PSK");
			break;

		case WIFI_AUTH_WPA_WPA2_PSK:
			strcpy(out, "WPA WPA2 OSK");
			break;

		case WIFI_AUTH_WPA2_ENTERPRISE:
			strcpy(out, "WPA2 Enterprise");
			break;

		case WIFI_AUTH_WPA2_PSK:
			strcpy(out, "WPA2 PSK");
			break;

		case WIFI_AUTH_WEP:
			strcpy(out, "WEP");
			break;

		default:
			strcpy(out, "Unknown");
			break;
		}

		return out;
	}

#else

	char* WiFiInterface::GetEncryptionTypeName(int type, char* out)
	{
		// Get the name of encryption type 'type'.
		switch (type)
		{
		case ENC_TYPE_AUTO:
			strcpy(out, "Auto");
			break;

		case ENC_TYPE_CCMP:
			strcpy(out, "WPA2");
			break;

		case ENC_TYPE_NONE:
			strcpy(out, "None");
			break;

		case ENC_TYPE_TKIP:
			strcpy(out, "WPA");
			break;

		case ENC_TYPE_WEP:
			strcpy(out, "WEP");
			break;

		default:
			strcpy(out, "Unknown");
			break;
		}

		return out;
	}

#endif


	DateTime* WiFiInterface::GetNetworkTime(DateTime* out)
	{
#ifdef ARDJACK_ESP32
		Utils::SecondsToTime(0, out);
#else
		// Get the time, as seconds since January 1st, 1970.
		// (WiFi gets it periodically from an internet NNTP server.)
		long seconds = WiFi.getTime();

		if (Globals::Verbosity > 3)
		{
			char temp[82];
			Log::LogInfo(PRM("WiFi.getTime: "), ltoa(seconds, temp, 10));
		}

		Utils::SecondsToTime(seconds, out);
#endif

	return out;
}


char* WiFiInterface::GetStatusAsString(int status, char *out)
{
	out[0] = NULL;

	switch (status)
	{
#ifdef ARDJACK_ESP32
#else
	case WL_AP_CONNECTED:
		strcat(out, PRM("AP_CONNECTED"));
		break;

	case WL_AP_FAILED:
		strcat(out, PRM("AP_FAILED"));
		break;

	case WL_AP_LISTENING:
		strcat(out, PRM("AP_LISTENING"));
		break;
#endif

	case WL_CONNECTED:
		strcat(out, PRM("CONNECTED"));
		break;

	case WL_CONNECT_FAILED:
		strcat(out, PRM("CONNECT_FAILED"));
		break;

	case WL_CONNECTION_LOST:
		strcat(out, PRM("CONNECTION_LOST"));
		break;

	case WL_DISCONNECTED:
		strcat(out, PRM("DISCONNECTED"));
		break;

	case WL_IDLE_STATUS:
		strcat(out, PRM("IDLE_STATUS"));
		break;

	case WL_NO_SHIELD:
		strcat(out, PRM("NO_SHIELD"));
		break;

	case WL_NO_SSID_AVAIL:
		strcat(out, PRM("NO_SSID_AVAIL"));
		break;

#ifdef ARDJACK_FEATHER_M0
	case WL_PROVISIONING:
		strcat(out, PRM("PROVISIONING"));
		break;

	case WL_PROVISIONING_FAILED:
		strcat(out, PRM("PROVISIONING_FAILED"));
		break;
#endif

	case WL_SCAN_COMPLETED:
		strcat(out, PRM("SCAN_COMPLETED"));
		break;

	default:
		sprintf(out, PRM("%s: UNRECOGNISED WiFi STATUS: %d"), Name, status);
		break;
	}

	return out;
}


void WiFiInterface::LogStatus(bool refresh)
{
	if (refresh)
		GetConnectionInfo();

	char lineBuffer[102];
	char temp[40];

	char chan[20];
	char encType[20];
	char ssid[20];
	char strength[20];

	//itoa(WiFi.channel(), chan, 10);
	sprintf(strength, "%d dbm", (int)WiFi.RSSI());

	// Log the SSID of the network we're connected to and the WiFi connection info.
#ifdef ARDJACK_ESP32
	strcpy(encType, "?");
#else
	GetEncryptionTypeName(WiFi.encryptionType(), encType);

	if (strlen(WiFi.SSID()) > 0)
		strcpy(ssid, WiFi.SSID());
	else
		strcpy(ssid, "?");

	Log::LogInfo(PRM("   SSID:               "), ssid);
#endif

	//Log::LogInfo(PRM("   WiFi channel:       "), chan);
#ifdef ARDJACK_ESP32
	Log::LogInfo(PRM("   Encryption type:    "), encType);
#endif
	Log::LogInfo(PRM("   Local IP:           "), Utils::IpAddressToString(LocalIp, temp));
	Log::LogInfo(PRM("   MAC address:        "), MacStr);
	Log::LogInfo(PRM("   Router MAC address: "), BSSIDStr);
	Log::LogInfo(PRM("   Subnet mask:        "), Utils::IpAddressToString(SubnetMask, temp));
	Log::LogInfo(PRM("   Gateway IP:         "), Utils::IpAddressToString(GatewayIp, temp));
	Log::LogInfo(PRM("   Signal (RSSI):      "), strength);
}


bool WiFiInterface::LookupHost(const char* host, IPAddress& ipAddress)
{
	if (!Utils::StringContains(host, "."))
	{
		char temp[82];
		sprintf(temp, PRM("DNS LOOKUP FAILED: Bad host name: '%s'"), host);
		Log::LogError(temp);
		return false;
	}

	int result = WiFi.hostByName(host, ipAddress);

	if (result != 1)
	{
		char temp[82];
		sprintf(temp, PRM("DNS LOOKUP FAILED: Host '%s', error code %d"), host, result);
		Log::LogError(temp);
		return false;
	}

	return true;
}


bool WiFiInterface::Ping(const char* host)
{
#ifdef ARDJACK_ESP32
	Log::LogWarning("PING not available for ESP32");
	return false;
#else
	char temp[82];
	Log::LogInfo(PRM("Pinging "), host, "...");

	int pingResult = WiFi.ping(host);

	if (pingResult < 0)
	{
		sprintf(temp, PRM("PING FAILED: Error code = %d"), pingResult);
		Log::LogError(temp);
		return false;
	}

	sprintf(temp, PRM("PING SUCCESS: RTT = %d ms"), pingResult);
	Log::LogInfo(temp);

	return true;
#endif
}


bool WiFiInterface::ScanWiFi()
{
	char temp[102];

	Log::LogInfo(PRM("Scanning WiFi network..."));
	int count = WiFi.scanNetworks();

	sprintf(temp, PRM("%d WiFi networks found:"), count);
	Log::LogInfo(temp);

	Table table;
	table.AddColumn("SSID", 20);
	table.AddColumn("Chan", 8);
	table.AddColumn("Encryption", 18);
	table.AddColumn("Signal", 10);

	Log::LogInfo(table.HorizontalLine(temp));
	Log::LogInfo(table.Header(temp));
	Log::LogInfo(table.HorizontalLine(temp));

	char chan[20];
	char encType[20];
	char ssid[20];
	char strength[20];

	for (int i = 0; i < count; i++)
	{
		itoa(WiFi.channel(i), chan, 10);
		GetEncryptionTypeName(WiFi.encryptionType(i), encType);
		sprintf(strength, "%d dbm", (int)WiFi.RSSI(i));

		strcpy(ssid, "?");

#ifdef ARDJACK_ESP32
		if (strlen(WiFi.SSID(i).c_str()) > 0)
			strcpy(ssid, WiFi.SSID(i).c_str());
#else
		if (strlen(WiFi.SSID(i)) > 0)
			strcpy(ssid, WiFi.SSID(i));
#endif

		Log::LogInfo(table.Row(temp, ssid, chan, encType, strength));
	}

	Log::LogInfo(table.HorizontalLine(temp));

	return true;
}


int WiFiInterface::SignalStrength()
{
	return (int)WiFi.RSSI();
}

#endif

#endif
