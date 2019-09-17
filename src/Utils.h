/*
	Utils.h

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
	#include "DetectBoard.h"

	#ifdef ARDJACK_ESP32
		//#include <uinstd.h>
	#else
		#ifdef ARDJACK_ARM
			// should use uinstd.h to define sbrk but Due causes a conflict
			extern "C" char* sbrk(int incr);
		#else
			extern char *__brkval;
		#endif
	#endif

	#ifdef ARDJACK_NETWORK_AVAILABLE
		#include "WiFiLibrary.h"
	#endif
#else
	#include "stdafx.h"
#endif

#include "DateTime.h"
#include "Globals.h"
#include "StringList.h"

#ifdef ARDUINO
	struct MemoryInfo
	{
		uint32_t FreeRAM;
		uint32_t HeapFree;
		uint32_t HeapMax;
		uint32_t HeapUsed;
		uint32_t MallocMargin;
		void* RAMBase;
		uint32_t RAMSize;
		void* SP;
		void* StackStart;
	};
#endif

	static const uint8_t _DaysInMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	static const char* _ShortMonthNames = "JanFebMarAprMayJunJulAugSepOctNovDec";


class Utils
{
protected:
	static char Buffer_Bool2[6];
	static char Buffer_Int2String[10];

public:
	static void AddTime_Ms(DateTime* time, long interval);
	static char* Bool210(bool value);
	static char* Bool210(bool value, char* result);
	static char* Bool2yesno(bool value);
	static char* Bool2yesno(bool value, char* result);
	static bool CanTrim(const char* text);
	static bool CheckMem(bool logIt = false, const char* caption = "");
	static bool CompareStrings(const char* value1, int type, const char* value2, bool ignoreCase);
	static int CompareTimes(DateTime* time1, DateTime* time2);
	static void CopyTime(DateTime* src, DateTime* dest);
	//static int DaysInMonth(int month, int year);
	static bool DecodeNetworkPath(const char* path, char* computer, char* resource, bool quiet = false);
	static void DelayMs(int delay_ms);
	static void DelayUs(int delay_us);
	static bool DoBeep(int freq_hz = 1000, int dur_ms = 200);
	static char* EncodeNetworkPath(char* out, const char* computer, const char* resource);
	static const char* FindFirstNonWhitespace(const char* text);
	static char* FindNextMulti(const char* text, char chars[], int count, int* index);
	static char* FormatTime(char* value, DateTime* dt, const char* format);
	static void GetArgs(const char* text, char* firstPart, const char** remainder, char separator = ' ');
	static char* GetDateString(char* date, const char* format = NULL, bool utc = false);
	static bool GetFirstField(char* pSrc, char separator, char* result, int maxSize, bool trim = true);
	static char* GetNow(char* datetime, const char* format = NULL, bool utc = false);
	static char* GetTimeString(char* time, const char* format = NULL, bool utc = false);
	static unsigned long HostID_To_Address(const char* s);
	static const char* Int2String(long value, int radix = 10);
	//static bool IsLeapYear(int year);
	static bool IsWhite(char ch);
	static bool IsWhitespace(char ch);
	static bool JoinFields(char* line, char separator, StringList* fields, int start, int count);
	static bool JoinFieldsOld(char* line, char separator, char fields[][ARDJACK_MAX_VALUE_LENGTH], int start, int count);
	static char* MacAddressToString(uint8_t *mac, char *out);
	static int MaxInt(int i, int j);
	static void MemFree(void* block);
	static void* MemMalloc(size_t size);
	static int MinInt(int i, int j);
	static int Nint(double value);
	static bool Now(DateTime* now, bool utc = false);
	static long NowMs();
	static char* RepeatChar(char *text, char ch, int count);
	static DateTime* SecondsToTime(long seconds, DateTime* dt);
	static bool SetDate(const char* text);
	static bool SetTime(const char* text);
	static bool SplitAtNumber(const char* text, char *name, int *number, int default_Value);
	static int SplitText(const char* text, char separator, StringList* fields, int maxCount, int maxSize,
		bool trimFields = true);
	static int SplitText2Array(const char* text, char separator, char fields[][ARDJACK_MAX_VALUE_LENGTH], int maxCount,
		int maxLength, bool trimFields = true);
	static bool String2Bool(const char* text, bool defValue = false);
	static int String2Comparison(const char* name, int defValue = ARDJACK_STRING_COMPARE_TRUE, bool quiet = false);
	static double String2Double(const char* text, float defValue = 0.0);
	static float String2Float(const char* text, float defValue = 0.0);
	static int String2Int(const char* text, int defValue = 0);
	static long String2Long(const char* text, long defValue = 0);
	static char* StringComparisonTypeName(int type, char* out);
	static bool StringContains(const char* value1, const char* value2, bool ignoreCase = true);
	static bool StringEndsWith(const char* value1, const char* value2, bool ignoreCase = true);
	static bool StringEquals(const char* value1, const char* value2, bool ignoreCase = true);
	static char* StringReplace(const char* source, const char* str1, const char* str2,
		bool ignoreCase, char* out, int size);
	static char* StringReplaceMultiHex(const char* source, char chars[], int replacements[], int count, char* out, int size);
	static bool StringStartsWith(const char* value1, const char* value2, bool ignoreCase = true);
	static char* TimeToDateString(DateTime* dt, char *out, const char* format = "%d/%d/%d");
	static char* TimeToString(DateTime* dt, char *out, const char* format = "%d/%d/%d %02d:%02d:%02d");
	static char* TimeToTimeString(DateTime* dt, char *out, const char* format = "%02d:%02d:%02d.%03d");
	static char *Trim(char *text);

#ifdef ARDUINO
	static size_t GetFreeRAM();
	static bool GetMemoryInfo(MemoryInfo* memInfo);

	#ifdef ARDJACK_NETWORK_AVAILABLE
		static char* IpAddressToString(IPAddress ip, char *out);
	#endif
#else
	static bool CreateFolder(const char* folder);
	static bool FileExists(const char* filename);
	static bool FolderExists(const char* folder);
	static char* GetAppDocsFolder(char* folder, int size);
	static bool GetComputerInfo(char* hostName, char* ipAddress);
	static bool GetTimeOfDay(struct timeval* tp, struct timezone* tzp);
	static char* GetUserDocsFolder(char* folder, int size);
	static bool InitializeWinsock();
	//static void PostString(HWND hwnd, const char* text);
	static bool TerminateWinsock();
#endif

};

