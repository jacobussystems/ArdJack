/*
	Int8List.cpp

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


#include "Dynamic.h"
#include "Globals.h"
#include "Int8List.h"
#include "Log.h"
#include "Utils.h"




Int8List::Int8List(int size, bool autoExpand)
{
	if (Globals::Verbosity > 7)
		Log::LogInfoF(PRM("Int8List ctor: %p, size %d, autoExpand %d"), this, size, autoExpand);

	_Items = NULL;

	AutoExpand = autoExpand;
	Count = 0;
	Size = 0;

	SetSize(size);
}


Int8List::~Int8List()
{
	if (Globals::Verbosity > 7)
		Log::LogInfoF(PRM("Int8List::~: %p, Size %d, Count %d"), this, Size, Count);

	Clear();

	Utils::MemFree(_Items);
	_Items = NULL;
}


bool Int8List::Add(int8_t value)
{
	if (Count >= Size)
	{
		bool expanded = false;

		if (AutoExpand)
			expanded = SetSize((int)(1.5 * Size));

		if (!expanded)
		{
			Log::LogErrorF(PRM("Int8List::Add: %p, List is full (size = %d)"), this, Size);
			return false;
		}
	}

	_Items[Count] = value;

	if (Globals::Verbosity > 9)
		Log::LogInfoF(PRM("Int8List::Add: %p, [%d] = %d"), this, Count, value);

	Count++;

	return true;
}


bool Int8List::Add(Dynamic* value)
{
	int intVal = value->AsInt();

	return Add(intVal);
}


void Int8List::Clear()
{
	if (Globals::Verbosity > 6)
		Log::LogInfoF(PRM("Int8List::Clear: %p, Size %d, Count %d"), this, Size, Count);

	Count = 0;
}


int8_t Int8List::Get(int index, bool quiet)
{
	if ((index < 0) || (index >= Count))
	{
		if (!quiet)
			Log::LogErrorF(PRM("Int8List::Get: %p, Bad index: %d (Size %d, Count %d)"), this, index, Size, Count);
		return NULL;
	}

	int8_t result = _Items[index];

	if (Globals::Verbosity > 9)
		Log::LogInfoF(PRM("Int8List::Get: %p, [%d] = %d"), this, index, result);

	return result;
}


int Int8List::IndexOf(int8_t value, bool ignoreCase)
{
	for (int i = 0; i < Count; i++)
	{
		if (_Items[i] == value)
			return i;
	}

	return -1;
}


void Int8List::LogIt()
{
	Log::LogInfoF(PRM("Int8List: %p, size %d, count %d, autoExpand %d"), this, Size, Count, AutoExpand);

	for (int i = 0; i < Count; i++)
		Log::LogInfoF(PRM("   [%d] = %d"), i, _Items[i]);
}


bool Int8List::Put(int index, int8_t value)
{
	if (index >= Count)
		return Add(value);

	// Replace item 'index'.
	_Items[index] = value;

	return true;
}


bool Int8List::Remove(int index)
{
	if ((index < 0) || (index >= Count))
	{
		Log::LogErrorF(PRM("Int8List::Remove: %p, Bad index: %d"), this, index);
		return false;
	}

	// Move down any remaining items.
	for (int i = index; i < Count - 1; i++)
		_Items[i] = _Items[i + 1];

	_Items[Count - 1] = 0;
	Count--;

	return true;
}


bool Int8List::SetSize(int size)
{
	// Try to allocate a new 'Items' array for 'size' items.
	if (size <= Size)
		return true;

	if ((Size > 0) && (Globals::Verbosity > 9))
		Log::LogInfoF(PRM("Int8List::SetBuffer: %p, RESIZING from %d to %d items"), this, Size, size);

	int byteCount = size * sizeof(int8_t);

	int8_t* newItems = (int8_t*)Utils::MemMalloc(byteCount);

	if (NULL == newItems)
	{
		Log::LogErrorF(PRM("Int8List::SetBuffer: %p, Failed to resize from %d to %d items"), this, Size, size);
		return false;
	}

	// Initialize the new array.
	for (int i = 0; i < size; i++)
		newItems[i] = 0;

	if (NULL != _Items)
	{
		// Copy any items in the old array.
		for (int i = 0; i < Count; i++)
			newItems[i] = _Items[i];

		Utils::MemFree(_Items);
	}

	_Items = newItems;
	Size = size;

	return true;
}

