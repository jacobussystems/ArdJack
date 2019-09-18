/*
	VellermanK8055Device.h

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

// Windows only.

#ifdef ARDUINO
#else

#include "stdafx.h"

#include "Device.h"
#include "Globals.h"



// WARNING - Can't create more than one instance of 'VellemanK8055Device' until '_hDLL' and 'Velleman_ClearAllAnalog' etc.
// are re-designed (static?).


class VellemanK8055Device :
	public Device
{
protected:
	HINSTANCE _hDLL;

	virtual bool InitDLL();
	virtual bool ReadAnalog(Part* part, Dynamic* value);
	virtual bool ReadDigital(Part* part, Dynamic* value);
	virtual bool WriteAnalog(Part* part, Dynamic* value);
	virtual bool WriteDigital(Part* part, Dynamic* value);

	typedef void(CALLBACK* t_func)(int);
	typedef void(CALLBACK* t_func0)();
	typedef int(CALLBACK* t_func1)();
	typedef void(CALLBACK* t_func2)(int*, int*);
	typedef void(CALLBACK* t_func3)(int, int);
	typedef int(CALLBACK* t_func4)(int);
	typedef bool(CALLBACK* t_func5)(int);

	t_func0 Velleman_ClearAllAnalog;
	t_func0 Velleman_ClearAllDigital;
	t_func Velleman_ClearAnalogChannel;
	t_func Velleman_ClearDigitalChannel;
	t_func0 Velleman_CloseDevice;
	t_func4 Velleman_OpenDevice;
	t_func3 Velleman_OutputAllAnalog;
	t_func3 Velleman_OutputAnalogChannel;
	t_func2 Velleman_ReadAllAnalog;
	t_func1 Velleman_ReadAllDigital;
	t_func4 Velleman_ReadAnalogChannel;
	t_func5 Velleman_ReadDigitalChannel;
	t_func0 Velleman_SetAllAnalog;
	t_func0 Velleman_SetAllDigital;
	t_func Velleman_SetAnalogChannel;
	t_func Velleman_SetDigitalChannel;
	t_func0 Velleman_Version;
	t_func Velleman_WriteAllDigital;

public:
	VellemanK8055Device(const char* name);
	~VellemanK8055Device();

	virtual bool CreateDefaultInventory() override;
	virtual bool Open() override;
	virtual bool Read(Part* part, Dynamic* value) override;
	virtual bool Write(Part* part, Dynamic* value) override;
};

#endif

