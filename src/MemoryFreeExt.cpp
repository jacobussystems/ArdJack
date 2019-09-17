/*
	MemoryFreeExt.cpp

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

// Based on 'MemoryFree' downloaded 8 Aug 2019 from:
//		https://github.com/McNeight/MemoryFree

// Extended to use:
//		- ESP.getFreeHeap() for Espressif boards.
//		- 'mallinfo' for other ARM boards, to give total free heap size (N.B. may well be fragmented).


#include "pch.h"


#ifdef ARDUINO

// Arduino only.

#include <arduino.h>

#include "DetectBoard.h"
#include "Globals.h"
#include "Log.h"
#include "MemoryFreeExt.h"
#include "Utils.h"




#ifdef ARDJACK_ESP32

	#include <esp.h>

	bool GetHeapInfo(MemoryInfo* memInfo)
	{
		char temp[20];
		//Log::LogInfoF("ESP.getFreePsram: %d", ESP.getFreePsram());
		//Log::LogInfoF("ESP.getPsramSize: %d", ESP.getPsramSize());

		if (Globals::Verbosity > 7)
		{
			UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
			Log::LogInfoF("Stack high watermark: %d", uxHighWaterMark);
		}

		// ??
		memInfo->HeapFree = ESP.getFreeHeap();
		memInfo->HeapMax = 0;
		memInfo->HeapUsed = ESP.getHeapSize();

		return true;
	}

#else

	#ifdef ARDJACK_MALLOC_AVAILABLE

		#include <malloc.h>

		bool GetHeapInfo(MemoryInfo* memInfo)
		{
			struct mallinfo mi = mallinfo();

			memInfo->HeapFree = mi.fordblks;
			memInfo->HeapMax = mi.arena;
			memInfo->HeapUsed = mi.uordblks;

			return true;
		}

	#else

		extern unsigned int __heap_start;
		extern void *__brkval;

		// The free list structure as maintained by the avr-libc memory allocation routines.
		struct __freelist
		{
		  size_t sz;
		  struct __freelist *nx;
		};

		// The head of the free list structure.
		extern struct __freelist *__flp;

		// Calculates the size of the free list.
		int freeListSize()
		{
		  struct __freelist* current;
		  int total = 0;

		  for (current = __flp; current; current = current->nx)
		  {
			total += 2; /* Add two bytes for the memory block's header  */
			total += (int) current->sz;
		  }

		  return total;
		}

		bool GetHeapInfo(MemoryInfo* memInfo)
		{
			// ??
			memInfo->HeapMax = 0;
			memInfo->HeapUsed = 0;
			
			int free_memory;

			if ((int)__brkval == 0)
			{
				free_memory = ((int)&free_memory) - ((int)&__heap_start);
			}
			else
			{
				free_memory = ((int)&free_memory) - ((int)__brkval);
				free_memory += freeListSize();
			}

			memInfo->HeapFree = free_memory;

			return true;
		}

	#endif

#endif

#endif
