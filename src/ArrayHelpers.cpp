/*
	ArrayHelpers.cpp

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
	#define _strlwr strlwr
#else
	#include "stdafx.h"
	#include <malloc.h>
	#include <typeinfo>
#endif

#include "ArrayHelpers.h"
#include "Log.h"
#include "Utils.h"



#ifdef ARDUINO
#else

long ArrayHelpers::HeapUsed()
{
	// Returns used heap size in bytes or -1 if heap is corrupted.
	_HEAPINFO info = { 0, 0, 0 };
	long used = 0;
	int rc;

	while ((rc = _heapwalk(&info)) == _HEAPOK)
	{
		if (info._useflag == _USEDENTRY)
			used += (long)info._size;
	}

	if ((rc != _HEAPEND) && (rc != _HEAPEMPTY))
		used = (used ? -used : -1);

	return used;
}

#endif



bool ArrayHelpers::RemoveElement(void** array, int count, int index)
{
	// Remove element 'index' of an array of pointers.
	if ((index < 0) || (index > count - 1))
	{
		char temp[202];
		sprintf(temp, PRM("ArrayHelpers::RemoveElement: Invalid index (%d, count %d)"), index, count);
		Log::LogError(temp);
		return false;
	}

	for (int i = index; i <= count - 2; i++)
		array[i] = array[i + 1];

	array[count - 1] = NULL;

	return true;
}

