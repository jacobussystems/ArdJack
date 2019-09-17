/*
	Log.h

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

#pragma once

#ifdef ARDUINO
	#include <arduino.h>
#else
	#include "stdafx.h"
#endif

#include "FifoBuffer.h"

typedef void(*Logger_Callback)(const char* text);

static const int ARDJACK_LOG_ERROR = 0;
static const int ARDJACK_LOG_EXCEPTION = 1;
static const int ARDJACK_LOG_INFO = 2;
static const int ARDJACK_LOG_WARNING = 3;



class Log
{
protected:
#ifdef ARDUINO
	static char _CurLine[240];
#else
	static char _CurLine[1000];
#endif
	static FifoBuffer _OutputBuffer;

	static bool CheckOutputBuffer(int maxCount = 6);
	static bool Output(const char* text);
	static void Write(const char* text);
	static void WriteInternal(const char* caption, const char* arg0, const char* arg1, const char* arg2, const char* arg3,
		const char* arg4, const char* arg5, const char* arg6, const char* arg7, const char* arg8, const char* arg9);
	static void WriteMemory();
	static void WriteString(const char* caption, const char* text);
	static void WriteTime();

public:
	static bool Buffered;
	static bool IncludeMemory;
	static bool IncludeTime;
	static bool InUnitTest;
	static char Prefix[20];
	static bool UseSerial;
	static bool UseSerialUSB;

#ifdef ARDUINO
#else
	static Logger_Callback LoggerCallback;									// a callback for output during Unit Tests
#endif

	static void Flush();
	static bool Init();

	static void LogError(const char* arg0 = NULL, const char* arg1 = NULL, const char* arg2 = NULL,
		const char* arg3 = NULL, const char* arg4 = NULL, const char* arg5 = NULL, const char* arg6 = NULL,
		const char* arg7 = NULL, const char* arg8 = NULL, const char* arg9 = NULL);

	static void LogErrorF(const char* format, ...);

	static void LogException(const char* source, int code = -1, const char* arg0 = NULL, const char* arg1 = NULL,
		const char* arg2 = NULL, const char* arg3 = NULL, const char* arg4 = NULL, const char* arg5 = NULL,
		const char* arg6 = NULL, const char* arg7 = NULL, const char* arg8 = NULL, const char* arg9 = NULL);

	static void LogInfo(const char* arg0 = NULL, const char* arg1 = NULL, const char* arg2 = NULL,
		const char* arg3 = NULL, const char* arg4 = NULL, const char* arg5 = NULL, const char* arg6 = NULL,
		const char* arg7 = NULL, const char* arg8 = NULL, const char* arg9 = NULL);

	static void LogInfoF(const char* format, ...);

	static void LogItem(int type, const char* arg0 = NULL, const char* arg1 = NULL, const char* arg2 = NULL,
		const char* arg3 = NULL, const char* arg4 = NULL, const char* arg5 = NULL, const char* arg6 = NULL,
		const char* arg7 = NULL, const char* arg8 = NULL, const char* arg9 = NULL);

	static void LogItemF(int type, const char* format, ...);

	static void LogWarning(const char* arg0 = NULL, const char* arg1 = NULL, const char* arg2 = NULL,
		const char* arg3 = NULL, const char* arg4 = NULL, const char* arg5 = NULL, const char* arg6 = NULL,
		const char* arg7 = NULL, const char* arg8 = NULL, const char* arg9 = NULL);

	static void LogWarningF(const char* format, ...);
	static void Poll();
};

