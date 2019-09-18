/*
	WinClock.cpp

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


// Windows only.

#ifdef ARDUINO
#else

#include <time.h>

#include "DateTime.h"
#include "IoTClock.h"
#include "Log.h"
#include "WinClock.h"
#include "Utils.h"



WinClock::WinClock() : IoTClock()
{
}


WinClock::~WinClock()
{
}


bool WinClock::Now(DateTime* dt)
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	dt->Day = (uint8_t)st.wDay;
	dt->Month = (uint8_t)st.wMonth;
	dt->Year = (uint16_t)st.wYear;
	dt->Hours = (uint8_t)st.wHour;
	dt->Minutes = (uint8_t)st.wMinute;
	dt->Seconds = (uint8_t)st.wSecond;
	dt->Milliseconds = (uint16_t)st.wMilliseconds;

	return true;
}


long WinClock::NowMs()
{
	// From: https://stackoverflow.com/questions/34831145/using-gettimeofday-equivalents-on-windows
	long result;

	// Note: some broken versions only have 8 trailing zeros, the correct epoch has 9 trailing zeros.
	static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

	SYSTEMTIME  system_time;
	FILETIME    file_time;
	uint64_t    time;

	GetSystemTime(&system_time);

	SystemTimeToFileTime(&system_time, &file_time);
	time = ((uint64_t)file_time.dwLowDateTime);
	time += ((uint64_t)file_time.dwHighDateTime) << 32;

	result = (long)((time - EPOCH) / 10000L);

	return result;
}


bool WinClock::NowUtc(DateTime* dt)
{
	SYSTEMTIME st;
	GetSystemTime(&st);

	dt->Day = (uint8_t)st.wDay;
	dt->Month = (uint8_t)st.wMonth;
	dt->Year = (uint16_t)st.wYear;
	dt->Hours = (uint8_t)st.wHour;
	dt->Minutes = (uint8_t)st.wMinute;
	dt->Seconds = (uint8_t)st.wSecond;
	dt->Milliseconds = (uint16_t)st.wMilliseconds;

	return true;
}


bool WinClock::SetDate(int day, int Month, int year, bool utc)
{
	// Ignored for now.
	return true;
}


bool WinClock::SetDateTime(DateTime* dt, bool utc)
{
	// Ignored for now.
	return true;
}


bool WinClock::SetTime(int hours, int minutes, int seconds, bool utc)
{
	// Ignored for now.
	return true;
}


bool WinClock::Start(int day, int month, int year, int hours, int minutes, int seconds, bool utc)
{
	Log::LogInfo("WinClock started");

	return true;
}


bool WinClock::Stop()
{

	return true;
}

#endif
