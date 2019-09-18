#pragma once

#ifdef ARDUINO
#else

#include "stdafx.h"

#include "Globals.h"


class MemoryHelpers
{
protected:

public:
	static long HeapUsed();
};

#endif
