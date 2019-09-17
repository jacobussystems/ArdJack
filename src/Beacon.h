/*
	Beacon.h

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

#include "DateTime.h"
//#include "Dictionary.h"
#include "FieldReplacer.h"
#include "Globals.h"
#include "IoTObject.h"


#ifdef ARDJACK_INCLUDE_BEACONS

class Dictionary;
class FieldReplacer;
class Part;

//void Beacon_ReplaceField(const char* fieldExpr, Dictionary* args, char* value, ReplaceFieldParams* params);



class Beacon : public IoTObject
{
protected:
	char _Command[82];
	char _DateFormat[22];
	FieldReplacer* _FieldReplacer;
	int _Interval;																				// ms
	long _NextOutputTime;
	Part* _Part;
	Dictionary* _Substitutes;
	IoTObject* _Target;
	bool _TargetWasActive;
	char _Text[82];
	char _TimeFormat[22];

	virtual bool Activate() override;
	virtual bool ApplyConfig(bool quiet) override;
	virtual char* ApplyReplacements(const char* text, char* output);
	virtual bool Deactivate() override;
	virtual bool Output();
	virtual bool Send(const char* text);

public:
	int Events;

	Beacon(const char* name);
	~Beacon();

	virtual bool AddConfig();
	//virtual bool Configure(char settings[][ARDJACK_MAX_VALUE_LENGTH], int count);
	virtual bool Poll() override;
};

#endif
