/*
	PersistentFile.cpp

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
#endif

#include "Globals.h"


#ifdef ARDJACK_INCLUDE_PERSISTENCE

#ifdef ARDUINO
	#ifdef ARDJACK_FLASH_AVAILABLE
		#ifdef ARDJACK_ARDUINO_DUE
			#include <FlashAsEEPROM.h>
		#else
			#include <FlashStorage.h>
		#endif
	#endif
#else
	#include <stdio.h>
	#include "stdafx.h"
#endif

#include "Log.h"
#include "PersistentFile.h"
#include "PersistentFileManager.h"
#include "Table.h"
#include "Utils.h"


// Arduino + Windows.

bool PersistentFile::PutNameValuePair(const char* name, const char* value)
{
	// Write a setting in "name = value" format.
	char line[ARDJACK_PERSISTED_LINE_LENGTH];
	strcpy(line, name);
	strcat(line, " = ");
	strcat(line, value);

	return Puts(line);
}


#ifdef ARDUINO

PersistentFile::PersistentFile()
{
	LineCount = 0;
	_WriteMode = false;

	Manager = NULL;
	Name[0] = NULL;
}


bool PersistentFile::Close()
{
	if (_WriteMode)
	{
		if (NULL == Manager)
		{
			Log::LogError(PRM("PersistentFile::Close: No manager"));
			return false;
		}

		return Manager->Close(this);
	}

	return true;
}


bool PersistentFile::Eof()
{
	return (_LineIndex >= LineCount);
}


bool PersistentFile::Gets(char* line, int size)
{
	// Read the next line.
#ifdef ARDJACK_FLASH_AVAILABLE
	line[0] = NULL;

	if (_WriteMode || Eof())
		return false;

	FlashStorageClass<PersistentFileLine>* fs = Manager->GetLine(this, _LineIndex++);
	if (NULL == fs) return false;

	PersistentFileLine pfLine = fs->read();
	strcpy(line, pfLine.Line);

	return true;
#else
	return false;
#endif
}


bool PersistentFile::IsOpen()
{
	// TEMPORARY:
	return true;
}


bool PersistentFile::Open(const char* mode)
{
	char temp[102];

	if (NULL == Manager)
	{
		Log::LogError(PRM("PersistentFile::Open: No manager"));
		return false;
	}

	sprintf(temp, PRM("PersistentFile::Open: File '%s', mode '%s'"), Name, mode);
	Log::LogInfo(temp);

	_WriteMode = (mode[0] == 'w') || (mode[0] == 'W');

	if (!_WriteMode && Manager->Writing())
	{
		sprintf(temp, PRM("PersistentFile::Open: Can't read '%s' - manager is writing"), Name);
		Log::LogInfo(temp);
		return false;
	}

	if (_WriteMode)
		Manager->SetWriting(true);

	if (_WriteMode)
	{
		Log::LogInfo(PRM("PersistentFile::Open: Writing to flash"));
		LineCount = 0;
	}
	else
	{
		sprintf(temp, PRM("PersistentFile::Open: Reading %d lines from flash"), LineCount);
		Log::LogInfo(temp);
		_LineIndex = 0;
	}

	return true;
}


bool PersistentFile::Puts(const char* line)
{
	// Write 'line'.
	if (!_WriteMode)
		return false;

	if (NULL == Manager)
	{
		Log::LogError(PRM("PersistentFile::Puts: No manager"));
		return false;
	}

	if (!Manager->Puts(this, line))
		return false;

	LineCount++;

	return true;
}


#else


PersistentFile::PersistentFile()
{
	_File = NULL;
	LineCount = 0;
	_LineIndex = 0;
	_WriteMode = false;

	Filename[0] = NULL;
	Name[0] = NULL;
}


bool PersistentFile::Close()
{
	if (!IsOpen())
	{
		Log::LogInfo(PRM("PersistentFile::Close: Not open"));
		return true;
	}

	fclose(_File);
	_File = NULL;

	if (_WriteMode)
	{
		char temp[102];
		sprintf(temp, PRM("PersistentFile::Close: %d lines written to file: "), LineCount);
		Log::LogInfo(temp, Filename);
	}

	return true;
}


bool PersistentFile::Eof()
{
	if (!IsOpen())
	{
		Log::LogError(PRM("PersistentFile::Eof: Not open"));
		return true;
	}

	if (_WriteMode)
		return true;

	return (feof(_File) != 0);
}


bool PersistentFile::Gets(char* line, int size)
{
	// Read the next line.
	line[0] = NULL;

	if (!IsOpen())
	{
		Log::LogError(PRM("PersistentFile::Gets: Not open"));
		return false;
	}

	if (_WriteMode)
	{
		Log::LogError(PRM("PersistentFile::Gets: File is open for writing"));
		return false;
	}

	char* ret = fgets(line, size, _File);
	if (NULL == ret) return false;

	Utils::Trim(line);
	LineCount++;

	return true;
}


bool PersistentFile::IsOpen()
{
	return (NULL != _File);
}


bool PersistentFile::Open(const char* mode)
{
	LineCount = 0;
	_LineIndex = 0;
	_WriteMode = Utils::StringStartsWith(mode, "w");

	_File = fopen(Filename, mode);

	if (NULL == _File)
	{
		Log::LogError(PRM("PersistentFile::Open: File not opened: "), Filename);
		return false;
	}

	if (_WriteMode)
		Log::LogInfo(PRM("PersistentFile::Open: File opened (write): "), Filename);
	else
		Log::LogInfo(PRM("PersistentFile::Open: File opened (read): "), Filename);

	return true;
}


bool PersistentFile::Puts(const char* line)
{
	// Write the next line.
	if (!IsOpen())
	{
		Log::LogError(PRM("PersistentFile::Puts: Not open"));
		return false;
	}

	if (!_WriteMode)
	{
		Log::LogError(PRM("PersistentFile::Puts: File is open for reading"));
		return false;
	}

	int ret = fputs(line, _File);
	if (ret < 0)
	{
		Log::LogError(PRM("PersistentFile::Puts: Failed to write to file: "), Filename);
		return false;
	}

	ret = fputs("\n", _File);
	if (ret < 0)
	{
		Log::LogError(PRM("PersistentFile::Puts: Failed to write to file: "), Filename);
		return false;
	}

	LineCount++;

	return true;
}

#endif

#endif
