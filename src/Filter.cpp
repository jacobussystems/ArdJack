/*
	Filter.cpp

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
#else
	#include "stdafx.h"
#endif

#include "Filter.h"
#include "Log.h"
#include "Utils.h"



// This is a signal filter, used to debounce digital inputs, or slow the data rate ('throttle') for analog inputs.

// Not to be confused with a Message Filter used in routing - see MessageFilter.h / MessageFilter.cpp.

Filter::Filter(const char* name)
	: IoTObject(name)
{
	Config = new Configuration();

	_MaxInterval = 1000;
	_MinDiff = 2.5;
	_MinInterval = 100;
}


Filter::~Filter()
{
	delete Config;
}


bool Filter::Activate()
{
	if (Globals::Verbosity > 3)
		Log::LogInfo(PRM("Filter::Activate: '"), Name, "'");

	if (!ValidateConfig())
		return false;

	if (!ApplyConfig())
		return false;

	return true;
}


bool Filter::AddConfig()
{
	if (!IoTObject::AddConfig())
		return false;

	if (Globals::Verbosity > 3)
		Log::LogInfo(PRM("Filter::AddConfig: "), Name);

	Config->AddRealProp("MaxInterval", "Maximum interval (from last notification time).", _MaxInterval, "ms");
	Config->AddRealProp("MinDiff", "Minimum difference (from last notified state).", _MinDiff);
	Config->AddRealProp("MinInterval", "Minimum interval (debounce).", _MinInterval, "ms");

	return Config->SortItems();
}


bool Filter::ApplyConfig(bool quiet)
{
	Config->GetAsInteger("MaxInterval", &_MaxInterval);
	Config->GetAsReal("MinDiff", &_MinDiff);
	Config->GetAsInteger("MinInterval", &_MinInterval);

	return true;
}


bool Filter::Evaluate(double value, double lastValue, long lastTimeMs)
{
	// Returns true if 'state' passes this filter, i.e. it's a significant change and should be notified.
	double absDiff = abs(value - lastValue);

	if ((_MaxInterval > 0) && (absDiff != 0))
	{
		if (Utils::NowMs() >= lastTimeMs + _MaxInterval)
			return true;
	}

	if (_MinDiff == 0)
		return true;

	return (absDiff >= _MinDiff);
}


bool Filter::Evaluate(bool state, bool lastNotifiedState, long lastNotifiedMs, bool lastChangeState, long lastChangeMs)
{
	// Returns true if 'state' passes this filter, i.e. it's a change that should be notified.

	// Any changes?
	if ((state == lastNotifiedState) && (state == lastChangeState))
		return false;

	//Log::LogInfoF(PRM("Filter::Evaluate: state %d, lastNotifiedState %d, lastChangeState %d, lastChangeMs %d, lastNotifiedMs %d"),
	//	state, lastNotifiedState, lastChangeState, lastChangeMs, lastNotifiedMs);

	// Are we within a debounce period?
	int nowMs = Utils::NowMs();

	if ((state != lastChangeState) && (nowMs > lastChangeMs + _MinInterval))
	{
		// Yes.
		//Log::LogInfoF(PRM("Filter::Evaluate: Debounce start, state %d"), state);
		return false;
	}

	if ((state == lastChangeState) && (nowMs < lastChangeMs + _MinInterval))
	{
		// Yes.
		if (nowMs < lastChangeMs + 50)
			//Log::LogInfoF(PRM("Filter::Evaluate: Debounce inside, state %d"), state);
		return false;
	}

	if (state == lastNotifiedState)
		return false;

	//Log::LogInfoF(PRM("Filter::Evaluate: PASS, state %d, lastNotifiedState %d, lastChangeState %d, lastChangeMs %d, lastNotifiedMs %d"),
	//	state, lastNotifiedState, lastChangeState, lastChangeMs, lastNotifiedMs);

	return true;
}

