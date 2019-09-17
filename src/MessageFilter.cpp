/*
	MessageFilter.cpp

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

#include "IoTMessage.h"
#include "Log.h"
#include "MessageFilter.h"
#include "Utils.h"




MessageFilter::MessageFilter()
{
}


bool MessageFilter::CleanMessage(IoTMessage* msg, MessageFilterItem* filterItem)
{
	// Clean 'msg.Text' by removing whatever made it pass 'filterItem'.
	char temp[ARDJACK_MAX_MESSAGE_TEXT_LENGTH];
	Utils::StringReplace(msg->Text(), filterItem->Text, "", filterItem->IgnoreCase, temp, 202);
	Utils::Trim(temp);

	msg->SetText(temp);

	return true;
}


bool MessageFilter::EvaluateMessage(IoTMessage* msg)
{
	// Does 'msg' pass this filter?
	switch (msg->Format)
	{
	case ARDJACK_MESSAGE_FORMAT_0:
		if (TextFilter.Active())
		{
			if (!TextFilter.Evaluate(msg->Text()))
				return false;

			// 'msg' passes this filter.

			// Clean 'msg.Text' by removing whatever made it pass the Prefix filter (i.e. the prefix).
			CleanMessage(msg, &TextFilter);
		}

		break;

	default:
		if (FromFilter.Active() && !FromFilter.Evaluate(msg->FromPath()))
			return false;

		if (ReturnFilter.Active() && !ReturnFilter.Evaluate(msg->ReturnPath()))
			return false;

		if (BodyFilter.Active() && !BodyFilter.Evaluate(msg->Text()))
			return false;

		if (ToFilter.Active() && !ToFilter.Evaluate(msg->ToPath()))
			return false;

		if (TypeFilter.Active() && !TypeFilter.Evaluate(msg->Type()))
			return false;

		break;
	}

	return true;
}


MessageFilterItem* MessageFilter::GetFilterItem(const char* name, bool quiet)
{
	if (Utils::StringEquals(name, PRM("body")))
		return &BodyFilter;

	if (Utils::StringEquals(name, PRM("from")))
		return &FromFilter;

	if (Utils::StringEquals(name, PRM("return")))
		return &ReturnFilter;

	if (Utils::StringEquals(name, PRM("text")))
		return &TextFilter;

	if (Utils::StringEquals(name, PRM("to")))
		return &ToFilter;

	if (Utils::StringEquals(name, PRM("type")))
		return &TypeFilter;

	return NULL;
}

