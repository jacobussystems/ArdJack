/*
	WinClock.h

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


// Windows only.

#ifdef ARDUINO
#else

#include "DateTime.h"
#include "IoTClock.h"


class WinClock : public IoTClock
{
protected:

public:
	WinClock();
	~WinClock();

	virtual bool Now(DateTime* dt) override;
	virtual long NowMs() override;
	virtual bool NowUtc(DateTime* dt) override;
	virtual bool SetDate(int day, int month, int year, bool utc = false) override;
	virtual bool SetDateTime(DateTime* dt, bool utc = false) override;
	virtual bool SetTime(int hours, int minutes, int seconds, bool utc = false) override;
	virtual bool Start(int day, int month, int year, int hours, int minutes, int seconds, bool utc = false) override;
	virtual bool Stop() override;
};


#endif
