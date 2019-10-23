/*
	Route.h

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

#include "FifoBuffer.h"
#include "IoTMessage.h"
#include "MessageFilter.h"

class Connection;
class Device;
class Route;
class IoTMessage;

typedef void(*Route_Callback)(void* caller, Route* route, IoTMessage* msg);



class Route
{
protected:

public:
	bool AlwaysUse;
	FifoBuffer *Buffer;										// a 'FifoBuffer' (for asynchronous working)
	Route_Callback Callback;								// a callback (for synchronous working)
	void* CallbackObj;										// callback object (for synchronous working)
	MessageFilter Filter;
	char Name[ARDJACK_MAX_NAME_LENGTH];
	bool StopOnRouted;
	Device* Target;											// set if the Route is created by a Device
	//char Target[ARDJACK_MAX_NAME_LENGTH];					// target object (for types: Command, Request, Response)
	int Type;

	Route(const char* name, int type = ARDJACK_ROUTE_TYPE_NONE);

	static char* GetTypeName(int type, char* out);
	virtual bool Handle(IoTMessage* msg);
	virtual void SetTextFilter(const char* text, int operation = ARDJACK_STRING_COMPARE_STARTS_WITH);
};

