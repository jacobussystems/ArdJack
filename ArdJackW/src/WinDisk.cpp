/*
	WinDisk.cpp

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

// Windows only.

#ifdef ARDUINO
#else

#ifdef ARDJACK_INCLUDE_WINDISK

#include "stdafx.h"
#include <typeinfo>

#include "Dynamic.h"
#include "Log.h"
#include "UserPart.h"
#include "WinDisk.h"



WinDisk::WinDisk()
	: UserPart()
{
	_IsInput = true;
}


double WinDisk::GetDriveFreeSpace(char drive)
{
	char path[10];
	path[0] = drive;
	path[1] = ':';
	path[2] = '\\';
	path[3] = NULL;

	unsigned __int64 freeBytes;
	unsigned __int64 freeBytesToCaller;
	unsigned __int64 totalBytes;

	bool status = GetDiskFreeSpaceEx(path, (PULARGE_INTEGER)& freeBytesToCaller, (PULARGE_INTEGER)& totalBytes,
		(PULARGE_INTEGER)& freeBytes);

	if (!status)
	{
		Log::LogWarning("Unable to get free space on drive ", path);
		return -1;
	}

	unsigned long long space = freeBytesToCaller;
	double result = space / (double)ARDJACK_BYTES_PER_GB;

	return result;
}


bool WinDisk::Read(Dynamic* value)
{
	char drive = 'A' + Pin;
	double space = GetDriveFreeSpace(drive);

	value->SetDouble(space);
	Value.SetDouble(space);

	return true;
}

#endif

#endif
