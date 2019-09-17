/*
	Part.h

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

#include "Dynamic.h"
#include "Globals.h"

class Configuration;
class Filter;
class StringList;



class Part
{
protected:

public:
	Configuration* Config;
	Filter* Filt;
	char FilterName[ARDJACK_MAX_NAME_LENGTH];
	uint8_t ItemCount;
#ifdef ARDJACK_INCLUDE_MULTI_PARTS
	Part* Items[ARDJACK_MAX_MULTI_PART_ITEMS];								// array of 'Part'
#endif
	bool LastChangeState;												// last change state, for debounce filtering
	int LastChangeTime;													// last change time (ms), for debounce filtering
	char Name[ARDJACK_MAX_NAME_LENGTH];
	int NotifiedTime;													// last notified time (ms)
	Dynamic NotifiedValue;												// last notified value
	bool Notify;														// send a notification when this Part's value changes?
	uint8_t Pin;														// pin / GPIO channel / channel etc.
	uint8_t Subtype;
	uint8_t Type;
	Dynamic Value;														// normally, a numeric value

	Part();
	~Part();

	virtual bool Activate();
	virtual bool AddConfig();
	virtual bool CheckChange();
	virtual bool Configure(StringList* settings, int start, int count);
#ifdef ARDJACK_INCLUDE_MULTI_PARTS
	virtual bool ConfigureMulti(const char* text);
#endif
	virtual bool Deactivate();
	virtual bool IsAnalog();
	virtual bool IsAnalogInput();
	virtual bool IsAnalogOutput();
	virtual bool IsDigital();
	virtual bool IsDigitalInput();
	virtual bool IsDigitalOutput();
	virtual bool IsInput();
	virtual bool IsOutput();
	virtual bool Read(Dynamic* value);
	virtual bool Write(Dynamic* value);
};

