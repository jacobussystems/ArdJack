/*
	ArduinoClock.cpp

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


// Arduino only.

#ifdef ARDUINO

#include "ArduinoClock.h"
#include "DateTime.h"
#include "IoTClock.h"
#include "Log.h"
#include "Utils.h"




ArduinoClock::ArduinoClock()
	: IoTClock()
{
	if (Globals::Verbosity > 7)
		Log::LogInfo(PRM("ArduinoClock ctor"));

	_InitialMillis = 0;

	_InitialTime.Year = 2019;
	_InitialTime.Month = 1;
	_InitialTime.Day = 0;
	_InitialTime.Hours = 0;
	_InitialTime.Minutes = 0;
	_InitialTime.Seconds = 0;
	_InitialTime.Milliseconds = 0;
}


ArduinoClock::~ArduinoClock()
{
}


bool ArduinoClock::Now(DateTime* now)
{
	*now = _InitialTime;

	long interval = millis() - _InitialMillis;
	Utils::AddTime_Ms(now, interval);

	return true;
}


long ArduinoClock::NowMs()
{
	return millis();
}


bool ArduinoClock::NowUtc(DateTime* now)
{
	// TEMPORARY:
	return Now(now);
}


bool ArduinoClock::SetDate(int day, int month, int year, bool utc)
{
	// Set the date part of '_InitialTime', where 'day', 'month' and 'year' are in human form (1-31 / 1-12 / human).
	_InitialTime.Day = day;													// DateTime's days are [1-31]
	_InitialTime.Month = month - 1;											// DateTime's months are [0-11]
	_InitialTime.Year = year;												// DateTime's years are human

	return true;
}


bool ArduinoClock::SetDateTime(DateTime* dt, bool utc)
{
	_InitialMillis = millis();
	_InitialTime = *dt;

	return true;
}


bool ArduinoClock::SetTime(int hours, int minutes, int seconds, bool utc)
{
	// Set the time part of '_InitialTime', where 'hours', 'minutes' and 'seconds' are in human form (0-23 : 0-59 : 0-59).
	_InitialMillis = millis();

	_InitialTime.Hours = hours;												// DateTime's hours are [0-23]
	_InitialTime.Minutes = minutes;											// DateTime's minutes are [0-59]
	_InitialTime.Seconds = seconds;											// DateTime's seconds are [0-59]
	_InitialTime.Milliseconds = 0;											// DateTime's milliseconds are [0-999]

	return true;
}


bool ArduinoClock::Start(int day, int month, int year, int hours, int minutes, int seconds, bool utc)
{
	SetTime(hours, minutes, seconds);
	SetDate(day, month, year);

	char temp[100];
	sprintf(temp, PRM("%d/%d/%d %02d::%02d::%02d, millis %d"), day, month, year, hours, minutes, seconds, millis());
	Log::LogInfo(PRM("ArduinoClock::Start: "), temp);

	return true;
}


bool ArduinoClock::Stop()
{
	return true;
}

#endif
