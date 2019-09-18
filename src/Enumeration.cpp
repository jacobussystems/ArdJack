/*
	Enumeration.cpp

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

#include "Enumeration.h"
#include "Globals.h"
#include "Int8List.h"
#include "Log.h"
#include "StringList.h"
#include "Utils.h"



Enumeration::Enumeration(const char* name)
{
	strcpy(Name, name);
}


Enumeration::~Enumeration()
{
}


bool Enumeration::Add(const char* name, int8_t value)
{
	// N.B. 'name' must be stored in code or on the heap, NOT on the stack.
	// It should be sufficient to pass a 'const char*' for 'name' or to use the Arduino 'PRM' macro (a.k.a. 'F' macro).
	// The Unit Tests check that this approach works OK.

	// Store abbreviations before or after the 'full' name, depending on the required effect.
	// E.g. for "AI" to take precedence over "AnalogInput" in 'LookupValue', use:
	//		PartTypes->Add("AI", ARDJACK_PART_TYPE_ANALOG_INPUT);
	//		PartTypes->Add("AnalogInput", ARDJACK_PART_TYPE_ANALOG_INPUT);
	// 'LookupValue' will return the first match, i.e. "AI".

	Names.Add(name);
	Values.Add(value);

	return true;
}


void Enumeration::Clear()
{
	Names.Clear();
	Values.Clear();
}


int Enumeration::Count()
{
	return Values.Count;
}


int8_t Enumeration::LookupName(const char* name, int8_t defaultValue, bool quiet)
{
	for (int i = 0; i < Values.Count; i++)
	{
		if (Utils::StringEquals(Names.Get(i), name))
			return Values.Get(i);
	}

	if (!quiet)
		Log::LogError(PRM("No such name in Enumeration '"), Name, "': '", name, "'");

	return defaultValue;
}


const char* Enumeration::LookupValue(int8_t value, const char* defaultName, bool quiet)
{
	// Returns the first matching item.
	for (int i = 0; i < Values.Count; i++)
	{
		int8_t val = Values.Get(i);

		if (val == value)
			return Names.Get(i);
	}

	if (!quiet)
		Log::LogErrorF(PRM("No such value in Enumeration '%s': %d"), Name, value);

	return defaultName;
}

