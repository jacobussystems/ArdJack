/*
	IniFiler.h

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

#include "Globals.h"
#include "IoTObject.h"
#include "Log.h"
#include "Utils.h"

const static int INIFILER_SECTION_NONE = 0;
const static int INIFILER_SECTION_DESCRIPTION = 1;
const static int INIFILER_SECTION_GENERAL = 2;
const static int INIFILER_SECTION_OBJECT = 3;



class IniFiler
{
protected:
	IoTObject* _CurObject;
	uint8_t _CurSectionType;										// enumeration: INIFILER_SECTION_NONE etc.

	virtual bool ReadLine_Description(const char* line);
	virtual bool ReadLine_General(const char* line);
	virtual bool ReadLine_Object(const char* line);
	virtual bool ReadLine_SectionStart(const char* line);

public:
	char CommentPrefix[8];

	IniFiler();

	virtual bool ReadLine(const char* line);					// caller provides the next line to read in 'line'
};

