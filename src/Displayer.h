/*
	Displayer.h

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
#else
	#include "stdafx.h"
#endif

#include "Globals.h"




class Displayer
{
protected:
	static bool DisplayHeader(const char* heading, const char* name = "");
	static bool DisplayObjectsOfType(int type);
	static bool SaveLogIncludeMemory;
	static bool SaveLogIncludeTime;

public:
	static bool DisplayActive(IoTObject* obj);
#ifdef ARDJACK_INCLUDE_BEACONS
	static bool DisplayBeacon(Beacon* Beacon);
	static bool DisplayBeacons();
#endif
#ifdef ARDJACK_INCLUDE_BRIDGES
	static bool DisplayBridge(Bridge* bridge);
	static bool DisplayBridges();
#endif
	static bool DisplayConnection(Connection* conn);
	static bool DisplayConnections();
#ifdef ARDJACK_INCLUDE_DATALOGGERS
	static bool DisplayDataLogger(DataLogger* logger);
	static bool DisplayDataLoggers();
#endif
	static bool DisplayDevice(Device* dev);
	static bool DisplayDevices();
	static bool DisplayFilter(Filter* filter);
	static bool DisplayFilters();
	static bool DisplayItem(const char* args);
	static bool DisplayMacros();
	static bool DisplayMemory();
	static bool DisplayNetwork();
#ifdef ARDJACK_NETWORK_AVAILABLE
	static bool DisplayNetworkInterface(NetworkInterface* net);
#endif
	static bool DisplayObject(IoTObject* obj);
	static bool DisplayObject2(char* text, bool quiet = false);
	static bool DisplayObjects(bool activeOnly);
	static bool DisplayPart(Device* dev, Part* part);
	static bool DisplaySize(const char* caption, int size);
	static bool DisplaySizes();
	static bool DisplayStatus();
};

