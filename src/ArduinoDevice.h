/*
	ArduinoDevice.h

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

#include "DetectBoard.h"
#include "Device.h"

class Dynamic;



class ArduinoDevice : public Device
{
protected:
	virtual bool ApplyConfig(bool quiet) override;


#ifdef ARDJACK_ARDUINO_DUE
	bool ConfigureDUE();
#endif
	
#ifdef ARDJACK_ARDUINO_MEGA2560
	bool ConfigureMega2560();
#endif

#ifdef ARDJACK_ARDUINO_MKR
	bool ConfigureArduinoMKR();
#endif

#ifdef ARDJACK_ARDUINO_SAMD_ZERO
	bool ConfigureSparkFunRedBoardTurbo();
#endif

#ifdef ARDJACK_ARDUINO_UNO
	bool ConfigureUno();
#endif

#ifdef ARDJACK_ESP32
	bool ConfigureEspressifESP32();
#endif

#ifdef ARDJACK_FEATHER_M0
	bool ConfigureFeatherM0();
#endif

	virtual bool CreateDefaultInventory() override;
	virtual bool ReadAnalog(Part* part, Dynamic* value);
	virtual bool ReadDigital(Part* part, Dynamic* value);
	virtual bool WriteAnalog(Part* part, Dynamic* value);
	virtual bool WriteDigital(Part* part, Dynamic* value);

public:
	ArduinoDevice(const char* name);
	~ArduinoDevice();

	virtual bool AddConfig() override;
	virtual Part* AddPart(const char* name, int type, int subtype, int pin) override;
	virtual bool Read(Part* part, Dynamic* value) override;
	virtual bool Write(Part* part, Dynamic* value) override;
};

#endif
