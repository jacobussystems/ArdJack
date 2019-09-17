/*
	Utils.cpp

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

#include "Globals.h"

#ifdef ARDUINO
	extern uint32_t __get_MSP(void);
	#include "MemoryFreeExt.h"
#else
	#include "stdafx.h"

	#include <windows.h>
	#include <math.h>
	#include <shlobj.h>
	#include <dbghelp.h>

	// Link to shell32.lib and Ws2_32.lib.
	#pragma comment(lib, "shell32.lib")
	#pragma comment(lib, "Ws2_32.lib")
#endif

#include <time.h>

#include "IoTClock.h"
#include "Log.h"
//#include "UrlEncoder.h"
#include "Utils.h"

#ifdef ARDUINO
	#ifdef ARDJACK_RTC_AVAILABLE
		#include "RtcClock.h"
	#else
		#include "ArduinoClock.h"
	#endif
#else
	#include "WinClock.h"
#endif

typedef int64_t timestamp_t;

char Utils::Buffer_Bool2[];
char Utils::Buffer_Int2String[];



	void Utils::AddTime_Ms(DateTime* time, long interval)
	{
		if ((time->Milliseconds < 0) || (time->Seconds < 0) || (time->Minutes < 0) || (time->Hours < 0))
		{
			Log::LogError(PRM("Utils::AddTime_Ms: Bad time value"));
			return;
		}

		// Adjust the milliseconds.
		long milliseconds = time->Milliseconds + interval;

		if (milliseconds < 1000)
		{
			time->Milliseconds = milliseconds;
			return;
		}

		int seconds = milliseconds / 1000;
		time->Milliseconds = milliseconds - seconds * 1000;

		// Bump the seconds.
		int newSeconds = time->Seconds + seconds;

		if (newSeconds < 60)
		{
			time->Seconds = newSeconds;
			return;
		}

		int minutes = newSeconds / 60;
		time->Seconds = newSeconds - minutes * 60;

		// Bump the minutes.
		int newMinutes = time->Minutes + minutes;

		if (newMinutes < 60)
		{
			time->Minutes = newMinutes;
			return;
		}

		int hours = newMinutes / 60;
		time->Minutes = newMinutes - hours * 60;

		// Bump the hours.
		int newHours = time->Hours + hours;

		if (newHours < 24)
		{
			time->Hours = newHours;
			return;
		}

		int days = newHours / 24;
		time->Hours = newHours - days * 24;

		// Bump the day.
		struct tm t = { };
		t.tm_year = time->Year - 1900;
		t.tm_mon = time->Month;
		t.tm_mday = time->Day;

		t.tm_mday += days;
		mktime(&t);

		time->Day = t.tm_mday;
		time->Month = t.tm_mon;
		time->Year = 1900 + t.tm_year;
	}


	char* Utils::Bool210(bool value)
	{
		return Bool210(value, Utils::Buffer_Bool2);
	}


	char* Utils::Bool210(bool value, char* result)
	{
		strcpy(result, value ? "1" : "0");
		return result;
	}


	char* Utils::Bool2yesno(bool value)
	{
		return Bool2yesno(value, Utils::Buffer_Bool2);
	}


	char* Utils::Bool2yesno(bool value, char* result)
	{
		strcpy(result, value ? "yes" : "no");
		return result;
	}


	bool Utils::CanTrim(const char* text)
	{
		int count = strlen(text);

		if (count == 0)
			return false;

		return IsWhite(text[0]) || IsWhite(text[count - 1]);
	}


	bool Utils::CheckMem(bool logIt, const char* caption)
	{
#ifdef ARDUINO
#ifdef ARDJACK_ESP32

#else
		size_t freeRAM = GetFreeRAM();

		if (freeRAM < 1024)
		{
			Log::LogWarningF(PRM("Utils::CheckMem: ('%s') Free RAM = %d bytes"), caption, freeRAM);
			return false;
		}

		if (logIt)
			Log::LogInfoF(PRM("Utils::CheckMem: ('%s') Free RAM = %d bytes"), caption, freeRAM);
#endif
#endif

		return true;
	}


	bool Utils::CompareStrings(const char* value1, int type, const char* value2, bool ignoreCase)
	{
		switch (type)
		{
		case ARDJACK_STRING_COMPARE_FALSE:
			return false;

		case ARDJACK_STRING_COMPARE_TRUE:
			return true;
		}

		bool result = false;

		switch (type)
		{
		case ARDJACK_STRING_COMPARE_CONTAINS:
		case ARDJACK_STRING_COMPARE_NOT_CONTAINS:
			// Does 'value1' contain 'value2'?
			result = StringContains(value1, value2, ignoreCase);
			break;

		case ARDJACK_STRING_COMPARE_ENDS_WITH:
		case ARDJACK_STRING_COMPARE_NOT_ENDS_WITH:
			// Does 'value1' end with 'value2'?
			result = StringEndsWith(value1, value2, ignoreCase);
			break;

		case ARDJACK_STRING_COMPARE_EQUALS:
		case ARDJACK_STRING_COMPARE_NOT_EQUALS:
			// Does 'value1' equal 'value2'?
			result = StringEquals(value1, value2, ignoreCase);
			break;

		case ARDJACK_STRING_COMPARE_IN:
		case ARDJACK_STRING_COMPARE_NOT_IN:
			// Is 'value1' within 'value2'?
			result = StringContains(value2, value1, ignoreCase);
			break;

		case ARDJACK_STRING_COMPARE_STARTS_WITH:
		case ARDJACK_STRING_COMPARE_NOT_STARTS_WITH:
			// Does 'value1' start with 'value2'?
			result = StringStartsWith(value1, value2, ignoreCase);
			break;
		}

		// Should we invert the result?
		bool invert = false;

		switch (type)
		{
		case ARDJACK_STRING_COMPARE_NOT_CONTAINS:
		case ARDJACK_STRING_COMPARE_NOT_ENDS_WITH:
		case ARDJACK_STRING_COMPARE_NOT_EQUALS:
		case ARDJACK_STRING_COMPARE_NOT_IN:
		case ARDJACK_STRING_COMPARE_NOT_STARTS_WITH:
			// Yes.
			invert = true;
			break;
		}

		if (invert)
			result = !result;

		return result;
	}


	int Utils::CompareTimes(DateTime* time1, DateTime* time2)
	{
		// Return -1 if 'time1' < 'time2'.
		// Return 0 if 'time1' = 'time2'.
		// Return 1 if 'time1' > 'time2'.

		if (time1->Year < time2->Year) return -1;
		if (time1->Year > time2->Year) return 1;

		if (time1->Month < time2->Month) return -1;
		if (time1->Month > time2->Month) return 1;

		if (time1->Day < time2->Day) return -1;
		if (time1->Day > time2->Day) return 1;

		if (time1->Hours < time2->Hours) return -1;
		if (time1->Hours > time2->Hours) return 1;

		if (time1->Minutes < time2->Minutes) return -1;
		if (time1->Minutes > time2->Minutes) return 1;

		if (time1->Seconds < time2->Seconds) return -1;
		if (time1->Seconds > time2->Seconds) return 1;

		if (time1->Milliseconds < time2->Milliseconds) return -1;
		if (time1->Milliseconds > time2->Milliseconds) return 1;

		return 0;
	}


	void Utils::CopyTime(DateTime* src, DateTime* dest)
	{
		dest->Day = src->Day;
		dest->Month = src->Month;
		dest->Year = src->Year;
		dest->Hours = src->Hours;
		dest->Minutes = src->Minutes;
		dest->Seconds = src->Seconds;
		dest->Milliseconds = src->Milliseconds;
	}


#ifdef ARDUINO
#else
	bool Utils::CreateFolder(const char* folder)
	{
		int status = SHCreateDirectoryExA(NULL, folder, NULL);

		switch (status)
		{
		case ERROR_ALREADY_EXISTS:
		case ERROR_FILE_EXISTS:
			Log::LogInfo(PRM("Folder exists: "), folder);
			break;

		case ERROR_SUCCESS:
			Log::LogInfo(PRM("Folder created: "), folder);
			break;

		default:
			// Failed.
			char temp[202];
			sprintf(temp, PRM("Failed to create folder: %s - code %d"), folder, status);
			Log::LogError(temp);
			return false;
		}

		return true;
	}
#endif


	//int Utils::DaysInMonth(int month, int year)
	//{
	//	if ((month < 0) || (month > 11))
	//		return 0;

	//	int result = _DaysInMonth[month];

	//	// February in a Leap year?
	//	if ((month == 1) && IsLeapYear(year))
	//		result = 29;

	//	return result;
	//}


	bool Utils::DecodeNetworkPath(const char* path, char* computer, char* resource, bool quiet)
	{
		// Based on UNC syntax, i.e.
		//		\\host-name\share-name\file_path
		//
		// For now, we're just using:
		//		\\computer\resource
		// where 'resource' is usually an object name.
		//
		// Any text after 'resource' will be ignored.

		computer[0] = NULL;
		resource[0] = NULL;

		if (strlen(path) == 0)
			return true;

		if (Utils::StringStartsWith(path, "\\\\"))
		{
			// Assume this is one of:
			//		\\computer\resource
			//		\\computer

			char temp[80];
			strcpy(temp, path + 2);
			Utils::Trim(temp);

			// Is there a resource part?
			const char* ptrResource = strchr(temp, '\\');

			if (NULL == ptrResource)
			{
				// No resource.
				strcpy(computer, temp);
				Utils::Trim(computer);
			}
			else
			{
				ptrResource++;
				int nCopy = ptrResource - temp - 1;
				strncpy(computer, temp, nCopy);
				computer[nCopy] = NULL;
				Utils::Trim(computer);

				if (strlen(ptrResource) > 0)
				{
					strcpy(resource, ptrResource);
					Utils::Trim(resource);
				}
			}

			return true;
		}

		if (Utils::StringStartsWith(path, "\\"))
		{
			// Assume this is:
			//		\resource

			char temp[ARDJACK_MAX_NAME_LENGTH];
			strcpy(temp, path + 1);
			Utils::Trim(temp);

			strcpy(resource, temp);

			return true;
		}

		if (!quiet)
			Log::LogError(PRM("Invalid network path: '"), path, "'");

		return false;
	}


	void Utils::DelayMs(int delay_ms)
	{
#ifdef ARDUINO
		delay(delay_ms);
#else
		Sleep(delay_ms);
#endif
	}


	void Utils::DelayUs(int delay_us)
	{
#ifdef ARDUINO
		delayMicroseconds(delay_us);
#else
		Sleep(delay_us / 1000);
#endif
	}


	bool Utils::DoBeep(int freq_hz, int dur_ms)
	{
		if (Globals::EnableSound)
		{
#ifdef ARDUINO
#else
			Beep(freq_hz, dur_ms);
#endif
		}

		return true;
	}


	char* Utils::EncodeNetworkPath(char* out, const char* computer, const char* resource)
	{
		// Based on UNC syntax, i.e.
		//		\\host-name\share-name\file_path
		//
		// For now, we're just using:
		//		\\computer\resource
		// where 'resource' is usually an object name.

		sprintf(out, "\\\\%s\\%s", computer, resource);
		return out;
	}


#ifdef ARDUINO
#else
	bool Utils::FileExists(const char* filename)
	{
		// TEMPORARY:
		FILE* f = fopen(filename, "r");

		return (NULL != f);
	}
#endif


	const char* Utils::FindFirstNonWhitespace(const char* text)
	{
		const char* result = text;

		for (int i = 0; i < strlen(text); i++)
		{
			if (!IsWhitespace(text[i]))
				return result;

			result++;
		}

		// 'text' is all whitespace.
		return text;
	}

	
	char* Utils::FindNextMulti(const char* text, char chars[], int count, int* index)
	{
		char* inptr = (char*)text;

		for (int i = 0; i < strlen(text); i++)
		{
			char c = text[i];

			for (int j = 0; j < count; j++)
			{
				if (c == chars[j])
				{
					*index = j;
					return inptr;
				}
			}

			inptr++;
		}

		return NULL;
	}


#ifdef ARDUINO
#else
	bool Utils::FolderExists(const char* folder)
	{

		return false;
	}
#endif


	char* Utils::FormatTime(char* value, DateTime* dt, const char* format)
	{
		if (Utils::StringEquals(format, "date"))
		{
			sprintf(value, PRM("%d/%d/%d"), dt->Day, dt->Month, dt->Year);
			return value;
		}

		if (Utils::StringEquals(format, "time"))
		{
			sprintf(value, PRM("%02d:%02d:%02d.%03d"), dt->Hours, dt->Minutes, dt->Seconds, dt->Milliseconds);
			return value;
		}

		strcpy(value, format);
		char temp[20];

		if (NULL != strstr(value, "dd"))
		{
			sprintf(temp, PRM("%d"), dt->Day);
			Utils::StringReplace(value, "dd", temp, false, value, 202);
		}

		if (NULL != strstr(value, "HH"))
		{
			sprintf(temp, PRM("%02d"), dt->Hours);
			Utils::StringReplace(value, "HH", temp, false, value, 202);
		}

		if (NULL != strstr(value, "mm"))
		{
			sprintf(temp, PRM("%02d"), dt->Minutes);
			Utils::StringReplace(value, "mm", temp, false, value, 202);
		}

		if (NULL != strstr(value, "MMM"))
		{
			char monthName[4];
			strncpy(monthName, _ShortMonthNames + ((dt->Month - 1) * 3), 3);
			monthName[3] = NULL;

			Utils::StringReplace(value, "MMM", monthName, false, value, 202);
		}
		else if (NULL != strstr(value, "MM"))
		{
			sprintf(temp, PRM("%d"), dt->Month);
			Utils::StringReplace(value, "MM", temp, false, value, 202);
		}

		if (NULL != strstr(value, "ss"))
		{
			sprintf(temp, PRM("%02d"), dt->Seconds);
			Utils::StringReplace(value, "ss", temp, false, value, 202);
		}

		if (NULL != strstr(value, "fff"))
		{
			sprintf(temp, PRM("%03d"), dt->Milliseconds);
			Utils::StringReplace(value, "fff", temp, false, value, 202);
		}
		else if (NULL != strstr(value, "ff"))
		{
			sprintf(temp, PRM("%02d"), dt->Milliseconds / 10);
			Utils::StringReplace(value, "ff", temp, false, value, 202);
		}
		else if (NULL != strstr(value, "f"))
		{
			sprintf(temp, PRM("%1d"), dt->Milliseconds / 100);
			Utils::StringReplace(value, "f", temp, false, value, 202);
		}

		if (NULL != strstr(value, "yyyy"))
		{
			sprintf(temp, PRM("%d"), dt->Year);
			Utils::StringReplace(value, "yyyy", temp, false, value, 202);
		}
		else if (NULL != strstr(value, "yy"))
		{
			sprintf(temp, PRM("%d"), dt->Year % 1000);
			Utils::StringReplace(value, "yy", temp, false, value, 202);
		}

		return value;
	}


#ifdef ARDUINO
#else
	char* Utils::GetAppDocsFolder(char* folder, int size)
	{
		Utils::GetUserDocsFolder(folder, MAX_PATH);
		if (NULL == folder) return NULL;

		strcat(folder, "\\Jacobus Systems\\");
		strcat(folder, Globals::AppName);

		if (Globals::Verbosity > 3)
			Log::LogInfo(PRM("App Documents folder: "), folder);

		if (!CreateFolder(folder))
			return NULL;

		return folder;
	}
#endif


	void Utils::GetArgs(const char* text, char* firstPart, const char** remainder, char separator)
	{
		// Copy the first argument in 'text' into 'firstPart', and return a pointer to the remaining text in 'remainder'.
		firstPart[0] = NULL;
		*remainder = NULL;

		// Start at the first non-whitespace character.
		const char* start = FindFirstNonWhitespace(text);

		if (NULL == start)
		{
			// 'text' is all non-whitespace.
			strcpy(firstPart, text);
			return;
		}

		const char* ptr = strchr(start, (int)separator);

		if (NULL == ptr)
		{
			// Separator not found.
			strcpy(firstPart, start);
			return;
		}

		if (ptr > text)
		{
			// Copy the preceding part of 'text' to 'firstPart'.
			int count = ptr - text;
			strncpy(firstPart, text, count);
			firstPart[count] = NULL;
			Trim(firstPart);
		}

		*remainder = FindFirstNonWhitespace(ptr + 1);
	}


#ifdef ARDUINO

#else
	bool Utils::GetComputerInfo(char* hostName, char* ipAddress)
	{
		hostName[0] = NULL;
		ipAddress[0] = NULL;

		char name[ARDJACK_MAX_NAME_LENGTH];
		char temp[80];

		if (gethostname(name, sizeof(name)) == SOCKET_ERROR)
		{
			sprintf(temp, PRM("Error %d when getting local host name."), WSAGetLastError());
			Log::LogError(temp);
			return false;
		}

		strcpy(hostName, name);
		Log::LogInfo("Host name: ", name);

		struct hostent *phe = gethostbyname(name);
		if (phe == 0)
		{
			sprintf(temp, PRM("Host lookup failed for '%s'."), name);
			Log::LogError(temp);
			return false;
		}

		for (int i = 0; phe->h_addr_list[i] != 0; ++i)
		{
			struct in_addr addr;
			memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));

			char ip[32];
			strcpy(ip, inet_ntoa(addr));
			sprintf(temp, PRM("IP address[%d]: %s"), i, ip);
			if (i > 0)
				strcat(ipAddress, " ");
			strcat(ipAddress, ip);
		}

		return true;
	}
#endif


	char* Utils::GetDateString(char* result, const char* format, bool utc)
	{
		result[0] = NULL;

		int day;
		int month;
		int year;

		Globals::Clock->GetDate(&day, &month, &year, utc);

		sprintf(result, "%d/%d/%d", day, month, year);

		return result;
	}


	bool Utils::GetFirstField(char* pSrc, char separator, char* result, int maxSize, bool trim)
	{
		// DESTRUCTIVE function to remove first field from 'pSrc', returning it via 'result'.
		// Fields may be delimited by matching double-quotes or single-quotes, and optionally trimmed.
		*result = NULL;

		//if (separator == ' ')
		Utils::Trim(pSrc);

		switch (pSrc[0])
		{
		case NULL:
			return false;

		case '"':
			// The next field starts with a double-quote.
		{
			// Find the next double-quote.
			char* next1 = strchr(pSrc + 1, '"');

			if (NULL == next1)
			{
				// No more double-quotes.
				strcpy(result, pSrc);				// return the remainder of the source string
				if (trim)
					Utils::Trim(result);
				*pSrc = NULL;						// clear the source string
				return true;
			}

			int index1 = next1 - pSrc - 1;
			strncpy(result, pSrc + 1, index1);		// get the start of the string up to the double-quote
			result[index1] = NULL;

			if (trim)
				Trim(result);

			strcpy(pSrc, next1 + 1);				// remove everything up to and including the double quote

			return (NULL != *result);
		}

		case '\'':
			// The next field starts with a single-quote.
		{
			// Find the next single-quote.
			char* next2 = strchr(pSrc + 1, '\'');

			if (NULL == next2)
			{
				// No more single-quotes.
				strcpy(result, pSrc);				// return the remainder of the source string
				if (trim)
					Utils::Trim(result);
				*pSrc = NULL;						// clear the source string
				return true;
			}

			int index2 = next2 - pSrc - 1;
			strncpy(result, pSrc + 1, index2);		// get the start of the string up to the single-quote
			result[index2] = NULL;

			if (trim)
				Trim(result);

			strcpy(pSrc, next2 + 1);				// remove everything up to and including the single quote

			return (NULL != *result);
		}
		}

		while (true)
		{
			char* first = strchr(pSrc, separator);		// find the first occurrence of the separator

			if (NULL == first)
			{
				// Separator was not found.
				strcpy(result, pSrc);					// return the remainder of the source string
				if (trim)
					Trim(result);
				*pSrc = NULL;							// clear the source string
				return true;
			}

			int index = first - pSrc;

			if (index == 0)
				* result = NULL;
			else
			{
				strncpy(result, pSrc, index);			// get the start of the string up to the separator
				result[index] = NULL;
			}

			strcpy(pSrc, first + 1);					// remove everything up to and including the separator

			if (index > 0)
				break;
		}

		if (NULL == result[0])
			return false;

		if (trim)
			Trim(result);

		return true;
	}


#ifdef ARDUINO
	size_t Utils::GetFreeRAM()
	{
		// Returns "how much memory is unassigned in the gap between the stack and the heap"
		// from: http://www.sumidacrossing.org/Musings/files/160606_Memory_and_the_Arduino.php

		size_t result = 0;

#ifdef ARDJACK_ARM
		result = (char*)&result - sbrk(0);
#endif

		return result;
	}


	bool Utils::GetMemoryInfo(MemoryInfo* memInfo)
	{
		int top;

		memInfo->FreeRAM = GetFreeRAM();
		memInfo->RAMSize = ARDJACK_SRAM * ARDJACK_BYTES_PER_KB;
		memInfo->SP = (void*)&top;

		GetHeapInfo(memInfo);

#ifdef ARDJACK_ESP32
		memInfo->MallocMargin = 0;	// __malloc_margin;
		memInfo->RAMBase = (void*)0x30000000;
		memInfo->StackStart = (void*)0x3fffffff;
#else
		#ifdef ARDJACK_ARM
			memInfo->MallocMargin = 0;	// __malloc_margin;
			memInfo->RAMBase = (void*)0x20000000;
			memInfo->StackStart = (void*)0x20007fff;
		#else
			memInfo->MallocMargin = 0;	// __malloc_margin;
			memInfo->RAMBase = (void*)0x0000;
			memInfo->StackStart = (void*)0x10ff;
		#endif
#endif

		return true;
	}
#endif


char* Utils::GetNow(char* result, const char* format, bool utc)
{
	result[0] = NULL;

	GetDateString(result, "", utc);
	strcat(result, " ");

	char time[20];
	strcat(result, GetTimeString(time, "", utc));

	return result;
}


char* Utils::GetTimeString(char* result, const char* format, bool utc)
{
	result[0] = NULL;

	if (NULL == Globals::Clock)
		strcpy(result, "");
	else
	{
		int hours;
		int milliseconds;
		int minutes;
		int seconds;

		Globals::Clock->GetTime(&hours, &minutes, &seconds, &milliseconds, utc);
		sprintf(result, "%02d:%02d:%02d.%03d", hours, minutes, seconds, milliseconds);
	}

	return result;
}


#ifdef ARDUINO
#else
	char* Utils::GetUserDocsFolder(char* folder, int size)
	{
		folder[0] = NULL;

		HRESULT ret = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, folder);
		if (ret != S_OK)
		{
			Log::LogError(PRM("Failed to get User's Documents folder"));
			return NULL;
		}

		if (Globals::Verbosity > 3)
			Log::LogInfo(PRM("User Documents folder: "), folder);

		if (!CreateFolder(folder))
			return NULL;

		return folder;
	}
#endif


unsigned long Utils::HostID_To_Address(const char* s)
{
#ifdef ARDUINO
	return 0;
#else
	struct hostent *host;

	// Assume we have a dotted IP address.
	long result = inet_addr(s);

	if (result != (long)INADDR_NONE)
		return result;

	// That failed so assume DNS will resolve it.
	host = gethostbyname(s);

	return host ? *((long *)host->h_addr_list[0]) : INADDR_NONE;
#endif
}


#ifdef ARDUINO

#else
	bool Utils::InitializeWinsock()
	{
		//if (Globals::WinsockStarted)
		//	return true;

		// Initialize winsock.
		WSADATA wsaData;
		char temp[102];

		int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (err != 0)
		{
			sprintf(temp, PRM("Failed to initialize Winsock - ERROR CODE: %d"), err);
			Log::LogError(temp);
			return false;
		}

		Globals::WinsockStarted = true;

		sprintf(temp, PRM("Winsock started: %s - %s"), wsaData.szDescription, wsaData.szSystemStatus);
		Log::LogInfo(temp);

		return true;
	}
#endif


const char* Utils::Int2String(long value, int radix)
{
	ltoa(value, Buffer_Int2String, radix);

	return Buffer_Int2String;
}


#ifdef ARDUINO
	#ifdef ARDJACK_NETWORK_AVAILABLE
		char* Utils::IpAddressToString(IPAddress ip, char *out)
		{
			sprintf(out, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

			return out;
		}
	#endif
#endif


	//bool Utils::IsLeapYear(int year)
	//{
	//	// TEMPORARY:
	//	return false;
	//}


	bool Utils::IsWhite(char c)
	{
		if (NULL == c) return false;

		return (c <= ' ') || iscntrl(c);
	}


	bool Utils::IsWhitespace(char c)
	{
		if (NULL == c) return false;

		return (c == '\t' || c == '\n' || c == ' ');
	}


	bool Utils::JoinFields(char* line, char separator, StringList* fields, int start, int count)
	{
		line[0] = NULL;

		char field[ARDJACK_MAX_VALUE_LENGTH];

		char sep[2];
		sep[0] = separator;
		sep[1] = NULL;

		for (int i = start; i <= start + count - 1; i++)
		{
			strcpy(field, fields->Get(i));

			if (i > start)
				strcat(line, sep);

			strcat(line, field);
		}

		return true;
	}


	bool Utils::JoinFieldsOld(char* line, char separator, char fields[][ARDJACK_MAX_VALUE_LENGTH], int start, int count)
	{
		line[0] = NULL;

		char field[ARDJACK_MAX_VALUE_LENGTH];

		char sep[2];
		sep[0] = separator;
		sep[1] = NULL;

		for (int i = start; i <= start + count - 1; i++)
		{
			strcpy(field, fields[i]);

			if (i > start)
				strcat(line, sep);

			strcat(line, field);
		}

		return true;
	}


	char* Utils::MacAddressToString(uint8_t* mac, char* out)
	{
		// Depends on the processor endianness?
#ifdef ARDJACK_ESP32
		sprintf(out, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
#else
		sprintf(out, "%02X:%02X:%02X:%02X:%02X:%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
#endif

		return out;
	}


	int Utils::MaxInt(int i, int j)
	{
		if (i >= j)
			return i;
		else
			return j;
	}


	void Utils::MemFree(void* block)
	{
		if (NULL == block)
		{
			Log::LogError(PRM("Utils::MemFree: Block @ NULL?"));
			return;
		}

		if (Globals::Verbosity > 9)
		{
			char temp[20];
			sprintf(temp, PRM("@ %p"), block);
			Log::LogInfo(PRM("Utils::MemFree: "), temp);
		}

		free(block);
	}


	void* Utils::MemMalloc(size_t size)
	{
		void* result = malloc(size);

		if (NULL == result)
		{
			// Failed.
			Log::LogErrorF(PRM("Utils::MemMalloc: Failed to get %d bytes"), size);
			return NULL;
		}

		// Success.
		if (Globals::Verbosity > 9)
			Log::LogInfoF(PRM("Utils::MemMalloc: %d bytes @ %p"), size, result);

		return result;
	}


	int Utils::MinInt(int i, int j)
	{
		if (i <= j)
			return i;
		else
			return j;
	}


	int Utils::Nint(double value)
	{
		// Return the nearest integer value to 'value'.
		if (value >= 0)
			return (int)floor(value + 0.5);
		else
			return (int)floor(value - 0.5) + 1;
	}


	bool Utils::Now(DateTime* now, bool utc)
	{
		if (utc)
			return Globals::Clock->NowUtc(now);
		else
			return Globals::Clock->Now(now);
	}


	long Utils::NowMs()
	{
		return Globals::Clock->NowMs();
	}


#ifdef ARDUINO
#else

	//void Utils::PostString(HWND hwnd, const char* text)
	//{
	//	// Sends a string to the specified window.
	//	const char* ptr = text;

	//	while (*ptr)
	//	{
	//		PostMessage(hwnd, WM_CHAR, (unsigned char)* ptr, 1L);
	//		// Should check the return code on PostMessage.
	//		// If 0, the message queue was full and the message wasn't posted.

	//		ptr++;
	//	}
	//}

#endif


	char* Utils::RepeatChar(char* text, char ch, int count)
	{
		char* ptr = text;

		for (int i = 0; i < count; i++)
			* ptr++ = ch;

		*ptr = NULL;

		return text;
	}


	DateTime* Utils::SecondsToTime(long seconds, DateTime * dt)
	{
		// Convert 'seconds', the number of seconds since January 1st, 1970, to 'dt'.
		// E.g. 1542914280 -> 22 Nov 2018.

		time_t timestamp = seconds;

#ifdef ARDUINO
		struct tm tmTemp;
		gmtime_r((const time_t*)& timestamp, &tmTemp);

		dt->Day = tmTemp.tm_mday;
		dt->Month = tmTemp.tm_mon + 1;
		dt->Year = tmTemp.tm_year + 1900;
		dt->Hours = tmTemp.tm_hour;
		dt->Minutes = tmTemp.tm_min;
		dt->Seconds = tmTemp.tm_sec;
		dt->Milliseconds = 0;
#else
		tm* tmTemp = gmtime((const time_t*)& timestamp);

		dt->Day = tmTemp->tm_mday;
		dt->Month = tmTemp->tm_mon + 1;
		dt->Year = tmTemp->tm_year + 1900;
		dt->Hours = tmTemp->tm_hour;
		dt->Minutes = tmTemp->tm_min;
		dt->Seconds = tmTemp->tm_sec;
		dt->Milliseconds = 0;
#endif

		return dt;
	}


	bool Utils::SetDate(const char* text)
	{
		char temp[80];

		// First, get the date into 'day', 'month' and 'year' in human form (1-31 / 1-12 / yyyy).

		// Default to 1 Jan 2019.
		int day = 1;
		int month = 1;
		int year = 2019;

		// Guess the date format.
		if (isalpha(text[0]))
		{
			// Assume the format is 'Aug 12 2019'.
			char shortMonthNamesUpper[40];
			strcpy(shortMonthNamesUpper, _ShortMonthNames);
			_strupr(shortMonthNamesUpper);

			strncpy(temp, text, 3);
			temp[3] = NULL;
			_strupr(temp);
			const char* ptr = strstr(shortMonthNamesUpper, temp);

			if (NULL != ptr)
				month = (ptr - shortMonthNamesUpper) / 3 + 1;

			strncpy(temp, text + 4, 2);
			temp[2] = NULL;
			day = atoi(temp);

			strncpy(temp, text + 7, 4);
			temp[4] = NULL;
			year = atoi(temp);
		}
		else
			sscanf(text, "%d/%d/%d", &day, &month, &year);

		Log::LogInfoF(PRM("Utils::SetDate: %d/%d/%d"), day, month, year);

#ifdef ARDUINO
		return Globals::Clock->SetDate(day, month, year);
#else
		return false;
#endif
	}


	bool Utils::SetTime(const char* text)
	{
		int hours = 12;
		int minutes = 0;
		int seconds = 0;

		sscanf(text, "%d:%d:%d", &hours, &minutes, &seconds);

		Log::LogInfoF(PRM("Utils::SetTime: %02d:%02d:%02d"), hours, minutes, seconds);

#ifdef ARDUINO
		return Globals::Clock->SetTime(hours, minutes, seconds);
#else
		return false;
#endif
	}


	bool Utils::SplitAtNumber(const char* text, char* name, int* number, int default_Value)
	{
		// Splits a string 'text' of the form 'xyz12' into 'name', the leading alphabetic part ("xyz"),
		// and 'number', the number (12).
		// If there's no number, or it has an invalid format, 'number' is set to 'defaultValue'.

		name[0] = NULL;
		*number = default_Value;

		for (int i = 0; i < strlen(text); i++)
		{
			char ch = text[i];

			if ((ch >= '0') && (ch <= '9'))
			{
				strcpy(name, text);
				name[i] = NULL;
				const char* rest = text + i;
				*number = Utils::String2Int(rest);
				break;
			}
		}

		if (strlen(name) == 0)
			strcpy(name, text);

		return true;
	}


	int Utils::SplitText(const char* text, char separator, StringList* fields, int maxCount, int maxLength,
		bool trimFields)
	{
		// Split 'text'.
		fields->Clear();

		char useText[300];
		strcpy(useText, text);

		char field[ARDJACK_MAX_VALUE_LENGTH];
		int count = 0;

		while ((strlen(useText) > 0) && (count < maxCount))
		{
			if (!GetFirstField(useText, separator, field, maxLength, trimFields))
				break;

			fields->Add(field);
			count++;

			if (count >= maxCount - 1)
			{
				// Anything left?
				if (trimFields)
					Utils::Trim(useText);

				if (strlen(useText) > 0)
				{
					fields->Add(useText);
					count++;
				}

				break;
			}
		}

		return count;
	}


	int Utils::SplitText2Array(const char* text, char separator, char fields[][ARDJACK_MAX_VALUE_LENGTH], int maxCount,
		int maxLength, bool trimFields)
	{
		// Split 'text'.
		if ((NULL == text) || (strlen(text) == 0))
			return 0;

		char useText[300];
		strcpy(useText, text);

		int count = 0;

		while ((strlen(useText) > 0) && (count < maxCount))
		{
			if (!GetFirstField(useText, separator, fields[count++], maxLength, trimFields))
				break;

			if (count >= maxCount - 1)
			{
				// Anything left?
				if (trimFields)
					Utils::Trim(useText);

				strcpy(fields[count], useText);

				if (strlen(useText) > 0)
					count++;

				break;
			}
		}

		return count;
	}


	bool Utils::String2Bool(const char* text, bool defValue)
	{
		char useText[20];
		strcpy(useText, text);
		_strlwr(useText);

		if (StringEquals(useText, "1") || StringEquals(useText, "on") || StringEquals(useText, "t")
			|| StringEquals(useText, "true") || StringEquals(useText, "y") || StringEquals(useText, "yes"))
			return true;

		if (StringEquals(useText, "0") || StringEquals(useText, "off") || StringEquals(useText, "f")
			|| StringEquals(useText, "false") || StringEquals(useText, "n") || StringEquals(useText, "no"))
			return false;

		char temp[80];
		sprintf(temp, PRM("Utils::String2Bool: Invalid '%s' (%d chars)"), text, (int)strlen(text));
		Log::LogInfo(temp);

		return defValue;
	}


	int Utils::String2Comparison(const char* name, int defValue, bool quiet)
	{
		if (Utils::StringEquals(name, PRM("Contains")))
			return ARDJACK_STRING_COMPARE_CONTAINS;

		if (Utils::StringEquals(name, PRM("EndsWith")))
			return ARDJACK_STRING_COMPARE_ENDS_WITH;

		if (Utils::StringEquals(name, PRM("Equals")))
			return ARDJACK_STRING_COMPARE_EQUALS;

		if (Utils::StringEquals(name, PRM("False")))
			return ARDJACK_STRING_COMPARE_FALSE;

		if (Utils::StringEquals(name, PRM("In")))
			return ARDJACK_STRING_COMPARE_IN;

		if (Utils::StringEquals(name, PRM("NotContains")))
			return ARDJACK_STRING_COMPARE_NOT_CONTAINS;

		if (Utils::StringEquals(name, PRM("NotEndsWith")))
			return ARDJACK_STRING_COMPARE_NOT_ENDS_WITH;

		if (Utils::StringEquals(name, PRM("NotEquals")))
			return ARDJACK_STRING_COMPARE_NOT_EQUALS;

		if (Utils::StringEquals(name, PRM("NotIn")))
			return ARDJACK_STRING_COMPARE_NOT_IN;

		if (Utils::StringEquals(name, PRM("NotStartsWith")))
			return ARDJACK_STRING_COMPARE_NOT_STARTS_WITH;

		if (Utils::StringEquals(name, PRM("StartsWith")))
			return ARDJACK_STRING_COMPARE_STARTS_WITH;

		if (Utils::StringEquals(name, PRM("True")))
			return ARDJACK_STRING_COMPARE_TRUE;

		if (!quiet)
			Log::LogErrorF(PRM("Invalid string comparison type: '%s'"), name);

		return defValue;
	}


	double Utils::String2Double(const char* text, float defValue)
	{
		if (strlen(text) == 0)
			return defValue;

#ifdef ARDUINO
		double result = strtod(text, NULL);
#else
		errno = 0;
		double result = strtod(text, NULL);

		if (errno == ERANGE)
			return defValue;
#endif

		return result;
	}


	float Utils::String2Float(const char* text, float defValue)
	{
		return String2Double(text, defValue);
	}


	int Utils::String2Int(const char* text, int defValue)
	{
		return String2Long(text, defValue);
	}


	long Utils::String2Long(const char* text, long defValue)
	{
		if (strlen(text) == 0)
			return defValue;

#ifdef ARDUINO
		long result = strtol(text, NULL, 10);
#else
		errno = 0;
		long result = strtol(text, NULL, 10);

		if (errno == ERANGE)
			return defValue;
#endif

		return result;
	}


	char* Utils::StringComparisonTypeName(int type, char* out)
	{
		switch (type)
		{
		case ARDJACK_STRING_COMPARE_CONTAINS:
			strcpy(out, PRM("Contains"));
			break;

		case ARDJACK_STRING_COMPARE_ENDS_WITH:
			strcpy(out, PRM("Ends With"));
			break;

		case ARDJACK_STRING_COMPARE_EQUALS:
			strcpy(out, PRM("Equals"));
			break;

		case ARDJACK_STRING_COMPARE_FALSE:
			strcpy(out, PRM("False"));
			break;

		case ARDJACK_STRING_COMPARE_IN:
			strcpy(out, PRM("In"));
			break;

		case ARDJACK_STRING_COMPARE_NOT_CONTAINS:
			strcpy(out, PRM("Not Contains"));
			break;

		case ARDJACK_STRING_COMPARE_NOT_ENDS_WITH:
			strcpy(out, PRM("Not Ends With"));
			break;

		case ARDJACK_STRING_COMPARE_NOT_EQUALS:
			strcpy(out, PRM("Not Equals"));
			break;

		case ARDJACK_STRING_COMPARE_NOT_IN:
			strcpy(out, PRM("Not In"));
			break;

		case ARDJACK_STRING_COMPARE_NOT_STARTS_WITH:
			strcpy(out, PRM("Not Starts With"));
			break;

		case ARDJACK_STRING_COMPARE_STARTS_WITH:
			strcpy(out, PRM("Starts With"));
			break;

		case ARDJACK_STRING_COMPARE_TRUE:
			strcpy(out, PRM("True"));
			break;

		default:
			sprintf(out, PRM("(type %d?)"), type);
			break;
		}

		return out;
	}


	bool Utils::StringContains(const char* value1, const char* value2, bool ignoreCase)
	{
		// Does 'value1' contain 'value2'?
		if (strlen(value1) < strlen(value2)) return false;

		if (!ignoreCase)
			return (NULL != strstr(value1, value2));

		// Size?
		char lcValue1[202];
		strcpy(lcValue1, value1);
		_strlwr(lcValue1);

		// Size?
		char lcValue2[202];
		strcpy(lcValue2, value2);
		_strlwr(lcValue2);

		return (NULL != strstr(lcValue1, lcValue2));
	}


	bool Utils::StringEndsWith(const char* value1, const char* value2, bool ignoreCase)
	{
		// Does 'value1' end with 'value2'?
		if (strlen(value1) < strlen(value2))
			return false;

		if (strlen(value1) == strlen(value2))
			return StringEquals(value1, value2, ignoreCase);

		// Size?
		char end[202];
		strncpy(end, value1 + strlen(value1) - strlen(value2), strlen(value2));
		end[strlen(value2)] = NULL;

		return StringEquals(end, value2, ignoreCase);
	}


	bool Utils::StringEquals(const char* value1, const char* value2, bool ignoreCase)
	{
		// Does 'value1' equal 'value2'?
		if ((NULL == value1) || (NULL == value2)) return false;
		if (strlen(value1) != strlen(value2)) return false;

		if (!ignoreCase)
			return (strcmp(value1, value2) == 0);

#ifdef ARDUINO
		return (strcasecmp(value1, value2) == 0);
#else
		return (stricmp(value1, value2) == 0);
#endif
	}


	char* Utils::StringReplace(const char* source, const char* str1, const char* str2, bool ignoreCase, char* out, int size)
	{
		// Replace all occurrences of 'str1' in 'source' with 'str2', returning the result in 'out'.

		// TEMPORARY: Ignore 'ignoreCase' and do case-sensitive matching.
		const char* inptr = source;

		// We'll use a temporary buffer for the result, so that 'out' can be the same address as 'source',
		// i.e. this can act as 'in-place' replacement from the caller's point of view.
		char temp[202];
		char* outptr = temp;
		*outptr = NULL;

		while (true)
		{
			const char* next = strstr(inptr, str1);

			if (NULL == next)
			{
				// Copy the remainder to 'temp'.
				strcpy(outptr, inptr);
				break;
			}

			// We've found another 'str1'.

			// Copy any preceding text to 'temp'.
			if (next > inptr)
			{
				int count = next - inptr;
				strncpy(outptr, inptr, count);
				outptr += count;
				*outptr = NULL;
			}

			// Copy 'str2' to 'temp'.
			strcpy(outptr, str2);

			inptr = next + strlen(str1);
			outptr += strlen(str2);
		}

		strcpy(out, temp);

		return out;
	}


	char* Utils::StringReplaceMultiHex(const char* source, char chars[], int replacements[], int count, char* out, int size)
	{
		// Replace all occurrences of 'chars' in 'source' with the corresponding hex-encoded items from 'replacements',
		// returning the result in 'out'.
		// ASSUMPTION: case is not relevant (i.e. no letters are being replaced).

		const char* inptr = source;
		char temp[4];

		char* outptr = out;
		*outptr = NULL;
		int index;

		while (true)
		{
			const char* next = FindNextMulti(inptr, chars, count, &index);

			if (NULL == next)
			{
				// Copy the remainder to 'out'.
				strcpy(outptr, inptr);
				break;
			}

			// We've found another replacement.

			// Copy any preceding text to 'out'.
			if (next > inptr)
			{
				int count = next - inptr;
				strncpy(outptr, inptr, count);
				outptr += count;
				*outptr = NULL;
			}

			// Copy the replacement text to 'out'.
			sprintf(temp, PRM("%%%02X"), replacements[index]);
			strcpy(outptr, temp);

			inptr = next + 1;
			outptr += strlen(temp);
		}

		return out;
	}


	bool Utils::StringStartsWith(const char* value1, const char* value2, bool ignoreCase)
	{
		// Does 'value1' start with 'value2'?
		if (strlen(value1) < strlen(value2))
			return false;

		if (strlen(value1) == strlen(value2))
			return StringEquals(value1, value2, ignoreCase);

		// Size?
		char start[202];
		strncpy(start, value1, strlen(value2));
		start[strlen(value2)] = NULL;

		return StringEquals(start, value2, ignoreCase);
	}


#ifdef ARDUINO
#else
	bool Utils::TerminateWinsock()
	{
		if (Globals::WinsockStarted)
		{
			if (Globals::Verbosity > 4)
				Log::LogInfo("Utils::TerminateWinsock");

			WSACleanup();
			Globals::WinsockStarted = false;
		}

		return true;
	}
#endif


	char* Utils::TimeToDateString(DateTime * dt, char* out, const char* format)
	{
		sprintf(out, format, dt->Day, dt->Month, dt->Year);

		return out;
	}


	char* Utils::TimeToString(DateTime * dt, char* out, const char* format)
	{
		sprintf(out, format, dt->Day, dt->Month, dt->Year,
			dt->Hours, dt->Minutes, dt->Seconds);

		return out;
	}


	char* Utils::TimeToTimeString(DateTime * dt, char* out, const char* format)
	{
		sprintf(out, format, dt->Hours, dt->Minutes, dt->Seconds, dt->Milliseconds);

		return out;
	}


	char* Utils::Trim(char* text)
	{
		// DESTRUCTIVE function to remove leading and trailing white space from 'text' (including CR and LF chars).
		if (!CanTrim(text))
			return text;

		// Remove any leading white space.

		// Find the first non-whitespace char.
		char* first = text;
		char* ptr = first;

		while (IsWhite(*ptr))
			ptr++;

		if (ptr > first)
		{
			// Remove the leading white space.
			strcpy(text, ptr);
		}

		// Remove any trailing white space.
		char* last = text + strlen(text) - 1;
		ptr = last;

		while (IsWhite(*ptr))
		{
			if (--ptr < text)
			{
				ptr = NULL;
				break;
			}
		}

		if ((NULL != ptr) && (ptr < last))
		{
			// Remove the trailing white space.
			ptr++;
			*ptr = NULL;
		}

		return text;
	}

