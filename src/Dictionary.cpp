/*
	Dictionary.cpp

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

#include "Dictionary.h"
#include "Log.h"
#include "StringList.h"
#include "Utils.h"



// Simple implementation of a Dictionary<string, string>.

Dictionary::Dictionary(bool ignoreCase)
{
	IgnoreCase = ignoreCase;
}


Dictionary::~Dictionary()
{
}


bool Dictionary::Add(const char* key, const char* value)
{
	// Add this key/value.
	int index = Lookup(key);

	if (index >= 0)
	{
		// This key is already present.
		Values.Put(index, value);
	}
	else
	{
		// Add this key.
		Keys.Add(key);
		Values.Add(value);
	}

	return true;
}


bool Dictionary::ContainsKey(const char* key)
{
	return (Lookup(key) >= 0);
}


int Dictionary::Count()
{
	return Keys.Count;
}


const char* Dictionary::Get(const char* key)
{
	int index = Lookup(key);

	if (index < 0)
		return NULL;

	return Values.Get(index);
}


const char* Dictionary::Get(int index, char* key)
{
	if ((index < 0) || (index > Keys.Count))
	{
		Log::LogErrorF(PRM("Get: Invalid index: %d (count %d)"), index, Keys.Count);
		return NULL;
	}

	strcpy(key, Keys.Get(index));

	return Values.Get(index);
}


int Dictionary::Lookup(const char* key)
{
	for (int i = 0; i < Keys.Count; i++)
	{
		if (Utils::StringEquals(key, Keys.Get(i), IgnoreCase))
			return i;
	}

	return -1;
}


bool Dictionary::Remove(const char* key, bool quiet)
{
	int index = Lookup(key);

	if (index < 0)
	{
		if (!quiet)
			Log::LogWarningF(PRM("Remove: No such key: '%s'"), key);
		return true;
	}

	Keys.Remove(index);
	Values.Remove(index);

	return true;
}

