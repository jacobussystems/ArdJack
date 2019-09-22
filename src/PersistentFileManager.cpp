/*
	PersistentFileManager.cpp

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
	#include "DetectBoard.h"

	#ifdef ARDJACK_FLASH_AVAILABLE
		#ifdef ARDJACK_ARDUINO_DUE
			#include <FlashAsEEPROM.h>
		#else
			#include <FlashStorage.h>
		#endif
	#endif
#else
	#include "stdafx.h"
#endif

#include "Globals.h"


#ifdef ARDJACK_INCLUDE_PERSISTENCE

#include "IniFiler.h"
#include "Log.h"
#include "PersistentFile.h"
#include "PersistentFileManager.h"
#include "Utils.h"


#ifdef ARDUINO
	#ifdef ARDJACK_FLASH_AVAILABLE
		FlashStorage(_FlashLinesStored, int);
		FlashStorage(_FlashLine0, PersistentFileLine);
		FlashStorage(_FlashLine1, PersistentFileLine);
		FlashStorage(_FlashLine2, PersistentFileLine);
		FlashStorage(_FlashLine3, PersistentFileLine);
		FlashStorage(_FlashLine4, PersistentFileLine);
		FlashStorage(_FlashLine5, PersistentFileLine);
		FlashStorage(_FlashLine6, PersistentFileLine);
		FlashStorage(_FlashLine7, PersistentFileLine);
		FlashStorage(_FlashLine8, PersistentFileLine);
		FlashStorage(_FlashLine9, PersistentFileLine);
		FlashStorage(_FlashLine10, PersistentFileLine);
		FlashStorage(_FlashLine11, PersistentFileLine);
		FlashStorage(_FlashLine12, PersistentFileLine);
		FlashStorage(_FlashLine13, PersistentFileLine);
		FlashStorage(_FlashLine14, PersistentFileLine);
		FlashStorage(_FlashLine15, PersistentFileLine);
		FlashStorage(_FlashLine16, PersistentFileLine);
		FlashStorage(_FlashLine17, PersistentFileLine);
		FlashStorage(_FlashLine18, PersistentFileLine);
		FlashStorage(_FlashLine19, PersistentFileLine);
		FlashStorage(_FlashLine20, PersistentFileLine);
		FlashStorage(_FlashLine21, PersistentFileLine);
		FlashStorage(_FlashLine22, PersistentFileLine);
		FlashStorage(_FlashLine23, PersistentFileLine);
		FlashStorage(_FlashLine24, PersistentFileLine);
		FlashStorage(_FlashLine25, PersistentFileLine);
		FlashStorage(_FlashLine26, PersistentFileLine);
		FlashStorage(_FlashLine27, PersistentFileLine);
		FlashStorage(_FlashLine28, PersistentFileLine);
		FlashStorage(_FlashLine29, PersistentFileLine);
		FlashStorage(_FlashLine30, PersistentFileLine);
		FlashStorage(_FlashLine31, PersistentFileLine);
		FlashStorage(_FlashLine32, PersistentFileLine);
		FlashStorage(_FlashLine33, PersistentFileLine);
		FlashStorage(_FlashLine34, PersistentFileLine);
		FlashStorage(_FlashLine35, PersistentFileLine);
		FlashStorage(_FlashLine36, PersistentFileLine);
		FlashStorage(_FlashLine37, PersistentFileLine);
		FlashStorage(_FlashLine38, PersistentFileLine);
		FlashStorage(_FlashLine39, PersistentFileLine);
	#endif
#endif


// Arduino + Windows.

bool PersistentFileManager::LoadIniFile(const char* name)
{
	// Load a persistent INI file called 'name'.
	if (Globals::Verbosity > 3)
		Log::LogInfo(PRM("PersistentFileManager::LoadIniFile: '"), name, "'");

	PersistentFile* file = Lookup(name, false);
	if (NULL == file)
	{
		Log::LogError(PRM("No such INI file: '"), name, "'");
		return false;
	}

	if (!file->Open("r"))
		return false;

	char line[ARDJACK_PERSISTED_LINE_LENGTH];
	IniFiler filer;

	while (!file->Eof())
	{
		if (!file->Gets(line, 200))
			break;

		if (!filer.ReadLine(line))
			break;
	}

	file->Close();

	return true;
}


#ifdef ARDUINO

PersistentFileManager::PersistentFileManager()
{
	if (Globals::Verbosity > 7)
		Log::LogInfo(PRM("PersistentFileManager ctor"));

#ifdef ARDJACK_FLASH_AVAILABLE
	_Lines[0] = &_FlashLine0;
	_Lines[1] = &_FlashLine1;
	_Lines[2] = &_FlashLine2;
	_Lines[3] = &_FlashLine3;
	_Lines[4] = &_FlashLine4;
	_Lines[5] = &_FlashLine5;
	_Lines[6] = &_FlashLine6;
	_Lines[7] = &_FlashLine7;
	_Lines[8] = &_FlashLine8;
	_Lines[9] = &_FlashLine9;
	_Lines[10] = &_FlashLine10;
	_Lines[11] = &_FlashLine11;
	_Lines[12] = &_FlashLine12;
	_Lines[13] = &_FlashLine13;
	_Lines[14] = &_FlashLine14;
	_Lines[15] = &_FlashLine15;
	_Lines[16] = &_FlashLine16;
	_Lines[17] = &_FlashLine17;
	_Lines[18] = &_FlashLine18;
	_Lines[19] = &_FlashLine19;
	_Lines[20] = &_FlashLine20;
	_Lines[21] = &_FlashLine21;
	_Lines[22] = &_FlashLine22;
	_Lines[23] = &_FlashLine23;
	_Lines[24] = &_FlashLine24;
	_Lines[25] = &_FlashLine25;
	_Lines[26] = &_FlashLine26;
	_Lines[27] = &_FlashLine27;
	_Lines[28] = &_FlashLine28;
	_Lines[29] = &_FlashLine29;
	_Lines[30] = &_FlashLine30;
	_Lines[31] = &_FlashLine31;
	_Lines[32] = &_FlashLine32;
	_Lines[33] = &_FlashLine33;
	_Lines[34] = &_FlashLine34;
	_Lines[35] = &_FlashLine35;
	_Lines[36] = &_FlashLine36;
	_Lines[37] = &_FlashLine37;
	_Lines[38] = &_FlashLine38;
	_Lines[39] = &_FlashLine39;
#endif

	_Writing = false;
	FileCount = 0;
}


bool PersistentFileManager::Clear()
{
	Log::LogInfo(PRM("PersistentFileManager::Clear"));
	FileCount = 0;
	_LineCount = 0;

#ifdef ARDJACK_FLASH_AVAILABLE
	// Empty all flash lines.
	for (int i = 0; i < ARDJACK_MAX_PERSISTED_LINES; i++)
		WriteFlash("", i);
#endif

	return true;
}


bool PersistentFileManager::Close(PersistentFile* file)
{
	char temp[102];
	sprintf(temp, PRM("PersistentFileManager::Close: File '%s'"), file->Name);
	Log::LogInfo(temp);

#ifdef ARDJACK_FLASH_AVAILABLE
	WriteFlash("*", _LineCount);								// don't bump '_LineCount'

	// Empty any remaining flash lines.
	for (int i = _LineCount + 1; i < ARDJACK_MAX_PERSISTED_LINES; i++)
		WriteFlash("", i);

	// Update the line count in flash.
	_FlashLinesStored.write(_LineCount);
#endif

	return true;
}


#ifdef ARDJACK_FLASH_AVAILABLE
FlashStorageClass<PersistentFileLine>* PersistentFileManager::GetLine(PersistentFile* file, int index)
{
	// Get line 'index' (zero-based for each file) of 'file'.
	if (_Writing)
	{
		Log::LogError(PRM("PersistentFileManager::GetLine: Called when writing"));
		return NULL;
	}

	if ((index < 0) || (!_Writing && (index >= file->LineCount)))
	{
		char temp[122];
		sprintf(temp, PRM("PersistentFileManager::GetLine: Invalid index for file '%s': %d"), file->Name, index);
		Log::LogError(temp);

		return NULL;
	}

	return _Lines[file->StartLineInFlash + index];
}
#endif


bool PersistentFileManager::List()
{
	char temp[ARDJACK_PERSISTED_LINE_LENGTH + 30];

	if (_Writing)
	{
		sprintf(temp, PRM("*** WRITING *** (_LineCount %d)"), _LineCount);
		Log::LogInfo(temp);
	}

	sprintf(temp, PRM("%d files: "), FileCount);
	Log::LogInfo(temp);

#ifdef ARDJACK_FLASH_AVAILABLE
	for (int i = 0; i < FileCount; i++)
	{
		PersistentFile* file = &Files[i];

		sprintf(temp, PRM("  '%s', %d lines starting at flash line %d"),
			file->Name, file->LineCount, file->StartLineInFlash);
		Log::LogInfo(temp);
	}

	sprintf(temp, PRM("%d flash lines:"), ARDJACK_MAX_PERSISTED_LINES);
	Log::LogInfo(temp);

	for (int i = 0; i < ARDJACK_MAX_PERSISTED_LINES; i++)
	{
		PersistentFileLine pfLine = _Lines[i]->read();

		if (NULL != pfLine.Line[0])
		{
			sprintf(temp, PRM("  %2d: '%s'"), i, pfLine.Line);
			Log::LogInfo(temp);
		}
	}
#endif

	return true;
}


PersistentFile* PersistentFileManager::Lookup(const char* name, bool autoCreate)
{
	// Is there a persistent file called 'name'?
	// If not, auto-create it if 'autoCreate' is true.

	PersistentFile* file;

	for (int i = 0; i < FileCount; i++)
	{
		file = &Files[i];

		if (Utils::StringEquals(file->Name, name))
			return file;
	}

	// No such file.
	if (autoCreate)
	{
#ifdef ARDJACK_FLASH_AVAILABLE
		Log::LogInfo(PRM("PersistentFileManager::Lookup: Auto-creating file '"), name, "'");

		char temp[82];
		sprintf(temp, PRM("*file %s"), name);
		WriteFlash(temp, _LineCount++);

		file = &Files[FileCount++];
		file->Manager = this;
		strcpy(file->Name, name);
		file->StartLineInFlash = _LineCount;

		return file;
#endif
	}

	return NULL;
}


bool PersistentFileManager::Puts(PersistentFile* file, const char* line)
{
	// Write 'line' to 'file'.
#ifdef ARDJACK_FLASH_AVAILABLE
	char temp[202];
	sprintf(temp, PRM("PersistentFileManager::Puts: Write to file '%s': '%s'"), file->Name, line);
	Log::LogInfo(temp);

	return WriteFlash(line, _LineCount++);
#else
	return false;
#endif
}


bool PersistentFileManager::Scan()
{
#ifdef ARDJACK_FLASH_AVAILABLE
	FileCount = 0;
	char temp[82];

	_LineCount = _FlashLinesStored.read();

	if (Globals::Verbosity > 5)
	{
		sprintf(temp, PRM("PersistentFileManager::Scan: %d flash lines"), _LineCount);
		Log::LogInfo(temp);
	}

	// Scan the flash lines.
	PersistentFile* file = NULL;

	for (int i = 0; i < ARDJACK_MAX_PERSISTED_LINES; i++)
	{
		if (i >= _LineCount)
		{
			// Finish the current file (if any).
			if (NULL != file)
			{
				file->LineCount = i - file->StartLineInFlash;
				file = NULL;
			}
			break;
		}

		PersistentFileLine pfLine = _Lines[i]->read();

		if (Utils::StringEquals(pfLine.Line, "*"))
		{
			// Finish the current file (if any).
			if (NULL != file)
			{
				file->LineCount = i - file->StartLineInFlash;
				file = NULL;
			}
		}

		if (Utils::StringStartsWith(pfLine.Line, "*file"))
		{
			// Start of a new file.
			char fields[2][ARDJACK_MAX_VALUE_LENGTH];

			int count = Utils::SplitText(pfLine.Line, ' ', fields, 2, ARDJACK_MAX_VALUE_LENGTH);
			if (count < 2)
			{
				Log::LogError(PRM("PersistentFileManager::Scan: Invalid *FILE line: "), pfLine.Line);
				return false;
			}

			char name[ARDJACK_MAX_NAME_LENGTH];
			strcpy(name, fields[1]);

			file = &Files[FileCount];
			file->Manager = this;
			strcpy(file->Name, name);
			file->StartLineInFlash = i + 1;

			FileCount++;
		}
	}

	// Finish the last file (if any, i.e. if no "*" line was found).
	if (NULL != file)
		file->LineCount = ARDJACK_MAX_PERSISTED_LINES - 1 - file->StartLineInFlash;

	// Log a summary.
	sprintf(temp, PRM("PersistentFileManager::Scan: %d files"), FileCount);
	Log::LogInfo(temp);

	for (int i = 0; i < FileCount; i++)
	{
		PersistentFile* file = &Files[i];

		sprintf(temp, PRM("  '%s', %d lines starting at flash line %d"),
			file->Name, file->LineCount, file->StartLineInFlash);
		Log::LogInfo(temp);
	}

	return true;
#else
	return false;
#endif
}


void PersistentFileManager::SetWriting(bool value)
{
	if (value && !_Writing)
		Log::LogInfo(PRM("PersistentFileManager::SetWriting: Now writing"));

	_Writing = value;
}


#ifdef ARDJACK_FLASH_AVAILABLE
bool PersistentFileManager::WriteFlash(const char* line, int index)
{
	char temp[102];

	if (Globals::Verbosity > 5)
	{
		sprintf(temp, PRM("PersistentFileManager::WriteFlash: Writing '%s' to flash line %d"), line, index);
		Log::LogInfo(temp);
	}

	FlashStorageClass<PersistentFileLine>* fs = _Lines[index];
	if (NULL == fs)
	{
		sprintf(temp, PRM("PersistentFileManager::WriteFlash: Failed to write flash line %d"), index);
		Log::LogError(temp);
		return false;
	}

	PersistentFileLine pfLine = fs->read();

	if (!Utils::StringEquals(pfLine.Line, line, true))
	{
		if (Globals::Verbosity > 4)
		{
			sprintf(temp, PRM("PersistentFileManager::WriteFlash: Updating flash line %d"), index);
			Log::LogInfo(temp);
		}

		sprintf(pfLine.Line, line);
		fs->write(pfLine);
	}

	return true;
}
#endif


bool PersistentFileManager::Writing()
{
	return _Writing;
}


#else


PersistentFileManager::PersistentFileManager()
{
	FileCount = 0;
}


bool PersistentFileManager::Clear()
{
	Log::LogInfo(PRM("PersistentFileManager::Clear"));
	FileCount = 0;

	return true;
}


bool PersistentFileManager::List()
{
	char temp[202];

	sprintf(temp, PRM("%d files:"), FileCount);
	Log::LogInfo(temp);

	for (int i = 0; i < FileCount; i++)
	{
		PersistentFile* file = &Files[i];

		sprintf(temp, PRM("\t%s\t%s"), file->Name, file->Filename);
		Log::LogInfo(temp);
	}

	return true;
}


PersistentFile* PersistentFileManager::Lookup(const char* name, bool autoCreate)
{
	// Is there a persistent file called 'name'?
	// ('autoCreate' is ignored in Windows.)

	for (int i = 0; i < FileCount; i++)
	{
		PersistentFile* file = &Files[i];

		if (Utils::StringEquals(file->Name, name))
			return file;
	}

	return NULL;
}


bool PersistentFileManager::Scan(const char* folder)
{
	FileCount = 0;
	PersistentFile* file = NULL;

	char filename[MAX_PATH];

	// Is there a Settings file?
	strcpy(filename, folder);
	strcat(filename, "\\");
	strcat(filename, Globals::AppName);
	strcat(filename, ".ini");

	if (Utils::FileExists(filename))
	{
		file = &Files[FileCount++];
		strcpy(file->Name, "configuration");
		strcpy(file->Filename, filename);
	}

	// Is there a Startup command file?
	strcpy(filename, folder);
	strcat(filename, "\\");
	strcat(filename, Globals::AppName);
	strcat(filename, ".startup");

	if (Utils::FileExists(filename))
	{
		file = &Files[FileCount++];
		strcpy(file->Name, "startup");
		strcpy(file->Filename, filename);
	}

	return true;
}

#endif

#endif
