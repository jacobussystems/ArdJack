/*
	IoTMessage.cpp

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

#include "Globals.h"
#include "IoTMessage.h"
#include "Log.h"
#include "StringList.h"
#include "Utils.h"




IoTMessage::IoTMessage()
{
	if (Globals::Verbosity > 7)
		Log::LogInfoF(PRM("IoTMessage ctor: %p"), this);

	_Items = new StringList(40);

	Format = ARDJACK_MESSAGE_FORMAT_0;
	Clear();
}


IoTMessage::~IoTMessage()
{
	if (NULL != _Items)
		delete _Items;
}


void IoTMessage::Clear()
{
	_Items->Clear();
}


bool IoTMessage::CreateMessageToSend(const char* type, int format, const char* text, IoTMessage* msg, const char* fromPath,
	const char* toPath, const char* returnPath)
{
	msg->Clear();

	msg->SetType(type);
	msg->Format = format;
	msg->SetText(text);
	msg->SetFromPath(fromPath);
	msg->SetToPath(toPath);
	msg->SetReturnPath(returnPath);

	msg->Encode();

	return true;
}


bool IoTMessage::Decode(const char* line)
{
	Clear();

	char temp[ARDJACK_MAX_MESSAGE_WIRETEXT_LENGTH];
	strcpy(temp, line);
	Utils::Trim(temp);

	SetWireText(temp);

	if (temp[0] == '[')
	{
		// There's a '[' header.
		Format = ARDJACK_MESSAGE_FORMAT_1;
		return DecodeFormat1(temp);
	}

	Format = ARDJACK_MESSAGE_FORMAT_0;
	SetText(temp);

	return true;
}


//bool IoTMessage::DecodeFormat0(const char* line)
//{
//	// 'line' starts with a prefix starting with '$' and ending with a colon, followed by zero or more spaces
//	// and the message text (if any).
//	// E.g. "$cmd: some text"
//
//	const char* prefixEnd = strchr(line, ':');
//
//	if (NULL == prefixEnd)
//	{
//		// There's no prefix.
//		SetText(line);
//		return true;
//	}
//
//	char temp[ARDJACK_MAX_MESSAGE_TEXT_LENGTH];
//
//	// There is a prefix.
//	// Store all of it as the 'Prefix', including the '$' and ':'.
//	int count = prefixEnd - line + 1;
//	strncpy(temp, line, count);
//	temp[count] = NULL;
//	Utils::Trim(temp);
//	SetPrefix(temp);
//
//	// Remove the prefix and store the remainder as the 'Text'.
//	strcpy(temp, prefixEnd + 1);
//	Utils::Trim(temp);
//	SetText(temp);
//
//	return true;
//}


bool IoTMessage::DecodeFormat1(const char* line)
{
	// 'line' contains a [header] followed by zero or more spaces and the message text (if any).
	// e.g. "[from=\\comp1\dev1 to=\\comp2\dev2] some text"

	const char* headerEnd = strchr(line, ']');

	if (NULL == headerEnd)
	{
		// Assume there's no header.
		SetText(line);
		return true;
	}

	// Get the header and text (if any).
	char header[ARDJACK_MAX_MESSAGE_WIRETEXT_LENGTH];
	int count = (headerEnd - line) - 1;
	strncpy(header, line + 1, count);
	header[count] = NULL;

	char temp[ARDJACK_MAX_MESSAGE_TEXT_LENGTH];
	strcpy(temp, headerEnd + 1);
	Utils::Trim(temp);
	SetText(temp);

	// Parse the header.
	StringList fields;
	count = Utils::SplitText(header, ' ', &fields, ARDJACK_MAX_VALUES, ARDJACK_MAX_VALUE_LENGTH);

	char parts[2][ARDJACK_MAX_VALUE_LENGTH];
	char key[10];
	char value[30];

	for (int i = 0; i < count; i++)
	{
		int partCount = Utils::SplitText2Array(fields.Get(i), '=', parts, 2, ARDJACK_MAX_VALUE_LENGTH);

		strcpy(key, parts[0]);
		Utils::Trim(key);
		_strlwr(key);

		value[0] = NULL;

		if (partCount == 2)
			strcpy(value, parts[1]);

		if (strcmp(key, "from") == 0)
			SetFromPath(value);

		else if (strcmp(key, "return") == 0)
			SetReturnPath(value);

		else if (strcmp(key, "to") == 0)
			SetToPath(value);

		else if (strcmp(key, "type") == 0)
			SetType(value);
	}

	return true;
}


//bool IoTMessage::DecodePath(const char* path, int *pathType, char *subPath)
//{
//	// Decode 'Path' to get the message type and the SubPath (if any).
//	*pathType = ARDJACK_MESSAGE_PATH_NONE;
//	subPath[0] = NULL;
//
//	char parts[2][ARDJACK_MAX_VALUE_LENGTH];
//	int count = Utils::SplitText2Array(path, '.', parts, 2, ARDJACK_MAX_VALUE_LENGTH);
//	char typeStr[20];
//
//	if (count == 1)
//		strcpy(typeStr, path);
//	else
//	{
//		strcpy(typeStr, parts[0]);
//		strcpy(subPath, parts[1]);
//		Utils::Trim(subPath);
//	}
//
//	Utils::Trim(typeStr);
//	_strlwr(typeStr);
//
//	if (strcmp(typeStr, "device") == 0)
//		*pathType = ARDJACK_MESSAGE_PATH_DEVICE;
//	else
//		Log::LogWarning(PRM("Invalid message path: '"), path, "'");
//
//	return true;
//}


//int IoTMessage::DecodeType(const char* name)
//{
//	char useName[ARDJACK_MAX_NAME_LENGTH];
//	strcpy(useName, name);
//	_strlwr(useName);
//
//	if (strcmp(useName, "command") == 0)
//		return ARDJACK_MESSAGE_TYPE_COMMAND;
//
//	if (strcmp(useName, "data") == 0)
//		return ARDJACK_MESSAGE_TYPE_DATA;
//
//	if (strcmp(useName, "heartbeat") == 0)
//		return ARDJACK_MESSAGE_TYPE_HEARTBEAT;
//
//	if (strcmp(useName, "notification") == 0)
//		return ARDJACK_MESSAGE_TYPE_NOTIFICATION;
//
//	if (strcmp(useName, "request") == 0)
//		return ARDJACK_MESSAGE_TYPE_REQUEST;
//
//	if (strcmp(useName, "response") == 0)
//		return ARDJACK_MESSAGE_TYPE_RESPONSE;
//
//	Log::LogWarning(PRM("Invalid message type: '"), name, "'");
//
//	return ARDJACK_MESSAGE_TYPE_NONE;
//}


bool IoTMessage::Encode()
{
	switch (Format)
	{
	case ARDJACK_MESSAGE_FORMAT_0:
		EncodeFormat0();
		break;

	case ARDJACK_MESSAGE_FORMAT_1:
		EncodeFormat1();
		break;
	}

	return true;
}


bool IoTMessage::EncodeFormat0()
{
	SetWireText(Text());

	return true;
}


bool IoTMessage::EncodeFormat1()
{
	// # The result may contain a [header] followed by a space and the message text.
	//  e.g. '[from=dev1 to=dev2 path=a.bb.ccc sent=???] text'

	char work[ARDJACK_MAX_MESSAGE_WIRETEXT_LENGTH];
	strcpy(work, "[");

	// Encode the header.
	if ((strlen(Type()) > 0))
	{
		strcat(work, "type=");
		strcat(work, Type());
		strcat(work, " ");
	}

	if ((strlen(FromPath()) > 0))
	{
		strcat(work, "from=");
		strcat(work, FromPath());
		strcat(work, " ");
	}

	if ((strlen(ToPath()) > 0))
	{
		strcat(work, "to=");
		strcat(work, ToPath());
		strcat(work, " ");
	}

	if ((strlen(ReturnPath()) > 0) && !Utils::StringEquals(ReturnPath(), FromPath()))
	{
		strcat(work, "return=");
		strcat(work, ReturnPath());
		strcat(work, " ");
	}

	Utils::Trim(work);
	strcat(work, "] ");

	// Add the text.
	strcat(work, Text());

	Utils::Trim(work);
	SetWireText(work);

	return true;
}


const char* IoTMessage::FromPath()
{
	return _Items->Get(MESSAGE_FROM_PATH);
}


//char *IoTMessage::GetTypeName(char *work)
//{
//	work[0] = NULL;
//
//	switch (Type)
//	{
//	case ARDJACK_MESSAGE_TYPE_COMMAND:
//		strcpy(work, "Command");
//		break;
//
//	case ARDJACK_MESSAGE_TYPE_DATA:
//		strcpy(work, "Data");
//		break;
//
//	case ARDJACK_MESSAGE_TYPE_HEARTBEAT:
//		strcpy(work, "Heartbeat");
//		break;
//
//	case ARDJACK_MESSAGE_TYPE_NOTIFICATION:
//		strcpy(work, "Notification");
//		break;
//
//	case ARDJACK_MESSAGE_TYPE_REQUEST:
//		strcpy(work, "Request");
//		break;
//
//	case ARDJACK_MESSAGE_TYPE_RESPONSE:
//		strcpy(work, "Response");
//		break;
//
//	default:
//		strcpy(work, "None");
//		break;
//	}
//
//	return work;
//}


void IoTMessage::LogIt()
{
	char buffer[302];

	Log::LogInfo(ToString(buffer));
}


const char* IoTMessage::ReturnPath()
{
	return _Items->Get(MESSAGE_RETURN_PATH);
}


bool IoTMessage::SetFromPath(const char* text)
{
	return _Items->Put(MESSAGE_FROM_PATH, text);
}


bool IoTMessage::SetReturnPath(const char* text)
{
	return _Items->Put(MESSAGE_RETURN_PATH, text);
}


bool IoTMessage::SetText(const char* text)
{
	return _Items->Put(MESSAGE_TEXT, text);
}


bool IoTMessage::SetToPath(const char* text)
{
	return _Items->Put(MESSAGE_TO_PATH, text);
}


bool IoTMessage::SetType(const char* text)
{
	return _Items->Put(MESSAGE_TYPE, text);
}


bool IoTMessage::SetWireText(const char* text)
{
	return _Items->Put(MESSAGE_WIRETEXT, text);
}


const char* IoTMessage::Text()
{
	return _Items->Get(MESSAGE_TEXT);
}


const char* IoTMessage::ToPath()
{
	return _Items->Get(MESSAGE_TO_PATH);
}


const char* IoTMessage::ToString(char* out)
{
	sprintf(out, PRM("Message (format %d): "), Format);

	switch (Format)
	{
	case ARDJACK_MESSAGE_FORMAT_0:
		break;

	default:
		if ((strlen(Type()) > 0))
		{
			strcat(out, "type '");
			strcat(out, Type());
			strcat(out, "', ");
		}

		if ((strlen(FromPath()) > 0))
		{
			strcat(out, "from '");
			strcat(out, FromPath());
			strcat(out, "', ");
		}

		if ((strlen(ToPath()) > 0))
		{
			strcat(out, "to '");
			strcat(out, ToPath());
			strcat(out, "', ");
		}

		if (strlen(ReturnPath()) > 0)
		{
			strcat(out, "return '");
			strcat(out, ReturnPath());
			strcat(out, "', ");
		}

		break;
	}

	if (strlen(Text()) > 0)
	{
		strcat(out, "text '");
		strcat(out, Text());
		strcat(out, "'");
	}

	return out;
}


const char* IoTMessage::Type()
{
	return _Items->Get(MESSAGE_TYPE);
}


const char* IoTMessage::WireText()
{
	return _Items->Get(MESSAGE_WIRETEXT);
}

