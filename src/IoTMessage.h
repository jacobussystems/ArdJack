/*
	IoTMessage.h

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

class StringList;



class IoTMessage
{
protected:
	// Indexes in '_Items'.
	static const uint8_t MESSAGE_FROM_PATH = 0;							// source path (format 1 only)
	static const uint8_t MESSAGE_RETURN_PATH = 1;						// return path (format 1 only, and only
																		// if not equal to the 'From' path)
	static const uint8_t MESSAGE_TEXT = 2;								// the text, e.g. a command, request or response
	static const uint8_t MESSAGE_TO_PATH = 3;							// destination path (format 1 only)
	static const uint8_t MESSAGE_TYPE = 4;								// the message type
	static const uint8_t MESSAGE_WIRETEXT = 5;							// full transmitted/received text,
																		// e.g. text (format 0), header + text (format 1)
	StringList* _Items;

	virtual void Clear();
	//virtual bool DecodeFormat0(const char* line);
	virtual bool DecodeFormat1(const char* line);
	virtual bool EncodeFormat0();
	virtual bool EncodeFormat1();

public:
	uint8_t Format;														// enumeration: ARDJACK_MESSAGE_FORMAT_0 etc.

	IoTMessage();
	~IoTMessage();

	static bool CreateMessageToSend(const char* type, int format, const char* text, IoTMessage* msg, const char* fromPath = "",
		const char* toPath = "", const char* returnPath = "");
	virtual bool Decode(const char* line);
	virtual bool Encode();
	virtual const char* FromPath();
	virtual void LogIt();
	virtual const char* ReturnPath();
	virtual bool SetFromPath(const char* text);
	virtual bool SetReturnPath(const char* text);
	virtual bool SetText(const char* text);
	virtual bool SetToPath(const char* text);
	virtual bool SetType(const char* text);
	virtual bool SetWireText(const char* text);
	virtual const char* Text();
	virtual const char* ToPath();
	virtual const char* ToString(char* text);
	virtual const char* Type();
	virtual const char* WireText();
};

