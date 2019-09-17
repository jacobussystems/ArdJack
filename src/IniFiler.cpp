// IniFiler.cpp

#include "pch.h"

#include "ConfigProp.h"
#include "Globals.h"
#include "IniFiler.h"
#include "IoTObject.h"
#include "Log.h"
#include "Utils.h"



IniFiler::IniFiler()
{
	strcpy(CommentPrefix, "#");
	_CurSectionType = INIFILER_SECTION_NONE;
}


bool IniFiler::ReadLine(const char* line)
{
	// Caller provides the next line to read in 'line'.

	// Blank line?
	if (NULL == line[0]) return true;

	// Comment line?
	if (Utils::StringStartsWith(line, CommentPrefix)) return true;

	if (line[0] == '[')
		return ReadLine_SectionStart(line);

	switch (_CurSectionType)
	{
	case INIFILER_SECTION_DESCRIPTION:
		// Reading a Description section.
		ReadLine_Description(line);
		break;

	case INIFILER_SECTION_GENERAL:
		// Reading a General section (just Settings).
		ReadLine_General(line);
		break;

	case INIFILER_SECTION_NONE:
		// Waiting for the start of a section.
		Log::LogError(PRM("INI file: Invalid line (waiting for section): '"), line, "'");
		return false;

	case INIFILER_SECTION_OBJECT:
		// Reading an Object section for '_CurObject'.
		ReadLine_Object(line);
		break;
	}

	return true;
}


bool IniFiler::ReadLine_Description(const char* line)
{
	// Ignore Description section contents for now.
	return true;
}


bool IniFiler::ReadLine_General(const char* line)
{
	char fields[4][ARDJACK_MAX_VALUE_LENGTH];
	bool handled;

	int count = Utils::SplitText2Array(line, '=', fields, 4, ARDJACK_MAX_VALUE_LENGTH);

	if (count < 2)
	{
		Log::LogError(PRM("INI file: Insufficient fields for GENERAL line: '"), line, "'");
		return false;
	}

	return Globals::Set(fields[0], fields[1], &handled);
}


bool IniFiler::ReadLine_Object(const char* line)
{
	// Ignore 'line' if there's no current object.
	if (NULL == _CurObject) return false;

	//char temp[102];

	char fields[4][ARDJACK_MAX_VALUE_LENGTH];
	int count = Utils::SplitText2Array(line, '=', fields, 4, ARDJACK_MAX_VALUE_LENGTH);

	if (count < 2)
	{
		Log::LogError(PRM("INI file: Insufficient fields for OBJECT line: '"), line, "'");
		return false;
	}

	if (Globals::Verbosity > 4)
		Log::LogInfo(_CurObject->Name, ": ", fields[0], " -> ", fields[1]);

	ConfigProp* item = _CurObject->Config->LookupPath(fields[0]);
	if (NULL == item)
	{
		char temp[102];
		sprintf(temp, PRM("INI file: Object '%s' has no Configuration property '%s'"), _CurObject->Name, fields[0]);
		Log::LogError(temp);
		return false;
	}

	return item->SetFromString(fields[1]);
}


bool IniFiler::ReadLine_SectionStart(const char* line)
{
	// Start of a new section.
	if (!Utils::StringEndsWith(line, "]"))
	{
		Log::LogError(PRM("INI file: Invalid line (no trailing ']'): '"), line, "'");
		return false;
	}

	char section[82];
	strcpy(section, line + 1);										// skip the leading '['
	section[strlen(section) - 1] = NULL;							// remove the trailing ']'
	Utils::Trim(section);

	if (NULL == section[0])
	{
		Log::LogError(PRM("INI file: Empty section header: '"), line, "'");
		return false;
	}

	StringList fields;

	int count = Utils::SplitText(section, ' ', &fields, ARDJACK_MAX_VALUES, ARDJACK_MAX_VALUE_LENGTH);

	char first[82];
	strcpy(first, fields.Get(0));
	_strupr(first);

	if (Utils::StringEquals(first, "GENERAL", true))
	{
		_CurSectionType = INIFILER_SECTION_GENERAL;
		return true;
	}

	if (Utils::StringEquals(first, "DESCRIPTION", true))
	{
		_CurSectionType = INIFILER_SECTION_DESCRIPTION;
		return true;
	}

	if (Utils::StringEquals(first, "OBJECT", true))
	{
		if (count < 2)
		{
			Log::LogError(PRM("INI file: Invalid section header in '"), line, "'");
			return false;
		}

		char name[ARDJACK_MAX_NAME_LENGTH];
		strcpy(name, fields.Get(1));

		_CurObject = Globals::ObjectRegister->LookupName(name);
		if (NULL == _CurObject)
		{
			Log::LogError(PRM("INI file: Invalid object name in '"), line, "'");
			return false;
		}

		_CurSectionType = INIFILER_SECTION_OBJECT;
		return true;
	}

	Log::LogError(PRM("INI file: Invalid section type in '"), line, "'");

	return false;
}

