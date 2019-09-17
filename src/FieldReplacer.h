/*
	FieldReplacer.h

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

#include "Dictionary.h"
#include "Globals.h"
#include "IoTObject.h"


struct ReplaceFieldParams
{
	char DateFormat[20];
	char Intro[6];
	char Outro[6];
	bool ReplaceSpecialFields;
	char SpecialFieldPrefix[6];
	char TimeFormat[20];
};

typedef void (*ReplaceFieldCallback)(const char* fieldExpr, Dictionary* args, char* value, ReplaceFieldParams* params);

char* FieldReplacer_GetTimeString(char* value, const char* format = "HH:mm:ss", bool utc = false);
void FieldReplacer_ReplaceField(const char* fieldExpr, Dictionary* args, char* value, ReplaceFieldParams* params);
char* FieldReplacer_ReplaceSpecialField(const char* fieldExpr, char* value, ReplaceFieldParams* params);



class FieldReplacer
{
protected:
	virtual const char* FindMatchingOutro(const char* text);
	//virtual bool ProcessEscapes_1(const char* text, char* out);
	//virtual bool ProcessEscapes_2(const char* text, char* out);

public:
	ReplaceFieldParams Params;

	FieldReplacer();

	virtual bool ReplaceFields(const char* text, Dictionary* args, ReplaceFieldCallback callback, char* value);
};

