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

	Config->AddRealProp("MaxInterval", "Maximum interval (from last notification).", _MaxInterval, "ms");
	Config->AddRealProp("MinDiff", "Minimum difference (analog, from last notified value).", _MinDiff);
	Config->AddRealProp("MinInterval", "Minimum interval (debounce / data throttle).", _MinInterval, "ms");

	return Config->SortItems();
}


bool Filter::ApplyConfig(bool quiet)
{
	Config->GetAsInteger("MaxInterval", &_MaxInterval);
	Config->GetAsReal("MinDiff", &_MinDiff);
	Config->GetAsInteger("MinInterval", &_MinInterval);

	return true;
}


bool Filter::EvaluateAnalog(double newValue, double lastNotifiedValue, long lastNotifiedMs)
{
	// Returns true if 'newValue' passes this filter, i.e. it's a change that should be notified.

	// Any changes?
	//if ((newValue == lastNotifiedValue) && (newValue == lastChangeValue))
	if (newValue == lastNotifiedValue)
		return false;

	// Are we within the minimum interval?
	if (Utils::NowMs() < lastNotifiedMs + _MinInterval)
	{
		// Yes - ignore any changes.
		return false;
	}

	double absDiff = abs(newValue - lastNotifiedValue);

	// Are we past the maximum interval?
	if ((_MaxInterval > 0) && (absDiff != 0))
	{
		if (Utils::NowMs() >= lastNotifiedMs + _MaxInterval)
		{
			// Yes - notify the change.
			return true;
		}
	}

	// We're between the minimum and maximum intervals - check the difference against 'MinDiff'.
	return (absDiff >= _MinDiff);
}


bool Filter::EvaluateDigital(bool newState, bool lastNotifiedState, long lastNotifiedMs, bool lastChangeState, long lastChangeMs)
{
	// Returns true if 'newState' passes this filter, i.e. it's a change that should be notified.

	// Any changes?
	if ((newState == lastNotifiedState) && (newState == lastChangeState))
		return false;

	//Log::LogInfoF(PRM("Filter::Evaluate: newState %d, lastNotifiedState %d, lastChangeState %d, lastChangeMs %d, lastNotifiedMs %d"),
	//	newState, lastNotifiedState, lastChangeState, lastChangeMs, lastNotifiedMs);

	// Are we within a debounce period ('MinInterval')?
	int nowMs = Utils::NowMs();

	if ((newState != lastChangeState) && (nowMs >= lastChangeMs + _MinInterval))
	{
		// No.
		// Start a new debounce period.
		return false;
	}

	if ((newState == lastChangeState) && (nowMs < lastChangeMs + _MinInterval))
	{
		// Yes, we're inside the debounce period.
		return false;
	}

	// We're not in a debounce period.
	return (newState != lastNotifiedState);
}

