/*
	FieldReplacer.cpp

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
	#include <typeinfo>
#endif

#include "FieldReplacer.h"
#include "Log.h"
#include "Utils.h"



FieldReplacer::FieldReplacer()
{
	strcpy(Params.DateFormat, "dd MM yyyy");
	strcpy(Params.Intro, "[");
	strcpy(Params.Outro, "]");
	Params.ReplaceSpecialFields = true;
	strcpy(Params.SpecialFieldPrefix, "");							// "$");
	strcpy(Params.TimeFormat, "HH:mm:ss");
}


const char* FieldReplacer::FindMatchingOutro(const char* text)
{
	// Find the matching 'outro' (if any).
	int count = 0;
	int j = 0;

	while (j <= strlen(text))
	{
		if (strncmp(text + j, Params.Intro, strlen(Params.Intro)) == 0)
		{
			count++;
			j += strlen(Params.Intro);
			continue;
		}

		if (strncmp(text + j, Params.Outro, strlen(Params.Outro)) == 0)
		{
			if (count == 0)
				return text + j;

			count--;
			j += strlen(Params.Outro);
			continue;
		}

		j++;
	}

	return NULL;
}


//bool FieldReplacer::ProcessEscapes_1(const char* text, char* out)
//{
//	char temp[20];
//	strcpy(temp, "\\");
//	strcat(temp, Params.Intro);
//
//	bool escapes = Utils::StringContains(text, temp, false);
//
//	if (!escapes)
//	{
//		strcpy(temp, "\\");
//		strcat(temp, Params.Outro);
//		escapes = Utils::StringContains(text, temp, false);
//	}
//
//	if (escapes)
//	{
//		// Escape(s) are present - replace '\'intro with '' and '\'outro with '', and replace them later.
//
//	}
//
//	return escapes;
//}
//
//
//bool FieldReplacer::ProcessEscapes_2(const char* text, char* out)
//{
//
//	return true;
//}


bool FieldReplacer::ReplaceFields(const char* text, Dictionary* args, ReplaceFieldCallback callback, char* value)
{
	// Assuming Intro is "[" and Outro is "]", replace fields in 'text' of the form "[name]" with the values
	// present in 'args' (or the 'special' values, e.g. 'time').

	// The result is returned in 'value'.
	value[0] = NULL;
	char temp[202];

	//// Any escapes?
	//bool escapes = ProcessEscapes_1(text, temp);

	// Do replacements.
	const char* start = text;
	const char* last = text + strlen(text);
	int count;
	char fieldValue[102];

	while (start < last)
	{
		// Look for the next 'intro' (if any).
		const char* nextIntro = strstr(start, Params.Intro);

		if (NULL == nextIntro)
		{
			// No more 'intros' in the text.
			strcat(value, start);
			break;
		}

		// We've found an 'intro'.

		// Process the text up to and including the 'Intro'.
		count = nextIntro - start;
		strncpy(temp, start, count);
		temp[count] = NULL;

		strcat(value, temp);
		start = nextIntro + strlen(Params.Intro);

		// Look for the matching 'outro' (if any).
		const char* matchingOutro = FindMatchingOutro(start);

		if (NULL == matchingOutro)
		{
			// No matching 'outro' in the text.
			Log::LogWarning(PRM("No matching '"), Params.Outro, PRM("' found: '"), text, "'");
			strcat(value, start);
			break;
		}

		// We've found a matching 'outro'.
		
		// Get the field expression.
		char fieldExpr[102];
		count = matchingOutro - start;
		strncpy(fieldExpr, start, count);
		fieldExpr[count] = NULL;

		if (strlen(fieldExpr) > 0)
		{
			// Recursive call to handle embedded fields.
			while (NULL != strstr(fieldExpr, Params.Intro))
				ReplaceFields(fieldExpr, args, callback, fieldExpr);

			// Try to get a value for this field expression.
			if (NULL != callback)
			{
				(*callback)(fieldExpr, args, fieldValue, &Params);

				if (strlen(fieldValue) > 0)
				{
					//if (NULL != NewlineReplacement)
					//{
					//	fieldValue = fieldValue.Replace(Environment.NewLine, NewlineReplacement);
					//	fieldValue = fieldValue.Replace("\n", NewlineReplacement);
					//}

					if (NULL != strstr(fieldValue, Params.Intro))
					{
						// Recursive call to handle embedded fields.
						char useValue[202];
						ReplaceFields(fieldValue, args, callback, useValue);
						strcat(value, useValue);
					}
					else
						strcat(value, fieldValue);
				}
			}
			else
			{
				// This field expression is not recognised - pass it on.
				strcpy(fieldValue, Params.Intro);
				strcat(fieldValue, fieldExpr);
				strcat(fieldValue, Params.Outro);

				strcat(value, fieldValue);
			}
		}

		start = matchingOutro + strlen(Params.Outro);
	}

	//if (escapes)
	//	ProcessEscapes_2(value, ?);

	return true;
}



char* FieldReplacer_GetTimeString(char* value, const char* format, bool utc)
{
	value[0] = NULL;

	// Get the current time.
	DateTime now;
	Utils::Now(&now, utc);

	Utils::FormatTime(value, &now, format);

	return value;
}


void FieldReplacer_ReplaceField(const char* fieldExpr, Dictionary* args, char* value, ReplaceFieldParams* params)
{
	value[0] = NULL;

	// 'args' takes precedence.
	if ((NULL != args) && args->ContainsKey(fieldExpr))
	{
		// TEMPORARY:
		args->Get(fieldExpr, value, 100);

		return;
	}

	// Is it a special field?
	if ((strlen(params->SpecialFieldPrefix) == 0) || Utils::StringStartsWith(fieldExpr, params->SpecialFieldPrefix, false))
	{
		// Yes.
		if (params->ReplaceSpecialFields)
			FieldReplacer_ReplaceSpecialField(fieldExpr, value, params);
		else
		{
			strcpy(value, params->Intro);
			strcat(value, fieldExpr);
			strcat(value, params->Outro);
		}

		return;
	}

	else
	{
		strcpy(value, "(");
		strcat(value, fieldExpr);
		strcat(value, ")");
	}

	return;
}


char* FieldReplacer_ReplaceSpecialField(const char* fieldExpr, char* value, ReplaceFieldParams* params)
{
	value[0] = NULL;

	// A 'Special' field, e.g. a System Info item.
	const char* text = fieldExpr + strlen(params->SpecialFieldPrefix);

	char fields[2][ARDJACK_MAX_VALUE_LENGTH];

	int count = Utils::SplitText2Array(text, ' ', fields, 2, ARDJACK_MAX_VALUE_LENGTH);
	if (count == 0) return value;

	char field0[ARDJACK_MAX_VALUE_LENGTH];
	strcpy(field0, fields[0]);
	_strlwr(field0);

	if (Utils::StringEquals(field0, PRM("computer"), false))
	{
		strcpy(value, Globals::ComputerName);
		return value;
	}

	if (Utils::StringEquals(field0, PRM("date"), false))
		return FieldReplacer_GetTimeString(value, params->DateFormat);

	if (Utils::StringEquals(field0, PRM("ip"), false))
	{
		strcpy(value, Globals::IpAddress);
		return value;
	}

	if (Utils::StringEquals(field0, PRM("time"), false))
		return FieldReplacer_GetTimeString(value, params->TimeFormat);

	if (Utils::StringEquals(field0, PRM("timems"), false))
		return FieldReplacer_GetTimeString(value, PRM("HH:mm:ss.fff"));

	if (Utils::StringEquals(field0, PRM("utc"), false))
		return FieldReplacer_GetTimeString(value, params->TimeFormat, true);

	if (Utils::StringEquals(field0, PRM("utcms"), false))
		return FieldReplacer_GetTimeString(value, PRM("HH:mm:ss.fff"), true);

	// TEMPORARY:
	//if (NULL != GetSysInfo)
	//{
	//	value = GetSysInfo(text);
	//	return;
	//}

	//Log::LogInfo(PRM("'GetSysInfo' isn't setup"));

	Log::LogWarning(PRM("Unrecognized special field: '"), fieldExpr, "'");

	strcpy(value, "(");
	strcat(value, fieldExpr);
	strcat(value, ")");

	return value;
}

