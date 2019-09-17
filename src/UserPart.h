/*
	UserPart.h

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

#include "pch.h"

#ifdef ARDUINO
	#define _strlwr strlwr
#else
	#include "stdafx.h"
	#include <typeinfo>
#endif

#include "Globals.h"
#include "Log.h"
#include "Part.h"
#include "Utils.h"



// A UserPart can optionally:
//		Read/Write values for its owner Device.

class UserPart : public Part
{
protected:
	bool _IsAnalog;
	bool _IsDigital;
	bool _IsInput;
	bool _IsOutput;

public:
	UserPart();

	virtual bool AddConfig() override;
	virtual bool IsAnalog() override;
	virtual bool IsDigital() override;
	virtual bool IsInput() override;
	virtual bool IsOutput() override;
};
