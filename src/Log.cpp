/*
	Log.cpp

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
	#include <stdarg.h>
#else
	#include <iostream>
	#include <windows.h>
#endif

#include "Connection.h"
#include "FifoBuffer.h"
#include "Globals.h"
#include "Log.h"
#include "MemoryFreeExt.h"
#include "Utils.h"

#ifdef ARDUINO
	#ifdef ARDJACK_RTC_AVAILABLE
		#include "RtcClock.h"
	#else
		#include "ArduinoClock.h"
	#endif

	char Log::_CurLine[] = "";
#else
	char Log::_CurLine[] = "";
#endif

FifoBuffer Log::_OutputBuffer = FifoBuffer(ARDJACK_MAX_LOG_BUFFER_ITEM_LENGTH, ARDJACK_MAX_LOG_BUFFER_ITEMS);

bool Log::Buffered = false;
bool Log::IncludeMemory = false;
bool Log::IncludeTime = true;
bool Log::InUnitTest = false;
char Log::Prefix[20] = "# ";
bool Log::UseSerial = true;
bool Log::UseSerialUSB = false;

#ifdef ARDUINO
#else
	Logger_Callback Log::LoggerCallback = NULL;
#endif



bool Log::CheckOutputBuffer(int maxCount)
{
	if (Buffered && !_OutputBuffer.IsEmpty())
	{
		char text[ARDJACK_MAX_LOG_BUFFER_ITEM_LENGTH];

		for (int i = 1; i <= maxCount; i++)
		{
			if (_OutputBuffer.IsEmpty())
				break;

			_OutputBuffer.Pop(text);
			Output(text);
		}
	}

	return true;
}


void Log::Flush()
{
	if (InUnitTest)
	{
#ifdef ARDUINO
#else
		if (NULL != LoggerCallback)
			LoggerCallback(_CurLine);
#endif
		strcpy(_CurLine, Prefix);

		return;
	}

	if (Buffered)
	{
		// Add '_CurLine' to the output buffer.
		if (_OutputBuffer.IsFull())
		{
			char text[ARDJACK_MAX_LOG_BUFFER_ITEM_LENGTH];
			_OutputBuffer.Pop(text);
			Output(text);
		}

		// Limit the length of this log line.
		_CurLine[ARDJACK_MAX_LOG_BUFFER_ITEM_LENGTH - 4] = NULL;

		_OutputBuffer.Push(_CurLine);

		// Allow some time for output buffering?
		Utils::DelayMs(10);
	}
	else
		Output(_CurLine);

	strcpy(_CurLine, Prefix);
}


bool Log::Init()
{
	strcpy(_CurLine, Prefix);

	LogInfo(PRM("Log::Init"));

	return true;
}


void Log::LogError(const char* arg0, const char* arg1, const char* arg2, const char* arg3, const char* arg4,
	const char* arg5, const char* arg6, const char* arg7, const char* arg8, const char* arg9)
{
	WriteInternal(PRM("ERROR"), arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
	Utils::DoBeep();
}


void Log::LogErrorF(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	char temp[202];
	vsnprintf(temp, 200, format, args);
	LogError(temp);

	va_end(args);
}


void Log::LogException(const char* source, int code, const char* arg0, const char* arg1, const char* arg2,
	const char* arg3, const char* arg4, const char* arg5, const char* arg6, const char* arg7, const char* arg8,
	const char* arg9)
{
	char temp[102];
	sprintf(temp, PRM("EXCEPTION %d in '%s'"), code, source);

	WriteInternal(temp, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
	Utils::DoBeep();
}


void Log::LogInfo(const char* arg0, const char* arg1, const char* arg2, const char* arg3, const char* arg4,
	const char* arg5, const char* arg6, const char* arg7, const char* arg8, const char* arg9)
{
	WriteInternal(NULL, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}


void Log::LogInfoF(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	char temp[202];
	vsnprintf(temp, 200, format, args);
	LogInfo(temp);

	va_end(args);
}


void Log::LogItem(int type, const char* arg0, const char* arg1, const char* arg2, const char* arg3, const char* arg4,
	const char* arg5, const char* arg6, const char* arg7, const char* arg8, const char* arg9)
{
	switch (type)
	{
	case ARDJACK_LOG_ERROR:
		LogError(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
		break;

	case ARDJACK_LOG_EXCEPTION:
		LogException("?", 0, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
		break;

	case ARDJACK_LOG_WARNING:
		LogWarning(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
		break;

	default:
		LogInfo(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
		break;
	}
}


void Log::LogItemF(int type, const char* format, ...)
{
	va_list args;
	va_start(args, format);

	char temp[202];
	vsnprintf(temp, 200, format, args);

	switch (type)
	{
	case ARDJACK_LOG_ERROR:
		LogError(temp);
		break;

	case ARDJACK_LOG_WARNING:
		LogWarning(temp);
		break;

	default:
		LogInfo(temp);
		break;
	}

	va_end(args);
}


void Log::LogWarning(const char* arg0, const char* arg1, const char* arg2, const char* arg3, const char* arg4,
	const char* arg5, const char* arg6, const char* arg7, const char* arg8, const char* arg9)
{
	WriteInternal(PRM("WARNING"), arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}


void Log::LogWarningF(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	char temp[202];
	vsnprintf(temp, 200, format, args);
	LogWarning(temp);

	va_end(args);
}


bool Log::Output(const char* text)
{
	if (NULL != Globals::LogTarget)
	{
		// N.B. Infinite loop at this point if any 'Log' statements are executed while writing the text!

		// Write the current log line to the global 'log target' Connection.
		Connection *conn = (Connection *)Globals::LogTarget;
		conn->SendTextQuiet(text);

		return true;
	}

#ifdef ARDUINO
	SERIAL_PORT_MONITOR.println(text);

	//if (UseSerial)
	//	SERIAL_PORT_MONITOR.println(text);

	//#ifdef ARDJACK_ARDUINO_DUE
	//	if (UseSerialUSB)
	//		SERIAL_PORT_MONITOR.println(text);
	//#endif
#else
	printf(text);
	printf("\n");

	#ifdef _DEBUG
		OutputDebugString(text);
		OutputDebugString("\n");
	#endif
#endif

	return true;
}


void Log::Poll()
{
	if (Buffered)
		CheckOutputBuffer();
}


void Log::Write(const char* text)
{
	// Append 'text' to '_CurLine'.
	strcat(_CurLine, text);
}


void Log::WriteInternal(const char* caption, const char* arg0, const char* arg1, const char* arg2, const char* arg3,
	const char* arg4, const char* arg5, const char* arg6, const char* arg7, const char* arg8, const char* arg9)
{
	if (!Globals::InitialisedStatic)
		return;

	if (IncludeTime)
		WriteTime();

	if (IncludeMemory)
		WriteMemory();

	if (NULL != caption)
	{
		Write(caption);
		Write(" - ");
	}

	if (NULL != arg0)
	{
		Write(arg0);

		if (NULL != arg1)
		{
			Write(arg1);

			if (NULL != arg2)
			{
				Write(arg2);

				if (NULL != arg3)
				{
					Write(arg3);

					if (NULL != arg4)
					{
						Write(arg4);

						if (NULL != arg5)
						{
							Write(arg5);

							if (NULL != arg6)
							{
								Write(arg6);

								if (NULL != arg7)
								{
									Write(arg7);

									if (NULL != arg8)
									{
										Write(arg8);

										if (NULL != arg9)
											Write(arg9);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	Flush();
}


void Log::WriteMemory()
{
#ifdef ARDUINO
	MemoryInfo memInfo;
	GetHeapInfo(&memInfo);

	int32_t freeRAM = Utils::GetFreeRAM();

	char temp[20];
	sprintf(temp, PRM("[%d, %d]"), memInfo.HeapMax, freeRAM);

	Write(temp);
	Write(" ");
#endif
}


void Log::WriteString(const char* caption, const char* text)
{
	char temp[10];

	if (IncludeTime)
		WriteTime();

	if (IncludeMemory)
		WriteMemory();

	Write(caption);
	Write("'");
	Write(text);
	Write("', ");
	Write(itoa(Utils::StringLen(text), temp, 10));
	Write(" chars: ");

	for (int i = 0; i < Utils::StringLen(text); i++)
	{
		sprintf(temp, PRM("%02X "), text[i]);
		Write(temp);
	}

	Flush();
}


void Log::WriteTime()
{
	char time[40];
	Utils::GetTimeString(time);

	if (strlen(time) > 0)
	{
		Write(time);
		Write(" ");
	}
}

