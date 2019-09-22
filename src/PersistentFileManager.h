/*
	PersistentFileManager.h

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

	#include "FlashLibrary.h"

	//#ifdef ARDJACK_FLASH_AVAILABLE
	//	#ifdef ARDJACK_ARDUINO_DUE
	//		#include <FlashAsEEPROM.h>
	//	#else
	//		#include <FlashStorage.h>
	//	#endif
	//#endif
#else
	#include "stdafx.h"
#endif

#include "Globals.h"


#ifdef ARDJACK_INCLUDE_PERSISTENCE

#include "DateTime.h"
#include "Log.h"
#include "PersistentFile.h"
#include "Utils.h"




class PersistentFileManager
{
protected:
	#ifdef ARDUINO
		int _LineCount;														// total no.of persisted lines in flash
		bool _Writing;

		#ifdef ARDJACK_FLASH_AVAILABLE
			FlashStorageClass<PersistentFileLine>* _Lines[ARDJACK_MAX_PERSISTED_LINES];

			virtual bool WriteFlash(const char* line, int index);
		#endif
#endif

public:
	int FileCount;
	PersistentFile Files[ARDJACK_MAX_PERSISTED_FILES];

	PersistentFileManager();

#ifdef ARDUINO
	virtual bool Close(PersistentFile* file);
	virtual bool Puts(PersistentFile* file, const char* line);
	virtual bool Scan();
	virtual void SetWriting(bool value);
	virtual bool Writing();

	#ifdef ARDJACK_FLASH_AVAILABLE
		virtual FlashStorageClass<PersistentFileLine>* GetLine(PersistentFile* file, int index);
	#endif
#else
	virtual bool Scan(const char* folder);
#endif

	virtual bool Clear();
	virtual bool List();
	virtual bool LoadIniFile(const char* name);
	virtual PersistentFile* Lookup(const char* name, bool autoCreate = false);
};

#endif
