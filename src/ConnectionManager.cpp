/*
	ConnectionManager.cpp

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

#include "Connection.h"
#include "ConnectionManager.h"
#include "Enumeration.h"
#include "Globals.h"
#include "IoTManager.h"
#include "Log.h"
#include "Utils.h"




ConnectionManager::ConnectionManager()
	: IoTManager()
{
	if (Globals::Verbosity > 7)
		Log::LogInfo(PRM("ConnectionManager ctor"));

	ObjectType = ARDJACK_OBJECT_TYPE_CONNECTION;
	OutputBuffer = new FifoBuffer(sizeof(ConnectionOutputBufferItem), ARDJACK_MAX_CONNECTION_OUTPUT_BUFFER_ITEMS);

	if (NULL == Subtypes)
	{
		Subtypes = new Enumeration(PRM("Subtypes"));

#ifdef ARDUINO
#else
		Subtypes->Add(PRM("Clipboard"), ARDJACK_CONNECTION_SUBTYPE_CLIPBOARD);
#endif
		Subtypes->Add(PRM("HTTP"), ARDJACK_CONNECTION_SUBTYPE_HTTP);
		Subtypes->Add(PRM("LOG"), ARDJACK_CONNECTION_SUBTYPE_LOG);
		Subtypes->Add(PRM("Serial"), ARDJACK_CONNECTION_SUBTYPE_SERIAL);
		Subtypes->Add(PRM("TCP"), ARDJACK_CONNECTION_SUBTYPE_TCP);
		Subtypes->Add(PRM("UDP"), ARDJACK_CONNECTION_SUBTYPE_UDP);
	}
}


ConnectionManager::~ConnectionManager()
{
	if (NULL != OutputBuffer)
		delete OutputBuffer;
}


bool ConnectionManager::CheckOutputBuffer()
{
	if ((NULL == OutputBuffer) || OutputBuffer->IsEmpty())
		return true;

	ConnectionOutputBufferItem item;

	for (int i = 0; i < 5; i++)
	{
		if (OutputBuffer->IsEmpty() || !OutputBuffer->Pop(&item))
			break;

		if (item.Conn->Active())
			item.Conn->SendQueuedOutput(item.Text);
		else
		{
			Log::LogWarning(PRM("ConnectionManager::CheckOutputBuffer: '"), item.Conn->Name,
				PRM("' is not active - item discarded: '"), item.Text, "'");
		}
	}

	return true;
}


bool ConnectionManager::Interact(const char* text)
{
	// Add/modify/delete/reset/send (to) a Connection.
	StringList fields;

	int count = Utils::SplitText(text, ' ', &fields, 20, ARDJACK_MAX_VALUE_LENGTH);
	if (count < 2)
	{
		Log::LogError(PRM("Interact: Invalid command: "), text);
		return false;
	}

	char connName[ARDJACK_MAX_VALUE_LENGTH];
	strcpy(connName, fields.Get(0));
	Connection* conn = LookupConnection(connName);

	char action[ARDJACK_MAX_VALUE_LENGTH];
	strcpy(action, fields.Get(1));
	_strupr(action);

	if (Utils::StringEquals(action, PRM("ALWAYSUSE"), true))
	{
		// Set the specified Route(s) to 'AlwaysUse'.
		for (int i = 2; i < fields.Count; i++)
		{
			Route* route = conn->LookupRoute(fields.Get(i));

			if (NULL != route)
				route->AlwaysUse = true;
		}

		return true;
	}

	if (Utils::StringEquals(action, PRM("NOALWAYSUSE"), true))
	{
		// Set the specified Route(s) to NOT 'AlwaysUse'.
		for (int i = 2; i < fields.Count; i++)
		{
			Route* route = conn->LookupRoute(fields.Get(i));

			if (NULL != route)
				route->AlwaysUse = false;
		}

		return true;
	}

	if (Utils::StringEquals(action, PRM("RESET"), true))
	{
		// Clear any Routes.
		conn->RouteCount = 0;

		// Setup standard Routes.
		return Globals::SetupStandardRoutes(conn);
	}

	if (Utils::StringEquals(action, "SEND", true))
	{
		if (count < 3)
		{
			Log::LogError(PRM("Interact: Invalid SEND command: "), text);
			return false;
		}

		return conn->OutputText(fields.Get(2));
	}

	Log::LogErrorF(PRM("Invalid CONNECTION command: '%s'"), text);

	return false;
}


Connection* ConnectionManager::LookupConnection(const char* name)
{
	IoTObject* obj = Globals::ObjectRegister->LookupName(name);
	if (NULL == obj) return NULL;

	if (obj->Type != ARDJACK_OBJECT_TYPE_CONNECTION)
		return NULL;

	return (Connection*)obj;
}


int ConnectionManager::LookupSubtype(const char* name)
{
	return Subtypes->LookupName(name, ARDJACK_OBJECT_SUBTYPE_UNKNOWN);
}


void ConnectionManager::Poll()
{
	if (_PollBusy) return;

	_PollBusy = true;

	// Poll the active Connections.
	PollObjectsOfType(ARDJACK_OBJECT_TYPE_CONNECTION);

	// Check the output buffer.
	CheckOutputBuffer();

	_PollBusy = false;
}


bool ConnectionManager::QueueOutput(Connection* conn, const char* text)
{
	if (NULL == OutputBuffer)
		return false;

	ConnectionOutputBufferItem item;
	item.Conn = conn;
	strcpy(item.Text, text);

	for (int i = 0; i < 10; i++)
	{
		if (!OutputBuffer->IsFull() && OutputBuffer->Push(&item))
			return true;

		Utils::DelayMs(200);
	}

	Log::LogError(PRM("Buffer full: unable to queue output from '"), conn->Name, "'");

	return false;
}

