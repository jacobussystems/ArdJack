/*
	FifoBuffer.cpp

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
	#include "cppQueue.h"
#else
	#include "stdafx.h"
	#include "RingBuf.h"
#endif

#include "FifoBuffer.h"
#include "Log.h"
#include "Utils.h"



FifoBuffer::FifoBuffer(int size, int count)
{
	if (Globals::Verbosity > 7)
		Log::LogInfoF(PRM("FifoBuffer ctor: %p, size %d, count %d"), this, size, count);

#ifdef ARDUINO
	_Queue = new Queue(size, count, FIFO, false);
#else
	_RingBuf = RingBuf_new(size, count);
#endif
}


FifoBuffer::~FifoBuffer()
{
	if (Globals::Verbosity > 7)
		Log::LogInfoF(PRM("FifoBuffer ~: %p"), this);

#ifdef ARDUINO
	if (NULL != _Queue)
		delete _Queue;
#else
	if (NULL != _RingBuf)
		free(_RingBuf);
#endif
}


bool FifoBuffer::IsEmpty()
{
#ifdef ARDUINO
	return _Queue->isEmpty();
#else
	return _RingBuf->isEmpty(_RingBuf);
#endif
}


bool FifoBuffer::IsFull()
{
#ifdef ARDUINO
	return _Queue->isFull();
#else
	return _RingBuf->isFull(_RingBuf);
#endif
}


bool FifoBuffer::Pop(void* obj)
{
	if (IsEmpty())
	{
		Log::LogWarningF(PRM("FifoBuffer::Pop: %p, buffer empty"), this);
		return false;
	}

#ifdef ARDUINO
	(void *) _Queue->pull(obj);
#else
	(void *) _RingBuf->pull(_RingBuf, obj);
#endif

	return true;
}


bool FifoBuffer::Push(const void* obj)
{
	if (IsFull())
	{
		Log::LogWarningF(PRM("FifoBuffer::Push: %p, buffer full"), this);
		return false;
	}

#ifdef ARDUINO
	_Queue->push(obj);
#else
	_RingBuf->add(_RingBuf, obj);
#endif

	return true;
}

