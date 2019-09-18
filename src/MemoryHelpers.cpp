#include "pch.h"

#include "Globals.h"

#ifdef ARDUINO
#else

#include "stdafx.h"
#include <malloc.h>

#include "Log.h"
#include "MemoryHelpers.h"
#include "Utils.h"



long MemoryHelpers::HeapUsed()
{
	// Returns used heap size in bytes or -1 if heap is corrupted.
	_HEAPINFO info = { 0, 0, 0 };
	long used = 0;
	int rc;

	while ((rc = _heapwalk(&info)) == _HEAPOK)
	{
		if (info._useflag == _USEDENTRY)
			used += info._size;
	}

	if ((rc != _HEAPEND) && (rc != _HEAPEMPTY))
		used = (used ? -used : -1);

	return used;
}

#endif

