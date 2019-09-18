/*
	ClipboardConnection.cpp

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

#include "stdafx.h"

#include "ClipboardConnection.h"
#include "IoTMessage.h"
#include "Log.h"
#include "Utils.h"




ClipboardConnection::ClipboardConnection(const char* name)
	: Connection(name)
{
}


ClipboardConnection::~ClipboardConnection()
{
}


bool ClipboardConnection::CheckClipboard()
{
	if (!IsClipboardFormatAvailable(CF_TEXT))
		return false;

	HANDLE hData = GetClipboardData(CF_TEXT);
	if (NULL == hData)
	{
		Log::LogError(PRM("Failed to get Clipboard data handle"));
		return false;
	}

	// Lock the handle to get the actual text pointer
	char *pText = static_cast<char*>(GlobalLock(hData));
	if (NULL == pText)
	{
		Log::LogError(PRM("Failed to get Clipboard text"));
		return false;
	}

	// Save the text.
	char buffer[260];
	int count = Utils::StringLen(pText);
	if (count > 256)
		count = 256;
	strncpy(buffer, pText, count);
	buffer[count] = NULL;

	// Release the lock.
	GlobalUnlock(hData);

	if (Globals::Verbosity > 2)
	{
		char temp[400];
		sprintf(temp, "%s RX: Read %d bytes: ", Name, count);
		Log::LogInfo(temp, buffer);
	}

	EmptyClipboard();

	ProcessInput(buffer);

	return true;
}


bool ClipboardConnection::OutputMessage(IoTMessage* msg)
{
	// Write to the Clipboard.

	// Ignored for now.
	char temp[102];
	sprintf(temp, PRM("ClipboardConnection::OutputMessage: '%s': Ignored '"), Name);
	Log::LogInfo(temp, msg->Text(), "'");

	return true;
}


bool ClipboardConnection::PollInputs(int maxCount)
{
	if (!OpenClipboard(NULL))
	{
		Log::LogError(PRM("Failed to open Clipboard"));
		return false;
	}

	CheckClipboard();
	CloseClipboard();

	return true;
}


bool ClipboardConnection::PollOutputs(int maxCount)
{
	return true;
}


bool ClipboardConnection::SendQueuedOutput(const char* text)
{
	return false;
}

