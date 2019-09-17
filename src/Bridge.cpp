/*
	Bridge.cpp

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
	#define _strlwr strlwr
#else
	#include "stdafx.h"
	#include <typeinfo>
#endif

#include "Bridge.h"
#include "Connection.h"
#include "Globals.h"
#include "Route.h"
#include "Log.h"
#include "Utils.h"



#ifdef ARDJACK_INCLUDE_BRIDGES

void Conn1_Callback(void* caller, Route* route, IoTMessage* msg)
{
	//Log::LogInfo(PRM("Conn1_Callback"));
	Bridge* bridge = (Bridge *)caller;
	bridge->Connection1_Callback(route, msg);
}

void Conn2_Callback(void* caller, Route* route, IoTMessage* msg)
{
	//Log::LogInfo(PRM("Conn2_Callback"));
	Bridge* bridge = (Bridge *)caller;
	bridge->Connection2_Callback(route, msg);
}



Bridge::Bridge(const char* name)
	: IoTObject(name)
{
	_DirectionType = ARDJACK_BRIDGE_DIRECTION_BIDIRECTIONAL;
	_Object1 = NULL;
	_Object1WasActive = false;
	_Object2 = NULL;
	_Object2WasActive = false;

	Object1Events = 0;
	Object2Events = 0;
}


bool Bridge::Activate()
{
	// Activate the Objects (if not already active).
	_Object1->SetActive(true);
	_Object2->SetActive(true);

	return true;
}


bool Bridge::AddConfig()
{
	// N.B. Bridges handle CONNECTIONS ONLY at the moment.
	if (!IoTObject::AddConfig())
		return false;

	Config->SetFromString("CanOutput", "true");

	if (Config->AddStringProp(PRM("Direction"), PRM("The direction type."), PRM("Bidir")) == NULL) return false;
	if (Config->AddStringProp(PRM("Object1"), PRM("Object 1 (name of a Connection).")) == NULL) return false;
	//if (Config->AddStringProp(PRM("Object1Inputs"), "") == NULL) return false;
	//if (Config->AddStringProp(PRM("Object1Outputs"), "") == NULL) return false;
	if (Config->AddStringProp(PRM("Object2"), PRM("Object 2 (name of a Connection).")) == NULL) return false;
	//if (Config->AddStringProp(PRM("Object2Inputs"), "") == NULL) return false;
	//if (Config->AddStringProp(PRM("Object2Outputs"), "") == NULL) return false;

	return Config->SortItems();
}


bool Bridge::ApplyConfig(bool quiet)
{
	if (Globals::Verbosity > 4)
		Log::LogInfo(PRM("Bridge::ApplyConfig"));

	char name[ARDJACK_MAX_NAME_LENGTH];
	char temp[102];

	// Get Object 1.
	Config->GetAsString("Object1", name);
	if (strlen(name) == 0)
	{
		sprintf(temp, PRM("Bridge '%s': No Object1 name"), Name);
		Log::LogError(temp);
		return false;
	}

	_Object1 = Globals::ObjectRegister->LookupName(name);
	if (NULL == _Object1)
	{
		sprintf(temp, PRM("Bridge '%s': Invalid Object1 name: '%s'"), Name, name);
		Log::LogError(temp);
		return false;
	}

	// Get Object 2.
	Config->GetAsString("Object2", name);
	if (strlen(name) == 0)
	{
		sprintf(temp, PRM("Bridge '%s': No Object2 name"), Name);
		Log::LogError(temp);
		return false;
	}

	_Object2 = Globals::ObjectRegister->LookupName(name);
	if (NULL == _Object2)
	{
		sprintf(temp, PRM("Bridge '%s': Invalid Object2 name: '%s'"), Name, name);
		Log::LogError(temp);
		return false;
	}

	// Get the Direction.
	Config->GetAsString("Direction", name);
	if (strlen(name) == 0)
	{
		sprintf(temp, PRM("Bridge '%s': No Direction"), Name);
		Log::LogError(temp);
		return false;
	}

	_strupr(name);

	if (Utils::StringEquals(name, "UNIDIR12"))
		_DirectionType = ARDJACK_BRIDGE_DIRECTION_UNIDIRECTIONAL_1TO2;
	else if (Utils::StringEquals(name, "UNIDIR21"))
		_DirectionType = ARDJACK_BRIDGE_DIRECTION_UNIDIRECTIONAL_2TO1;
	else
		_DirectionType = ARDJACK_BRIDGE_DIRECTION_BIDIRECTIONAL;

	// Save the Object states.
	_Object1WasActive = _Object1->Active();
	_Object2WasActive = _Object2->Active();

	// Setup Connections (if any).
	Connection* conn;

	if (_Object1->Type == ARDJACK_OBJECT_TYPE_CONNECTION)
	{
		conn = (Connection*)_Object1;

		// Clear all Routes.
		conn->ClearRoutes();

		// Setup a Route for Connection 1?
		switch (_DirectionType)
		{
		case ARDJACK_BRIDGE_DIRECTION_BIDIRECTIONAL:
		case ARDJACK_BRIDGE_DIRECTION_UNIDIRECTIONAL_1TO2:
			Route* route = new Route("Object1");
			route->CallbackObj = this;
			route->Callback = Conn1_Callback;
			conn->AddRoute(route);
			break;
		}
	}

	if (_Object2->Type == ARDJACK_OBJECT_TYPE_CONNECTION)
	{
		conn = (Connection*)_Object2;

		// Clear all Routes.
		conn->ClearRoutes();

		// Setup an Route for Connection 2?
		switch (_DirectionType)
		{
		case ARDJACK_BRIDGE_DIRECTION_BIDIRECTIONAL:
		case ARDJACK_BRIDGE_DIRECTION_UNIDIRECTIONAL_2TO1:
			Route* route = new Route("Object2");
			route->CallbackObj = this;
			route->Callback = Conn2_Callback;
			conn->AddRoute(route);
			break;
		}
	}

	return true;
}


bool Bridge::Connection1_Callback(Route* sender, IoTMessage* msg)
{
	if (!_Object1->Active()) return true;
	if (!_Object2->Active()) return true;

	switch (_DirectionType)
	{
		case ARDJACK_BRIDGE_DIRECTION_BIDIRECTIONAL:
		case ARDJACK_BRIDGE_DIRECTION_UNIDIRECTIONAL_1TO2:
			break;

		default:
			return true;
	}

	Object1Events++;

	// Modify the message before forwarding it?

	// Forward this message to Connection 2.
	Connection* conn = (Connection*)_Object2;

	return conn->OutputMessage(msg);
}


bool Bridge::Connection2_Callback(Route* sender, IoTMessage* msg)
{
	if (!_Object1->Active()) return true;
	if (!_Object2->Active()) return true;

	switch (_DirectionType)
	{
		case ARDJACK_BRIDGE_DIRECTION_BIDIRECTIONAL:
		case ARDJACK_BRIDGE_DIRECTION_UNIDIRECTIONAL_2TO1:
			break;

		default:
			return true;
	}

	Object2Events++;

	// Modify the message before forwarding it?

	// Forward this message to Connection 1.
	Connection* conn = (Connection*)_Object1;

	return conn->OutputMessage(msg);
}


bool Bridge::Deactivate()
{
	// Deactivate the Connections, if they were initially inactive.
	_Object1->SetActive(_Object1WasActive);
	_Object2->SetActive(_Object2WasActive);

	// Remove the Routes we added earlier.
	Connection* conn;

	if (_Object1->Type == ARDJACK_OBJECT_TYPE_CONNECTION)
	{
		conn = (Connection*)_Object1;
		conn->RouteCount = 0;
	}

	if (_Object2->Type == ARDJACK_OBJECT_TYPE_CONNECTION)
	{
		conn = (Connection*)_Object2;
		conn->RouteCount = 0;
	}

	return true;
}


//virtual bool Bridge::Configure(char settings[][ARDJACK_MAX_VALUE_LENGTH], int count)
//{
//}

#endif
