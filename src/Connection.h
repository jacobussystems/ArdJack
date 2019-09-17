/*
	Connection.h

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

#include "IoTMessage.h"
#include "IoTObject.h"

class FifoBuffer;
class Route;



class Connection : public IoTObject
{
protected:
	bool _CanInput;
	bool _CanOutput;
	char _CommandPrefix[10];
	char _CommentPrefix[10];
	bool _InputAnnounce;
	char _InputEncodingType[10];
	IoTMessage _InputMsg;
	bool _OutputAnnounce;
	char _OutputEncodingType[10];
	bool _WarnUnhandled;

	virtual bool Activate() override;
	virtual bool CheckAnnounce(bool output, const char* text);
	virtual bool ConfigureProperty(const char* propName, const char* propValue, const char* text);
	virtual bool ConfigureRoute(const char* propName, const char* propValue, const char* text);

public:
	Route* DefaultRoute;
	uint8_t RouteCount;
	Route* Routes[ARDJACK_MAX_INPUT_ROUTES];
	int RxCount;
	int RxEvents;
	int TxCount;
	int TxEvents;

	Connection(const char* name);
	~Connection();

	virtual bool AddConfig() override;
	virtual Route* AddRoute(Route* route);
	virtual Route* AddRoute(const char* name, int type, FifoBuffer* buffer, const char* prefix = "");
	virtual bool AlwaysUseRoute(const char* name, bool state = true);
	virtual bool ClearRoutes();
	virtual Route* LookupRoute(const char* name, bool quiet = false);
	virtual int LookupRouteIndex(const char* name, bool quiet = false);
	virtual bool OutputMessage(IoTMessage* msg);
	virtual bool OutputText(const char* text);
	virtual bool Poll() override;
	virtual bool PollInputs(int maxCount = 5);
	virtual bool PollOutputs(int maxCount = 5);
	virtual bool ProcessInput(const char* text);
	virtual bool RemoveRoute(const char* name);
	virtual bool RouteInputMessage(IoTMessage* msg, bool* routed);
	virtual bool SendQueuedOutput(const char* text);
	virtual bool SendText(const char* text);
	virtual bool SendTextQuiet(const char* text);
};

