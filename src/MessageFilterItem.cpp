/*
	MessageFilterItem.cpp

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
#include "MessageFilterItem.h"
#include "Utils.h"



MessageFilterItem::MessageFilterItem()
{
	IgnoreCase = true;
	Operation = ARDJACK_STRING_COMPARE_EQUALS;
	Text[0] = NULL;
}


bool MessageFilterItem::Active()
{
	return (Operation != ARDJACK_STRING_COMPARE_FALSE) && (strlen(Text) > 0);
}


bool MessageFilterItem::Evaluate(const char* value)
{
	return Utils::CompareStrings(value, Operation, Text, IgnoreCase);
}


char* MessageFilterItem::ToString(char* out)
{
	char temp[20];

	switch (Operation)
	{
	case ARDJACK_STRING_COMPARE_FALSE:
	case ARDJACK_STRING_COMPARE_TRUE:
		sprintf(out, PRM("%s - will ignore Text ('%s') and IgnoreCase (%s)"),
			Utils::StringComparisonTypeName(Operation, temp), Text, Utils::Bool2yesno(IgnoreCase));
		break;

	default:
		sprintf(out, PRM("%s '%s', IgnoreCase %s"),
			Utils::StringComparisonTypeName(Operation, temp), Text, Utils::Bool2yesno(IgnoreCase));
		break;
	}

	return out;
}

