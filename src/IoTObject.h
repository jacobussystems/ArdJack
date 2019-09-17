/*
	IoTObject.h

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
#else
	#include "stdafx.h"
#endif

#include "Configuration.h"
#include "Globals.h"

class StringList;



class IoTObject
{
protected:
	bool _Active;

	virtual bool Activate();
	virtual bool ConfigureProperty(const char* propName, const char* propValue, const char* text);
	virtual bool Deactivate();

public:
	Configuration* Config;
	char Name[ARDJACK_MAX_NAME_LENGTH];
	uint8_t Subtype;															// enumeration, e.g. ARDJACK_CONNECTION_SUBTYPE_SERIAL
	uint8_t Type;																// enumeration, e.g. ARDJACK_OBJECT_TYPE_BRIDGE

	IoTObject(const char* name);
	~IoTObject();

	virtual bool Active();
	virtual bool AddConfig();
	virtual bool ApplyConfig(bool quiet = false);
	virtual bool Configure(const char* entity, StringList* settings, int start, int count);
	virtual bool Poll();
	virtual bool SetActive(bool state);
	virtual bool Special(int action);
	virtual const char* ToString(char* text);
	virtual bool ValidateConfig(bool quiet = false);
};

