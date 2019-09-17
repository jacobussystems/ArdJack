/*
	IoTClock.cpp

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


#include "DateTime.h"
#include "IoTClock.h"
#include "Log.h"
#include "Utils.h"


IoTClock::IoTClock()
{
}


IoTClock::~IoTClock()
{
}


bool IoTClock::GetDate(int* day, int* month, int* year, bool utc)
{
	// Returns the date in 'day', 'month' and 'year', in human form (1-31 / 1-12 / human).
	DateTime now;

	if (utc)
		NowUtc(&now);
	else
		Now(&now);

	*day = now.Day;
	*month = now.Month;
	*year = now.Year;

	return true;
}


bool IoTClock::GetTime(int* hours, int* minutes, int* seconds, bool utc)
{
	// Returns the time in 'hours', 'minutes' and 'seconds', in human form (0-23 : 0-59 : 0-59).
	int milliseconds;

	return GetTime(hours, minutes, seconds, &milliseconds, utc);
}


bool IoTClock::GetTime(int* hours, int* minutes, int* seconds, int* milliseconds, bool utc)
{
	// Returns the time in 'hours', 'minutes' and 'seconds', in human form (0-23 : 0-59 : 0-59).
	DateTime now;

	if (utc)
		NowUtc(&now);
	else
		Now(&now);

	*hours = now.Hours;
	*minutes = now.Minutes;
	*seconds = now.Seconds;
	*milliseconds = now.Milliseconds;

	return true;
}


bool IoTClock::Now(DateTime* dt)
{
	return false;
}


long IoTClock::NowMs()
{
	return 0;
}


bool IoTClock::NowUtc(DateTime* dt)
{
	return false;
}


void IoTClock::Poll()
{

}


bool IoTClock::SetDate(int day, int month, int year, bool utc)
{
	return false;
}


bool IoTClock::SetDateTime(DateTime* dt, bool utc)
{
	return false;
}


bool IoTClock::SetTime(int hours, int minutes, int seconds, bool utc)
{
	return false;
}


bool IoTClock::Start(int day, int month, int year, int hours, int minutes, int Seconds, bool utc)
{
	return false;
}


bool IoTClock::Stop()
{
	return true;
}

