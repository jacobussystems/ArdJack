/*
	Device.h

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
#else
	#include "stdafx.h"
#endif

#include "Filter.h"
#include "Globals.h"
#include "IoTMessage.h"
#include "IoTObject.h"

class Connection;
class DeviceCodec1;
//class Dictionary;
class Dynamic;
class FieldReplacer;
class Shield;
class StringList;




class Device : public IoTObject
{
protected:
	bool _IsOpen;
	int _MessageFormat;
	char _MessagePrefix[10];
	char _MessageToPath[20];
	IoTMessage _ResponseMsg;

#ifdef ARDJACK_INCLUDE_SHIELDS
	char _ShieldName[ARDJACK_MAX_NAME_LENGTH];
#endif

	virtual bool Activate() override;
	virtual bool ApplyConfig(bool quiet = false) override;
	virtual bool CheckInput(Part* part, bool* change);
	virtual bool CloseParts();
	virtual bool Deactivate() override;
	virtual bool OpenParts();
	virtual bool PollInputs();
	virtual bool PollOutputs();
	virtual bool ValidateConfig(bool quiet = false) override;

public:
	char DeviceClass[ARDJACK_MAX_NAME_LENGTH];
	DeviceCodec1* DeviceCodec;
#ifdef ARDJACK_INCLUDE_SHIELDS
	Shield* DeviceShield;
#endif
	char DeviceType[ARDJACK_MAX_NAME_LENGTH];
	char DeviceVersion[ARDJACK_MAX_NAME_LENGTH];
	Connection* InputConnection;
	int InputTimeout;													// ms
	Connection* OutputConnection;
	uint8_t PartCount;
	Part* Parts[ARDJACK_MAX_PARTS];
	int ReadEvents;
	int WriteEvents;

	Device(const char* name);
	~Device();

	virtual bool AddConfig() override;
	virtual Part* AddPart(const char* name, int type, int subtype, int pin);
	virtual bool AddParts(const char* prefix, int count, int type, int subtype, int startIndex, int startPin);
	virtual bool ClearInventory();
	virtual bool Close();
	virtual bool Configure(const char* entity, StringList* settings, int start, int count) override;
#ifdef ARDJACK_INCLUDE_SHIELDS
	virtual bool ConfigureForShield();
#endif
	virtual bool ConfigurePart(Part* part, StringList* settings, int start, int count);
	virtual bool ConfigurePartType(int partType, StringList* settings, int start, int count);
	virtual bool CreateDefaultInventory();
	virtual bool DoBeep(int index, int freqHz, int durMs);
	virtual bool DoFlash(const char* name = "led0", int durMs = 20);
	virtual int GetCount(int partType);
	virtual bool GetParts(const char* expr, Part* parts[], uint8_t* count, bool quiet = false);
	virtual bool GetPartsOfType(int partType, Part* parts[], uint8_t* count);
	static int LookupOperation(const char* name);
	virtual Part* LookupPart(const char* name, bool quiet = false);
	virtual bool LookupParts(const char* names, Part* parts[], uint8_t* count);
	virtual bool Open();
	virtual bool Poll() override;
	virtual bool Read(Part* part, Dynamic* value);
#ifdef ARDJACK_INCLUDE_MULTI_PARTS
	virtual bool ReadMulti(Part* part, char* value);
#endif
	virtual bool ScanInputs(bool* changes, int count, int delayMs);
	virtual bool ScanInputsOnce(bool* changes);
	//virtual bool Send(int oper, int partType, int index, char values[][ARDJACK_MAX_VALUE_LENGTH]);
	virtual bool SendInventory(bool includeZeroCounts = false);
	virtual bool SendNotification(Part* part);
	virtual bool SendResponse(int oper, const char* aName, const char* text);
	virtual bool SetNotify(Part* part, bool state);
	virtual bool SetNotify(int partType, bool state);
	virtual bool Update();
	virtual bool Write(Part* part, Dynamic* value);
#ifdef ARDJACK_INCLUDE_MULTI_PARTS
	virtual bool WriteMulti(Part* part, const char* value);
#endif
};

