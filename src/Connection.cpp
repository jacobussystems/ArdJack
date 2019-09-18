/*
	Connection.cpp

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

#include "ConfigProp.h"
#include "Connection.h"
#include "ConnectionManager.h"
#include "Device.h"
#include "Globals.h"
#include "IoTMessage.h"
#include "IoTObject.h"
#include "Log.h"
#include "Route.h"
#include "Utils.h"



Connection::Connection(const char* name)
	: IoTObject(name)
{
	_CanInput = true;
	_CanOutput = false;
	_CommandPrefix[0] = NULL;
	_CommentPrefix[0] = NULL;
	_InputAnnounce = false;
	strcpy(_InputEncodingType, "ascii");
	_OutputAnnounce = false;
	strcpy(_OutputEncodingType, "ascii");
	_WarnUnhandled = true;

	DefaultRoute = NULL;
	RouteCount = 0;
	RxCount = 0;
	RxEvents = 0;
	TxCount = 0;
	TxEvents = 0;

	for (int i = 0; i < ARDJACK_MAX_INPUT_ROUTES; i++)
		Routes[i] = NULL;
}


Connection::~Connection()
{
	ClearRoutes();
}


bool Connection::Activate()
{
	if (Globals::Verbosity > 2)
		Log::LogInfo(PRM("Connection::Activate: '"), Name, "'");

	if (!IoTObject::Activate()) return false;

	Config->GetAsBoolean("CanInput", &_CanInput);
	Config->GetAsBoolean("CanOutput", &_CanOutput);
	Config->GetAsString("CommandPrefix", _CommandPrefix);
	Config->GetAsString("CommentPrefix", _CommentPrefix);
	Config->GetAsBoolean("InAnnounce", &_InputAnnounce);
	Config->GetAsBoolean("OutAnnounce", &_OutputAnnounce);
	Config->GetAsBoolean("WarnUnhandled", &_WarnUnhandled);

	char temp[ARDJACK_MAX_NAME_LENGTH];
	Config->GetAsString("DefaultRoute", temp);

	DefaultRoute = LookupRoute(temp);

	return true;
}


bool Connection::AddConfig()
{
	if (!IoTObject::AddConfig())
		return false;

	Config->AddBooleanProp(PRM("CanInput"), PRM("Can input?"), _CanInput);
	Config->AddBooleanProp(PRM("CanOutput"), PRM("Can output?"), _CanOutput);
	Config->AddStringProp(PRM("CommandPrefix"), PRM("Command prefix."), Globals::CommandPrefix);
	Config->AddStringProp(PRM("CommentPrefix"), PRM("Comment prefix."), Globals::CommentPrefix);
	Config->AddStringProp(PRM("DefaultRoute"), PRM("Default route."), "");
	Config->AddBooleanProp(PRM("InAnnounce"), PRM("Announce each input?"), _InputAnnounce);
	Config->AddBooleanProp(PRM("OutAnnounce"), PRM("Announce each output?"), _OutputAnnounce);
	Config->AddBooleanProp(PRM("WarnUnhandled"), PRM("Warn on each unhandled input?"), _WarnUnhandled);

	return Config->SortItems();
}


Route *Connection::AddRoute(Route* route)
{
	if (NULL != route)
		Routes[RouteCount++] = route;

	return route;
}


Route* Connection::AddRoute(const char* name, int type, FifoBuffer *buffer, const char* prefix)
{
	// Have we already got a route called 'name'?
	Route* result = LookupRoute(name, true);

	if (NULL == result)
	{
		result = new Route(name);
		Routes[RouteCount++] = result;

		if (Globals::Verbosity > 6)
			Log::LogInfoF(PRM("Connection::AddRoute: %s, adding route '%s'"), Name, name);
	}
	else
	{
		if (Globals::Verbosity > 6)
			Log::LogInfoF(PRM("Connection::AddRoute: %s, reusing route '%s'"), Name, name);
	}

	result->Buffer = buffer;
	result->Type = type;

	// For Message Format 0, default to using the Text filter with 'prefix'.
	result->SetTextFilter(prefix);

	// For Message Format 1, default to using the Type filter.
	// N.B. We need to use at least one filter, otherwise everything will be passed.
	MessageFilterItem* typeFilter = &result->Filter.TypeFilter;
	typeFilter->Operation = ARDJACK_STRING_COMPARE_EQUALS;
	Route::GetTypeName(result->Type, typeFilter->Text);

	return result;
}


bool Connection::AlwaysUseRoute(const char* name, bool state)
{
	Route* route = LookupRoute(name);
	if (NULL == route) return false;

	route->AlwaysUse = state;

	return true;
}


bool Connection::CheckAnnounce(bool output, const char* text)
{
	bool announce = (output && _OutputAnnounce) || (!output && _InputAnnounce);

	if (announce)
	{
#ifdef ARDUINO
		// ?
		//// Flash Part "led0".
		//if (NULL != Globals::DefaultDevice)
		//	Globals::DefaultDevice->DoFlash("led0", 200);
#else
		Utils::DoBeep();
#endif
	}

	return true;
}


bool Connection::ClearRoutes()
{
	for (int i = 0; i < ARDJACK_MAX_INPUT_ROUTES; i++)
	{
		if (NULL != Routes[i])
		{
			delete Routes[i];
			Routes[i] = NULL;
		}
	}

	RouteCount = 0;

	return true;
}


bool Connection::ConfigureProperty(const char* propName, const char* propValue, const char* text)
{
	// Check for special cases like:
	//		Routes.Commands.Filter.Text.Text=$pcc:

	if (Utils::StringStartsWith(propName, "routes."))
		return ConfigureRoute(propName, propValue, text);

	return IoTObject::ConfigureProperty(propName, propValue, text);
}


bool Connection::ConfigureRoute(const char* propName, const char* propValue, const char* text)
{
	// E.g.
	//		configure udp0 Routes.Commands.Filter.Text.Operation=true

	StringList fields;
	int count = Utils::SplitText(propName, '.', &fields, 10, 100);

	if ((count < 3) || !Utils::StringEquals(fields.Get(0), "routes"))
	{
		Log::LogError(PRM("Invalid Route configuration item in: '"), text, "'");
		return false;
	}

	// Get the Route.
	Route* route = LookupRoute(fields.Get(1));
	if (NULL == route)
	{
		Log::LogError(PRM("Invalid Route name '"), fields.Get(1), "' in '", text, "'");
		return false;
	}

	// Process the next field.
	if (Utils::StringEquals(fields.Get(2), PRM("alwaysuse")))
	{
		bool state = Utils::String2Bool(propValue, true);

		route->AlwaysUse = state;

		return true;
	}

	if (Utils::StringEquals(fields.Get(2), PRM("filter")))
	{
		if (count < 5)
		{
			Log::LogError(PRM("Invalid Route configuration item in: '"), text, "'");
			return false;
		}

		// Get the filter item.
		MessageFilterItem* filterItem = route->Filter.GetFilterItem(fields.Get(3), true);
		if (NULL == filterItem)
		{
			Log::LogError(PRM("Invalid Route filter '"), fields.Get(3), "' in '", text, "'");
			return false;
		}

		// Get the filter item property.
		if (Utils::StringEquals(fields.Get(4), PRM("ignorecase")))
			filterItem->IgnoreCase = Utils::String2Bool(propValue);
		else if (Utils::StringEquals(fields.Get(4), PRM("operation")))
			filterItem->Operation = Utils::String2Comparison(propValue);
		else if (Utils::StringEquals(fields.Get(4), PRM("text")))
			strcpy(filterItem->Text, propValue);
		else
			{
				Log::LogError(PRM("Invalid Route filter property '"), fields.Get(4), "' in '", text, "'");
				return false;
			}

		return true;
	}

	if (Utils::StringEquals(fields.Get(2), PRM("noalwaysuse")))
	{
		route->AlwaysUse = false;

		return true;
	}

	if (Utils::StringEquals(fields.Get(2), PRM("type")))
	{
		if (Utils::StringEquals(propValue, PRM("command")))
			route->Type = ARDJACK_ROUTE_TYPE_COMMAND;
		else if (Utils::StringEquals(propValue, PRM("request")))
			route->Type = ARDJACK_ROUTE_TYPE_REQUEST;
		else if (Utils::StringEquals(propValue, PRM("response")))
			route->Type = ARDJACK_ROUTE_TYPE_RESPONSE;
		else
		{
			Log::LogError(PRM("Invalid Route type '"), propValue, "' in '", text, "'");
			return false;
		}

		return true;
	}

	Log::LogError(PRM("Invalid configuration item in: '"), text, "'");

	return false;
}


Route* Connection::LookupRoute(const char* name, bool quiet)
{
	if (strlen(name) == 0)
		return NULL;

	for (int i = 0; i < RouteCount; i++)
	{
		Route* route = Routes[i];

		if (Utils::StringEquals(route->Name, name))
			return route;
	}

	if (!quiet)
		Log::LogErrorF(PRM("%s: Unknown Route: '%s'"), Name, name);

	return NULL;
}


int Connection::LookupRouteIndex(const char* name, bool quiet)
{
	for (int i = 0; i < RouteCount; i++)
	{
		Route* route = Routes[i];

		if (Utils::StringEquals(route->Name, name))
			return i;
	}

	if (!quiet)
		Log::LogErrorF(PRM("%s: Unknown Route: '%s'"), Name, name);

	return -1;
}


bool Connection::OutputMessage(IoTMessage* msg)
{
	// Output 'msg'.
	// Default action is to output the message 'wire text'.
	return OutputText(msg->WireText());
}


bool Connection::OutputText(const char* text)
{
	// Send output 'text'.
	if (!_Active)
	{
		Log::LogError(PRM("Connection::OutputText: '"), Name, PRM("' is not active"));
		return false;
	}

	// Default action is to queue the text in the shared output buffer.
	return Globals::ConnectionMgr->QueueOutput(this, text);
}


bool Connection::Poll()
{
	// Used when a Connection is polled, e.g. by a ConnectionManager.
	if (_Active)
	{
		if (_CanInput)
			PollInputs();

		if (_CanOutput)
			PollOutputs();
	}

	return true;
}


bool Connection::PollInputs(int maxCount)
{
	if (!_Active || !_CanInput)
		return false;

	return true;
}


bool Connection::PollOutputs(int maxCount)
{
	if (!_Active || !_CanOutput)
		return false;

	return true;
}


bool Connection::ProcessInput(const char* text)
{
	bool handled = false;

	// Blank line?
	if (Utils::StringIsNullOrEmpty(text))
		return true;

	// Is this a 'comment' line?
	if (Utils::StringStartsWith(text, _CommentPrefix))
		return true;

	_InputMsg.Decode(text);

	if (Globals::Verbosity > 6)
		_InputMsg.LogIt();

	RouteInputMessage(&_InputMsg, &handled);

	if (!handled)
	{
		Log::LogItemF(_WarnUnhandled ? ARDJACK_LOG_WARNING : ARDJACK_LOG_INFO,
			PRM("Connection::ProcessInput: '%s' couldn't route: '%s'"), Name, text);
	}

	return true;
}


bool Connection::RemoveRoute(const char* name)
{
	int index = LookupRouteIndex(name);

	if (index < 0)
		Log::LogWarningF(PRM("Connection::RemoveRoute: '%s' has no Route '%s'"), Name, name);
	else
	{
		delete Routes[index];

		for (int i = index; i < RouteCount - 1; i++)
			Routes[i] = Routes[i + 1];

		RouteCount--;
	}

	return true;
}


bool Connection::RouteInputMessage(IoTMessage* msg, bool* routed)
{
	*routed = false;

	char computer[ARDJACK_MAX_NAME_LENGTH] = "";
	char resource[ARDJACK_MAX_NAME_LENGTH] = "";

	// Ignore messages that have a 'To' path not to this machine.
	if (strlen(msg->ToPath()) > 0)
	{
		Utils::DecodeNetworkPath(msg->ToPath(), computer, resource);

		if ((strlen(computer) > 0) && !Utils::StringEquals(computer, Globals::ComputerName))
		{
			if (Globals::Verbosity > 4)
			{
				Log::LogInfoF(PRM("Connection::RouteInputMessage: '%s' ignored message to another computer: '%s'"),
					Name, msg->WireText());
			}

			return true;
		}
	}

	// Ignore messages from this machine.
	if ((strlen(msg->FromPath()) > 0) && Utils::StringEquals(msg->FromPath(), Globals::FromName))
	{
		if (Globals::Verbosity > 3)
		{
			Log::LogInfoF(PRM("Connection::RouteInputMessage: '%s' ignored message from this computer: '%s'"),
				Name, msg->WireText());
		}

		return true;
	}

	if (RouteCount == 0)
	{
		Log::LogInfoF(PRM("Connection::RouteInputMessage: '%s' ignored message (no routes): '%s'"), Name, msg->Text());
		return true;
	}

	// Are any Routes set to 'AlwaysUse'?
	for (int i = 0; i < RouteCount; i++)
	{
		Route* route = Routes[i];

		// Always use this Route?
		if (route->AlwaysUse)
		{
			route->Handle(msg);
			*routed = true;

			if (route->StopIfHandled)
				break;
		}
	}

	if (!*routed)
	{
		// Try to match 'msg' to one or more Route's filters.
		for (int i = 0; i < RouteCount; i++)
		{
			Route* route = Routes[i];

			if (route->AlwaysUse)
				continue;

			// Use this Route?
			if (route->Filter.EvaluateMessage(msg))
			{
				route->Handle(msg);
				*routed = true;

				if (route->StopIfHandled)
					break;
			}
			else
			{
				if (Globals::Verbosity > 6)
				{
					Log::LogInfoF(PRM("Connection::RouteInputMessage: '%s': route '%s' ignored input message: '%s'"),
						Name, route->Name, msg->Text());
				}
			}
		}
	}

	if (*routed)
		return true;

	// 'msg' wasn't handled by any route in 'Routes'.

	// Is there a 'Default Route'?
	if (NULL != DefaultRoute)
	{
		// Yes - use the Default Route.
		if (Globals::Verbosity > 4)
		{
			Log::LogInfoF(PRM("Connection::RouteInputMessage: '%s', default route '%s' is handling msg: '%s'"),
				Name, DefaultRoute->Name, msg->Text());
		}

		DefaultRoute->Handle(msg);

		*routed = true;
		return true;
	}

	// We couldn't handle 'msg'.
	if (Utils::StringEquals(computer, Globals::ComputerName))
	{
		Log::LogWarning(PRM("Connection::RouteInputMessage: '"), Name, PRM("' didn't route message to this computer: '"),
			msg->Text(), "'");
	}
	else
	{
		if (Globals::Verbosity > 5)
			Log::LogInfo(PRM("Connection::RouteInputMessage: '"), Name, PRM("' didn't route message: '"), msg->Text(), "'");
	}

	return true;
}


bool Connection::SendQueuedOutput(const char* text)
{
	return SendText(text);
}


bool Connection::SendText(const char* text)
{
	return false;
}


bool Connection::SendTextQuiet(const char* text)
{
	return SendText(text);
}

