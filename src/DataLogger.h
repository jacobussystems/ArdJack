/*
	DataLogger.h

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
#include "IoTObject.h"


#ifdef ARDJACK_INCLUDE_DATALOGGERS

class Connection;
class Device;
class FieldReplacer;
class Route;
class IoTMessage;
class Part;



// For now, this is limited to Device -> Connection.

class DataLogger : public IoTObject
{
protected:
	Connection* _Connection;
	bool _ConnectionWasActive;
	char _DateFormat[22];
	Device* _Device;
	bool _DeviceWasActive;
	FieldReplacer* _FieldReplacer;
	int _Interval;																// sample interval (ms)
	long _NextSampleTime;
	char _OutputFormat[40];
	char _Prefix[30];
	char _TimeFormat[22];

	virtual bool Activate() override;
	virtual bool Deactivate() override;

public:
	int Events;
	uint8_t PartCount;
	Part* Parts[ARDJACK_MAX_DATALOGGER_PARTS];

	DataLogger(const char* name);
	~DataLogger();

	//virtual bool Connection_Callback(Route* route, IoTMessage* msg);
	virtual bool AddConfig() override;
	virtual bool ApplyConfig(bool quiet) override;
	virtual bool Poll() override;
	virtual bool Sample();
	//virtual bool Configure(char settings[][ARDJACK_MAX_VALUE_LENGTH], int count);
};

#endif
