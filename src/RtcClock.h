/*
	RtcClock.h

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

// Arduino only.

#ifdef ARDUINO

#include <arduino.h>
#include "DetectBoard.h"

#ifdef ARDJACK_RTC_AVAILABLE

#include "DateTime.h"
#include "IoTClock.h"

#ifdef ARDJACK_ARDUINO_DUE
	#include <RTCDue.h>
#endif

#ifdef ARDJACK_ARDUINO_MKR
	#include <RTCZero.h>
#endif

#ifdef ARDJACK_FEATHER_M0
	#include <RTCZero.h>
#endif


class RtcClock : public IoTClock
{
 protected:
	 unsigned long _RolloverMillis;									// millis() when 'seconds roll-over' was last detected
	 DateTime _RolloverTime;										// time when 'seconds roll-over' was last detected

	#ifdef ARDJACK_USE_RTCZERO
		RTCZero _RTClock;
	#else
		RTCDue _RTClock = RTCDue(XTAL);
	#endif

	virtual void AdjustMilliseconds();
	virtual void CheckRollover(DateTime* dt);
	virtual void Rollover(DateTime* dt);

public:
	RtcClock();
	 ~RtcClock();

	virtual bool GetDate(int *day, int *month, int *year, bool utc = false) override;
	virtual bool Now(DateTime* dt, bool utc = false) override;
	virtual bool GetTime(int *hours, int *minutes, int *seconds, bool utc = false) override;
	virtual bool SetDate(int day, int month, int year, bool utc = false) override;
	virtual bool SetDateTime(DateTime* dt, bool utc = false) override;
	virtual bool SetTime(int hours, int minutes, int seconds, bool utc = false) override;
	virtual bool Start(int day, int month, int year, int hours, int minutes, int seconds, bool utc = false) override;
	virtual bool Stop() override;
};


#endif

#endif
