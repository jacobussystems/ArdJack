// UrlEncoder.h

#pragma once

// Implements the encoding/decoding described at:
//		https://www.lifewire.com/encoding-urls-3467463
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


class UrlEncoder
{
protected:
	char _Chars[10];
	int _Codes[10];
	char _CodeStrings[10][4];
	int _Count;

public:
	UrlEncoder();

	virtual char* Decode(const char* text, char* out, int size);
	virtual char* Encode(const char* text, char* out, int size);
};

