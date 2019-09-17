/*
	RtcClock.cpp

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

#include "DetectBoard.h"

#ifdef ARDJACK_RTC_AVAILABLE

#include "DateTime.h"
#include "IoTClock.h"
#include "Log.h"
#include "RtcClock.h"
#include "Utils.h"



RtcClock::RtcClock()
	: IoTClock()
{
	_RolloverMillis = 0;
}


RtcClock::~RtcClock()
{
}


void RtcClock::AdjustMilliseconds()
{
	// Wait for the next seconds rollover.
	Log::LogInfo(PRM("RtcClock::AdjustMilliseconds: Entry"));

	int seconds = _RTClock.getSeconds();

	while (_RTClock.getSeconds() == seconds)
		;

	// A seconds rollover.
	_RolloverMillis = millis();

	_RolloverTime.Day = _RTClock.getDay();
	_RolloverTime.Month = _RTClock.getMonth();
	_RolloverTime.Year = _RTClock.getYear();
	_RolloverTime.Hours = _RTClock.getHours();
	_RolloverTime.Minutes = _RTClock.getMinutes();
	_RolloverTime.Seconds = _RTClock.getSeconds();
	_RolloverTime.Milliseconds = 0;

	Log::LogInfo(PRM("RtcClock::AdjustMilliseconds: Exit"));
}


void RtcClock::CheckRollover(DateTime* dt)
{
	// Hours rollover?
	//if (dt->Hours != _RolloverTime.Hours)
	if (dt->Minutes != _RolloverTime.Minutes)
		Rollover(dt);
}


bool RtcClock::GetDate(int *day, int *month, int *year, bool utc)
{
	DateTime dt;
	GetDateTime(&dt, utc);

	*day = dt.Day;
	*month = dt.Month;
	*year = dt.Year;

	return true;
}


bool RtcClock::GetDateTime(DateTime* dt, bool utc)
{
	dt->Day = _RTClock.getDay();
	dt->Month = _RTClock.getMonth();
	dt->Year = _RTClock.getYear();
	dt->Hours = _RTClock.getHours();
	dt->Minutes = _RTClock.getMinutes();
	dt->Seconds = _RTClock.getSeconds();
	dt->Milliseconds = 0;

	//CheckRollover(dt);

	DateTime work;
	work.Copy(&_RolloverTime);
	int interval = millis() - _RolloverMillis;
	//Utils::AddTime_Ms(&work, interval);

	//dt->Milliseconds = work.Milliseconds;

#ifdef ARDJACK_USE_RTCZERO
	// Workaround for the 2-digit year problem with the 'RTCZero' library.
	if (dt->Year < 1000)
		dt->Year += 2000;
#endif

	// ...and check for Summertime in the United States (US) or the European Economic Community (EEC)
	//if (_RTClock.isSummertime(EEC))

	return true;
}


bool RtcClock::GetTime(int *hours, int *minutes, int *seconds, bool utc)
{
	DateTime dt;
	GetDateTime(&dt, utc);

	*hours = dt.Hours;
	*minutes = dt.Minutes;
	*seconds = dt.Seconds;

	return true;
}


void RtcClock::Rollover(DateTime* dt)
{
	Log::LogInfo(PRM("RtcClock::Rollover"));

	//AdjustMilliseconds();
}


bool RtcClock::SetDate(int day, int Month, int year, bool utc)
{
	// Workaround for 2-digit Year problem.
#ifdef ARDJACK_USE_RTCZERO
	if (year >= 1000)
		year -= 2000;
#endif

	_RTClock.setDate(day, Month, year);

	return true;
}


bool RtcClock::SetDateTime(DateTime* dt, bool utc)
{
	_RTClock.setTime(dt->Hours, dt->Minutes, dt->Seconds);
	_RTClock.setDate(dt->Day, dt->Month, dt->Year);

	return false;
}


bool RtcClock::SetTime(int hours, int minutes, int seconds, bool utc)
{
	_RTClock.setTime(hours, minutes, seconds);

	return true;
}


bool RtcClock::Start(int day, int month, int year, int hours, int minutes, int seconds, bool utc)
{
	_RTClock.begin();

	SetTime(hours, minutes, seconds);
	SetDate(day, month, year);

	AdjustMilliseconds();

	char temp[100];
	sprintf(temp, PRM("%d/%d/%d %02d:%02d:%02d, millis %d"), day, month, year, hours, minutes, seconds, millis());
	Log::LogInfo(PRM("RtcClock::Start: "), temp);

	return true;
}


bool RtcClock::Stop()
{
	//_RTClock.

	return true;
}

#endif
#endif
