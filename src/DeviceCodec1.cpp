/*
	DeviceCodec1.cpp

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

#include "pch.h"

#ifdef ARDUINO
#else
	#include "stdafx.h"
#endif

#include "Device.h"
#include "DeviceCodec1.h"
#include "Log.h"
#include "Part.h"
#include "StringList.h"
#include "Utils.h"



DeviceCodec1::DeviceCodec1()
{
	if (Globals::Verbosity > 7)
		Log::LogInfo(PRM("DeviceCodec1 ctor"));
}


int DeviceCodec1::DecodeRequest(const char* line, char *aName, StringList* values)
{
	// Example formats:
	//		?ai0
	//		read ai0
	//		!di0 1
	//		write di0 1
	//		configure x=y xx=yy
	//		getcount di
	//		?ai.count

	int oper = ARDJACK_OPERATION_NONE;
	strcpy(aName, "");
	values->Clear();

	char useLine[200];
	strcpy(useLine, line);
	Utils::Trim(useLine);

	StringList fields;
	int fieldCount = Utils::SplitText(useLine, ' ', &fields, ARDJACK_MAX_VALUES, ARDJACK_MAX_VALUE_LENGTH);

	char firstChar = fields.Get(0)[0];
	char field0[ARDJACK_MAX_VALUE_LENGTH];
	strcpy(field0, &fields.Get(0)[1]);
	_strlwr(field0);
	int nextFieldIndex = 1;

	switch (firstChar)
	{
	case ':':
		oper = ARDJACK_OPERATION_CONFIGURE;
		break;

	case '^':
		oper = ARDJACK_OPERATION_CONFIGUREPART;
		break;

	case '~':
		oper = ARDJACK_OPERATION_GET_INVENTORY;
		break;

	case '?':
		oper = ARDJACK_OPERATION_READ;
		break;

	case '>':
		oper = ARDJACK_OPERATION_SUBSCRIBE;
		break;

	case '<':
		oper = ARDJACK_OPERATION_UNSUBSCRIBE;
		break;

	case '!':
		oper = ARDJACK_OPERATION_WRITE;
		break;

	default:
		strcpy(field0, fields.Get(0));
		oper = Device::LookupOperation(field0);

		if (oper == ARDJACK_OPERATION_NONE)
		{
			Log::LogWarningF(PRM("DecodeRequest: Can't decode: '%s' - invalid operation"), useLine);
			return oper;
		}

		field0[0] = NULL;

		if (fieldCount >= 2)
			strcpy(field0, fields.Get(1));

		nextFieldIndex = 2;
	}

	// Is there a dot?
	char *dot = strchr(field0, '.');

	if (NULL == dot)
	{
		// There's no dot, e.g.
		//	?ai0
		//	read ai0
		//	!di0 1
		//	write di0 1
		//	configure x=y xx=yy
		//	getcount di

		switch (oper)
		{
		case ARDJACK_OPERATION_CONFIGURE:
			strcpy(aName, "");

			// Get any arguments.
			for (int i = 1; i < fieldCount; i++)
				values->Add(fields.Get(i));
			break;

		default:
			strcpy(aName, field0);

			// Get any arguments.
			for (int i = nextFieldIndex; i < fieldCount; i++)
				values->Add(fields.Get(i));
			break;
		}

		return oper;
	}

	// There's a dot, e.g.
	//	?ai.count
	int charCount = dot - field0;
	strncpy(aName, field0, charCount);
	aName[charCount] = NULL;
	Utils::Trim(aName);

	char *afterDot = dot + 1;
	Utils::Trim(afterDot);

	if (Utils::StringEquals(afterDot, "count"))
		return ARDJACK_OPERATION_GET_COUNT;

	Log::LogWarningF(PRM("DecodeRequest: Can't decode: '%s' - invalid 'dot' option"), useLine);

	return ARDJACK_OPERATION_NONE;
}


bool DeviceCodec1::EncodeResponse(char *response, int oper, const char* aName, const char* text, StringList* values)
{
	// 'aName' is a Part name, Part Type name or empty (depending on 'oper').
	response[0] = NULL;

	switch (oper)
	{
		case ARDJACK_OPERATION_ACTIVATE:
		case ARDJACK_OPERATION_CHANGED:
		case ARDJACK_OPERATION_DEACTIVATE:
		case ARDJACK_OPERATION_GET_COUNT:
		case ARDJACK_OPERATION_GET_INFO:
		case ARDJACK_OPERATION_READ:
			if (((NULL == values) && (strlen(text) == 0)) || ((NULL != values) && (values->Count == 0)))
			{
				Log::LogErrorF(PRM("EncodeResponse: No response value(s) for operation %d, name '%s'"), oper, aName);
				return false;
			}
	}

	switch (oper)
	{
		case ARDJACK_OPERATION_ACTIVATE:
		case ARDJACK_OPERATION_DEACTIVATE:
			strcat(response, "active ");
			strcat(response, (NULL == values) ? text : values->Get(0));
			break;

		case ARDJACK_OPERATION_CHANGED:
			strcat(response, aName);
			strcat(response, ".change ");
			strcat(response, (NULL == values) ? text : values->Get(0));
			break;

		case ARDJACK_OPERATION_GET_COUNT:
			strcat(response, aName);
			strcat(response, ".count ");
			strcat(response, (NULL == values) ? text : values->Get(0));
			break;

		case ARDJACK_OPERATION_GET_INFO:
			strcat(response, "info ");
			strcat(response, (NULL == values) ? text : values->Get(0));
			break;

		case ARDJACK_OPERATION_READ:
			strcat(response, aName);

			if (NULL != values)
			{
				for (int i = 0; i < values->Count; i++)
				{
					strcat(response, " ");
					strcat(response, values->Get(i));
				}
			}
			else
			{
				strcat(response, " ");
				strcat(response, text);
			}
			break;

		case ARDJACK_OPERATION_SUBSCRIBED:
			strcat(response, aName);
			strcat(response, ".subscribed");
			break;

		case ARDJACK_OPERATION_UNSUBSCRIBED:
			strcat(response, aName);
			strcat(response, ".unsubscribed");
			break;

		default:
			Log::LogErrorF(PRM("EncodeResponse: Can't encode response for operation %d, name '%s'"), oper, aName);
			return false;
	}

	return true;
}

