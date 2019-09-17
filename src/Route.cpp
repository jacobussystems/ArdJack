/*
	Route.cpp

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

#include "Log.h"
#include "Route.h"
#include "Utils.h"



Route::Route(const char* name, int type)
{
	AlwaysUse = false;
	Buffer = NULL;
	Callback = NULL;
	CallbackObj = NULL;
	strcpy(Name, name);
	StopIfHandled = true;
	Target = NULL;
	Type = type;
}


char* Route::GetTypeName(int type, char* out)
{
	switch (type)
	{
	case ARDJACK_ROUTE_TYPE_COMMAND:
		strcpy(out, "Command");
		break;

	case ARDJACK_ROUTE_TYPE_REQUEST:
		strcpy(out, "Request");
		break;

	case ARDJACK_ROUTE_TYPE_RESPONSE:
		strcpy(out, "Response");
		break;

	default:
		strcpy(out, "None");
		break;
	}

	return out;
}


bool Route::Handle(IoTMessage* msg)
{
	if (NULL != Buffer)
	{
		CommandBufferItem item;
		item.Dev = Target;
		strcpy(item.Text, msg->Text());
		Buffer->Push(&item);
	}

	if (NULL != Callback)
		Callback(CallbackObj, this, msg);

	return true;
}


void Route::SetTextFilter(const char* text, int operation)
{
	// Set the Text filter (for Format 0 messages).
	MessageFilterItem* textFilter = &Filter.TextFilter;
	textFilter->Operation = ARDJACK_STRING_COMPARE_STARTS_WITH;
	strcpy(textFilter->Text, text);
}

