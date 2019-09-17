/*
	LogConnection.cpp

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

#include "stdafx.h"

#include "IoTMessage.h"
#include "Log.h"
#include "LogConnection.h"
#include "Utils.h"



LogConnection::LogConnection(const char* name)
	: Connection(name)
{
}


LogConnection::~LogConnection()
{
}


bool LogConnection::OutputMessage(IoTMessage* msg)
{
	// Log this output line.
	Log::LogInfo("LogConnection::OutputMessage: ", msg->Text());

	return SendText(msg->Text());
}


bool LogConnection::SendText(const char* text)
{
	// Log this output line.
	Log::LogInfo("LogConnection::SendText: ", text);

	TxCount += (int)strlen(text);
	TxEvents++;

	return true;
}

