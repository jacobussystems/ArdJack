/*
	StringList.cpp

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
#include "Log.h"
#include "StringList.h"
#include "Utils.h"




StringList::StringList(int size, bool autoExpand)
{
	if (Globals::Verbosity > 7)
		Log::LogInfoF(PRM("StringList ctor: %p, size %d bytes, autoExpand %d"), this, size, autoExpand);

	_Buffer = NULL;
	_Length = 0;

	AutoExpand = autoExpand;
	Count = 0;
	Size = 0;

	SetBuffer(size);
}


StringList::~StringList()
{
	if (Globals::Verbosity > 7)
		Log::LogInfoF(PRM("StringList::~: %p, Size %d, Count %d"), this, Size, Count);

	Clear();
}


bool StringList::Add(const char* text)
{
	if (Globals::Verbosity > 9)
		Log::LogInfoF(PRM("StringList::Add: %p, [%d] = '%s'"), this, Count, text);

	int newLength = _Length + strlen(text) + 1;

	if (newLength > Size)
	{
		bool expanded = false;

		if (AutoExpand)
			expanded = SetBuffer((int)(1.5 * newLength));

		if (!expanded)
		{
			Log::LogErrorF(PRM("StringList::Add: %p, List is full (buffer size = %d)"), this, Size);
			return false;
		}
	}

	strcpy(_Buffer + _Length, text);
	_Length += strlen(text) + 1;

	Count++;

	return true;
}


bool StringList::Add(Dynamic* value)
{
	char temp[ARDJACK_MAX_DYNAMIC_STRING_LENGTH];
	value->AsString(temp);

	return Add(temp);
}


void StringList::Clear(bool full)
{
	if (Globals::Verbosity > 6)
	{
		Log::LogInfoF(PRM("StringList::Clear: %p, full %d, Size %d, Count %d, _Length %d"),
			this, full, Size, Count, _Length);
	}

	if (full)
	{
		if (NULL != _Buffer)
		{
			Utils::MemFree(_Buffer);
			_Buffer = NULL;
		}

		Size = 0;
	}

	Count = 0;
	_Length = 0;
}


const char* StringList::Get(int index, bool quiet)
{
	if ((index < 0) || (index >= Count))
	{
		if (!quiet)
			Log::LogErrorF(PRM("StringList::Get: %p, Bad index: %d (buffer %d, Count %d)"), this, index, Size, Count);
		return Globals::EmptyString;
	}

	char* ptr = _Buffer;

	for (int i = 0; i < index; i++)
		ptr += strlen(ptr) + 1;

	if (Globals::Verbosity > 9)
		Log::LogInfoF(PRM("StringList::Get: %p, [%d] = '%s'"), this, index, ptr);

	return ptr;
}


int StringList::IndexOf(const char* text, bool ignoreCase)
{
	for (int i = 0; i < Count; i++)
	{
		const char* ptr = Get(i);

		if (Utils::StringEquals(ptr, text, ignoreCase))
			return i;
	}

	return -1;
}


void StringList::LogIt()
{
	Log::LogInfoF(PRM("StringList: %p, size %d, count %d, autoExpand %d"), this, Size, Count, AutoExpand);

	for (int i = 0; i < Count; i++)
	{
		const char* ptr = Get(i);
		Log::LogInfoF(PRM("   [%d] = '%s'"), i, ptr);
	}
}


bool StringList::Put(int index, const char* text)
{
	if (index >= Count)
	{
		for (int i = Count; i < index; i++)
			Add("");

		return Add(text);
	}

	// Replace item 'index'.
	const char* oldText = Get(index);

	// Any change?
	if (Utils::StringEquals(text, oldText, false))
		return true;

	// Yes, there's a change.
	if (Globals::Verbosity > 9)
		Log::LogInfoF(PRM("StringList::Put: %p, [%d] '%s' -> '%s'"), this, index, oldText, text);

	int itemIncrease = strlen(text) - strlen(oldText);
	int newSize;

	bool usingNewBuffer = false;
	char* newBuffer = NULL;

	if (itemIncrease > 0)
	{
		newSize = _Length + itemIncrease;

		if (newSize > Size)
		{
			usingNewBuffer = true;
			newSize = (int)(1.5 * (Size + itemIncrease));

			if (Globals::Verbosity > 7)
				Log::LogInfoF(PRM("StringList::Put: %p, index %d, RESIZING from %d to %d bytes"), this, index, Size, newSize);

			newBuffer = (char*)Utils::MemMalloc(newSize);
		}
	}

	if (usingNewBuffer)
		return Put_NewBuffer(index, text, itemIncrease, newBuffer, newSize);
	
	return Put_ExistingBuffer(index, text, itemIncrease);
}


bool StringList::Put_ExistingBuffer(int index, const char* text, int itemIncrease)
{
	const char* ptr = Get(index);

	// Any items after 'index'?
	if (index < Count - 1)
	{
		const char* ptrNext = Get(index + 1);
		int nMove = _Length - (int)(ptrNext - _Buffer);

		if (itemIncrease > 0)
		{
			// Move up items after 'index' to make room for the new item 'index'.
			memmove((void*)(ptrNext + itemIncrease), ptrNext, nMove);
		}
		else if (itemIncrease < 0)
		{
			// Move down items after 'index'.
			memcpy((void*)(ptr + strlen(text) + 1), ptrNext, nMove);
		}
	}

	// Copy the new item 'index'.
	strcpy((char*)ptr, text);

	_Length += itemIncrease;

	return true;
}


bool StringList::Put_NewBuffer(int index, const char* text, int itemIncrease, char* newBuffer, int newSize)
{
	int newLength = 0;
	const char* ptr = Get(index);

	// Any items before 'index'?
	if (index > 0)
	{
		// Copy items before 'index'.
		int nCopy = (int)(ptr - _Buffer);
		memcpy(newBuffer, _Buffer, nCopy);
		newLength += nCopy;
	}

	// Copy the new item 'index'.
	strcpy(newBuffer + newLength, text);
	newLength += strlen(text) + 1;

	// Any items after 'index'?
	if (index < Count - 1)
	{
		// Copy items after 'index'.
		const char* ptrNext = Get(index + 1);
		int nCopy = _Length - (int)(ptrNext - _Buffer);
		memcpy(newBuffer + newLength, ptrNext, nCopy);
		newLength += nCopy;
	}

	Utils::MemFree(_Buffer);
	_Buffer = newBuffer;
	Size = newSize;
	_Length = newLength;

	return true;
}


bool StringList::Remove(int index)
{
	if ((index < 0) || (index >= Count))
	{
		Log::LogErrorF(PRM("StringList::Remove: %p, Bad index: %d"), this, index);
		return false;
	}

	char* ptr = (char*)Get(index);
	int oldItemLen = strlen(ptr) + 1;

	// Is it the last item?
	if (index < Count - 1)
	{
		// No - move down the remaining item(s).
		const char* ptr2 = Get(index + 1);
		int nMove = _Length - (ptr2 - _Buffer);
		memcpy(ptr, ptr2, nMove);
	}

	_Length -= oldItemLen;
	Count--;

	return true;
}


bool StringList::SetBuffer(int size)
{
	// Try to allocate a new buffer of at least 'size' bytes.
	if (size <= Size)
		return true;

	int useSize = Utils::MaxInt(size, 20);

	if ((Size > 0) && (Globals::Verbosity > 7))
		Log::LogInfoF(PRM("StringList::SetBuffer: %p, RESIZING from %d to %d bytes"), this, Size, useSize);

	char* newBuffer = (char*)Utils::MemMalloc(useSize);

	if (NULL == newBuffer)
	{
		Log::LogErrorF(PRM("StringList::SetBuffer: %p, Failed to resize from %d to %d bytes"), this, Size, useSize);
		return false;
	}

	if (NULL != _Buffer)
	{
		// Copy any items from the old buffer.
		memcpy(newBuffer, _Buffer, Size);
		Utils::MemFree(_Buffer);
		_Buffer = NULL;
	}

	_Buffer = newBuffer;
	Size = useSize;

	return true;
}

