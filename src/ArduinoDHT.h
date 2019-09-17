/*
	ArduinoDHT.h

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

#ifdef ARDJACK_INCLUDE_ARDUINO_DHT


/*
	Uses the 'SimpleDHT' library from:
		https://github.com/winlinvip/SimpleDHT

	By default uses pin 2 for the DHT sensor data, so connect:
		'+'		+5V (or +3.3V)
		'out'	pin 2
		'-'		Ground

	Can reconfigure 'Pin' for the DHT Part, e.g. to swap input 'di0' (usually pin 3) and the DHT sensor (usually pin 2)
	on Device 'ard':
		configure ard.di0 pin=2
		configure ard.dht0 pin=3
*/

#include <SimpleDHT.h>

#include "UserPart.h"



class ArduinoDHT : public UserPart
{
protected:
	SimpleDHT11* _Dht11;
	int _Dht11_MinInterval;
	SimpleDHT22* _Dht22;
	int _Dht22_MinInterval;
	int _Interval;
	int _Model;
	char _ModelName[ARDJACK_MAX_NAME_LENGTH];
	long _NextSampleTime;
	int _Variable;
	char _VariableName[ARDJACK_MAX_NAME_LENGTH];

	virtual bool DeleteObjects();

public:
	ArduinoDHT();
	~ArduinoDHT();

	virtual bool Activate() override;
	virtual bool AddConfig() override;
	virtual bool Deactivate() override;
	virtual bool Read(Dynamic* value) override;
};


#endif

#endif
