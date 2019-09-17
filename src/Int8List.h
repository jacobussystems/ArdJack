/*
	Int8List.h

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

#include "Globals.h"

class Dynamic;



class Int8List
{
protected:
	int8_t* _Items;

public:
	bool AutoExpand;
	uint8_t Count;
	uint8_t Size;

	Int8List(int size = 10, bool autoExpand = true);
	~Int8List();

	virtual bool Add(int8_t value);
	virtual bool Add(Dynamic* value);
	virtual void Clear();
	virtual int8_t Get(int index, bool quiet = false);
	virtual int IndexOf(int8_t value, bool ignoreCase = true);
	virtual void LogIt();
	virtual bool Put(int index, int8_t value);
	virtual bool Remove(int index);
	virtual bool SetSize(int size);
};

