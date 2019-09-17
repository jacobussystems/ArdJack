// UrlEncoder.cpp

#include "pch.h"

#include "Globals.h"
#include "Log.h"
#include "UrlEncoder.h"
#include "Utils.h"


// This class encodes/decodes text using the URL encoding scheme:
//
//		:	%3B
//		/	%2F
//		#	%23
//		?	%3F
//		&	%24
//		@	%40
//		%	%25
//		+	%2B
//			%20 or + in query strings


UrlEncoder::UrlEncoder()
{
	_Count = 0;

	_Chars[_Count] = ':';
	_Codes[_Count++] = 0x3B;
	_Chars[_Count] = '/';
	_Codes[_Count++] = 0x2F;
	_Chars[_Count] = '#';
	_Codes[_Count++] = 0x23;
	_Chars[_Count] = '?';
	_Codes[_Count++] = 0x3F;
	_Chars[_Count] = '&';
	_Codes[_Count++] = 0x24;
	_Chars[_Count] = '@';
	_Codes[_Count++] = 0x40;
	_Chars[_Count] = '%';
	_Codes[_Count++] = 0x25;
	_Chars[_Count] = '+';
	_Codes[_Count++] = 0x2B;
	_Chars[_Count] = ' ';
	_Codes[_Count++] = 0x20;

	for (int i = 0; i < _Count; i++)
		sprintf(_CodeStrings[i], "%%%02X", _Codes[i]);
}


char* UrlEncoder::Decode(const char* text, char* out, int size)
{
	// TEMPORARY:  Slow method.
	strcpy(out, text);
	char temp[202];

	for (int i = 0; i < _Count; i++)
	{
		if (Utils::StringContains(out, _CodeStrings[i]))
		{
			char replacement[2];
			replacement[0] = _Chars[i];
			replacement[1] = NULL;

			strcpy(temp, out);
			Utils::StringReplace(temp, _CodeStrings[i], replacement, false, out, size);
		}
	}

	return out;
}


char* UrlEncoder::Encode(const char* text, char* out, int size)
{
	Utils::StringReplaceMultiHex(text, _Chars, _Codes, _Count, out, size);

	return out;
}

