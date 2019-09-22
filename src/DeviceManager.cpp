/*
	DeviceManager.cpp

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
#else
	#include "stdafx.h"
#endif

#include "Device.h"
#include "DeviceCodec1.h"
#include "DeviceManager.h"
#include "Displayer.h"
#include "Dynamic.h"
#include "Enumeration.h"
#include "Globals.h"
#include "IoTManager.h"
#include "IoTMessage.h"
#include "Log.h"
#include "Part.h"
#include "PartManager.h"
#include "Utils.h"




DeviceManager::DeviceManager()
	: IoTManager()
{
	if (Globals::Verbosity > 7)
		Log::LogInfo(PRM("DeviceManager ctor"));

	ObjectType = ARDJACK_OBJECT_TYPE_DEVICE;

	if (NULL == Subtypes)
	{
		Subtypes = new Enumeration(PRM("Subtypes"));
		Subtypes->Add(PRM("Arduino"), ARDJACK_DEVICE_SUBTYPE_ARDUINO);
		Subtypes->Add(PRM("Windows"), ARDJACK_DEVICE_SUBTYPE_WINDOWS);
		Subtypes->Add(PRM("VellemanK8055"), ARDJACK_DEVICE_SUBTYPE_VELLEMANK8055);
	}
}


DeviceManager::~DeviceManager()
{
}


Part* DeviceManager::AddPart(Device* dev, const char* text, const char* name, StringList* values)
{
	if (values->Count == 0)
	{
		Log::LogError(PRM("Insufficient arguments for ADD: '"), text, "'");
		return NULL;
	}

	// Get the Part type.
	int type = Globals::PartMgr->LookupType(values->Get(0), true);

	if (type == ARDJACK_PART_TYPE_NONE)
	{
		Log::LogError(PRM("Bad Part type for ADD: '"), text, "'");
		return NULL;
	}

	int subtype = ARDJACK_USERPART_SUBTYPE_NONE;
	int pin = 0;

	if (values->Count >= 2)
	{
		subtype = Globals::PartMgr->LookupSubtype(values->Get(1), true);

		if ((type == ARDJACK_PART_TYPE_USER) && (subtype == ARDJACK_USERPART_SUBTYPE_NONE))
		{
			Log::LogError(PRM("Bad UserPart subtype for ADD: '"), text, "'");
			return NULL;
		}

		if (values->Count >= 3)
			pin = atoi(values->Get(2));
	}

	// Does 'name' already exist?
	Part* result = dev->LookupPart(name, true);

	if (NULL == result)
	{
		// Create a new Part.
		result = dev->AddPart(name, type, subtype, pin);

		if (NULL == result)
		{
			Log::LogError(PRM("Failed to add Part: '"), text, "'");
			return NULL;
		}
	}
	else
	{
		// Update the existing Part.
		result->Type = type;
		result->Subtype = subtype;
		result->Pin = pin;
	}

	return result;
}


bool DeviceManager::ExecuteDeviceOperation(Device* dev, const char* text, int oper, char* aName, StringList* values)
{
	if (Globals::Verbosity > 4)
	{
		if (values->Count == 0)
			Log::LogInfoF(PRM("ExecuteDeviceOperation: Request = oper %d, name '%s'"), oper, aName);
		else
			Log::LogInfoF(PRM("ExecuteDeviceOperation: Request = oper %d, name '%s', values[0] '%s', %d values"),
				oper, aName, values->Get(0), values->Count);
	}

	switch (oper)
	{
	case ARDJACK_OPERATION_ACTIVATE:
	{
		dev->SetActive(true);

		char temp[10];
		strcpy(temp, dev->Active() ? "1" : "0");

		return dev->SendResponse(oper, "", temp);
	}

	case ARDJACK_OPERATION_ADD:
	{
		Part* part = AddPart(dev, text, aName, values);

		return (NULL != part);
	}

	case ARDJACK_OPERATION_BEEP:
	{
		int dur_ms = 400;
		int freq_hz = 1000;

		if (values->Count >= 1)
		{
			freq_hz = Utils::String2Int(values->Get(0), freq_hz);

			if (values->Count >= 2)
				dur_ms = Utils::String2Int(values->Get(1), dur_ms);
		}

		//device->DoBeep(index, freq_hz, durMs);

		return dev->DoBeep(0, freq_hz, dur_ms);
	}

	case ARDJACK_OPERATION_CLEAR:
	{
		// Clear the inventory.
		return dev->ClearInventory();
	}

	case ARDJACK_OPERATION_CONFIGURE:
	{
		// Configure Device 'dev'.
		if (values->Count == 0)
			return true;

		return dev->Configure(aName, values, 0, values->Count);
	}

	case ARDJACK_OPERATION_CONFIGUREPART:
	{
		// Configure Part or Part type 'aName' of Device 'dev'.
		if (values->Count == 0)
			return true;

		return dev->Configure(aName, values, 0, values->Count);
	}

	case ARDJACK_OPERATION_DEACTIVATE:
	{
		dev->SetActive(false);

		char temp[10];
		strcpy(temp, dev->Active() ? "1" : "0");

		return dev->SendResponse(oper, "", temp);
	}

	case ARDJACK_OPERATION_FLASH:
	{
		int durMs = 20;

		if (strlen(aName) == 0)
			strcpy(aName, "led0");

		if (values->Count >= 1)
			durMs = Utils::String2Int(values->Get(0), durMs);

		return dev->DoFlash(aName, durMs);
	}

	case ARDJACK_OPERATION_GET_COUNT:
	{
		if (strlen(aName) == 0)
		{
			Log::LogError(PRM("No Part type for GETCOUNT operation: '"), text, "'");
			return false;
		}

		int partType = Globals::PartMgr->LookupType(aName);
		if (partType == ARDJACK_PART_TYPE_NONE)
			return false;

		int partCount = dev->GetCount(partType);

		return dev->SendResponse(oper, aName, Utils::Int2String(partCount));
	}

	case ARDJACK_OPERATION_GET_INFO:
	{
		char temp[82];
		sprintf(temp, "'%s' '%s' '%s'", dev->DeviceClass, dev->DeviceType, dev->DeviceVersion);

		return dev->SendResponse(oper, "", temp);
	}

	case ARDJACK_OPERATION_GET_INVENTORY:
	{
		// Any subsequent argument -> 'include zero counts = true'.
		bool includeZeroCounts = (values->Count > 0);

		return dev->SendInventory(includeZeroCounts);
	}

	case ARDJACK_OPERATION_READ:
	{
		Part* part = dev->LookupPart(aName);

		if (NULL == part)
			return false;

		Dynamic value;
		dev->Read(part, &value);

		char temp[20];
		value.AsString(temp);

		return dev->SendResponse(oper, part->Name, temp);
	}

	case ARDJACK_OPERATION_SUBSCRIBE:
	{
		// Subscribe to (send notifications re.) a Part or Part type, e.g.
		//		device ard subscribe di0
		//		device ard subscribe di
		return SubscribePart(dev, aName, true);
	}

	case ARDJACK_OPERATION_UNSUBSCRIBE:
	{
		// Unsubscribe from (don't send notifications re.) a Part or Part type, e.g.
		//		device ard unsubscribe di0
		//		device ard unsubscribe di
		return SubscribePart(dev, aName, false);
	}

	case ARDJACK_OPERATION_WRITE:
	{
		int partType;

		Part* part = dev->LookupPart(aName, true);

		if (NULL == part)
		{
			partType = Globals::PartMgr->LookupType(aName, true);

			if (partType == ARDJACK_PART_TYPE_NONE)
			{
				Log::LogErrorF(PRM("ExecuteDeviceOperation: Device '%s' - no such Part or Part type: '%s'"), dev->Name, aName);
				return false;
			}
		}

		Dynamic value;

		if (values->Count > 0)
			value.SetString(values->Get(0));

		if (NULL != part)
		{
			// A Part was specified.
			if (!dev->Write(part, &value))
			{
				Log::LogErrorF(PRM("ExecuteDeviceOperation: Failed to Write, Device '%s', Part '%s', value '%s'"),
					dev->Name, part->Name, value.String());
				return false;
			}
		}
		else
		{
			// A Part type was specified.
			for (int i = 0; i < dev->PartCount; i++)
			{
				part = dev->Parts[i];

				if (part->Type == partType)
				{
					if (!dev->Write(part, &value))
					{
						Log::LogErrorF(PRM("ExecuteDeviceOperation: Failed to Write, Device '%s', Part '%s', value '%s'"),
							dev->Name, part->Name, value.String());
						return false;
					}
				}
			}
		}

		return true;
	}

	default:
		Log::LogWarning(PRM("ExecuteDeviceOperation: DEVICE operation ignored: '"), text, "'");
		return true;
	}
}


bool DeviceManager::HandleDeviceRequest(Device* dev, const char* text)
{
	// Handle a Device Request, which probably results in executing a Device Operation.
	if (NULL == dev)
	{
		Log::LogWarning(PRM("HandleDeviceRequest: No Device"));
		return false;
	}

	if (Globals::Verbosity > 4)
		Log::LogInfo(PRM("HandleDeviceRequest: Device '"), dev->Name, "', '", text, "'");

	char aName[ARDJACK_MAX_NAME_LENGTH];
	StringList values;

	int oper = dev->DeviceCodec->DecodeRequest(text, aName, &values);

	if (oper == ARDJACK_OPERATION_NONE)
	{
		Log::LogError(PRM("HandleDeviceRequest: Device '"), dev->Name, PRM("', - invalid operation: '"), text, "'");
		return false;
	}

	return ExecuteDeviceOperation(dev, text, oper, aName, &values);
}


bool DeviceManager::Interact(const char* text)
{
	// E.g.
	//	"ard !led0"
	//	"win scan count delay_ms"
	//	"win getinventory"
	//	"ard poll"
	//	"win !led0 1"
	//	"win set switch0 1"		(DEVELOPMENT)

	// Get the Device.
	char devName[ARDJACK_MAX_NAME_LENGTH];
	const char* remainder;

	Utils::GetArgs(text, devName, &remainder);

	Device* dev = Globals::DeviceMgr->LookupDevice(devName);
	if (NULL == dev)
	{
		Log::LogError(PRM("No such Device: "), devName);
		return false;
	}

	// Handle it in 'InteractAction'?
	bool handled = false;

	InteractAction(dev, remainder, &handled);
	if (handled) return true;

	// Maybe it's a Device command?
	return HandleDeviceRequest(dev, remainder);
}


bool DeviceManager::InteractAction(Device* dev, const char* text, bool* handled)
{
	// Check if 'text' is something we can handle here, otherwise it's presumably a Device Operation.
	*handled = false;

	// Split 'text' into a action and arguments.
	StringList fields;

	int count = Utils::SplitText(text, ' ', &fields, ARDJACK_MAX_VALUES, ARDJACK_MAX_VALUE_LENGTH);
	if (count < 1)
		return false;

	// Get the action.
	char action[20];
	strcpy(action, fields.Get(0));
	_strupr(action);

	if (Utils::StringEquals(action, "GETINVENTORY", true))
	{
		// Any subsequent argument -> 'include zero counts = true'.
		bool includeZeroCounts = (count >= 2);
		*handled = true;

		return dev->SendInventory(includeZeroCounts);
	}

	if (Utils::StringEquals(action, "POLL", true))
	{
		*handled = true;

		return dev->Poll();
	}

	if (Utils::StringEquals(action, "SCAN", true))
	{
		bool changes;
		*handled = true;

		// Default = scan for 20 seconds at 2 Hz.
		int scans = 40;
		int delay_ms = 500;

		if (count >= 2)
		{
			scans = Utils::String2Int(fields.Get(1), scans);

			if (count >= 3)
				delay_ms = Utils::String2Int(fields.Get(2), delay_ms);
		}

		return dev->ScanInputs(&changes, scans, delay_ms);
	}

	if (Utils::StringEquals(action, "SET", true))
	{
		// Set a Part's current value (DEVELOPMENT).
		// E.g.
		//		win set switch0 1
		*handled = true;

		if (count < 2)
			return false;

		char partName[ARDJACK_MAX_NAME_LENGTH];
		strcpy(partName, fields.Get(1));

		Part* part = dev->LookupPart(partName);

		if (NULL == part)
		{
			Log::LogError(PRM("No such Part: "), partName);
			return false;
		}

		if (count >= 3)
			part->Value.SetString(fields.Get(2));
		else
			part->Value.SetString("");

		return true;
		}

	if (Utils::StringEquals(action, "SPECIAL", true))
	{
		*handled = true;

		if (count < 2)
			return false;

		int action = Utils::String2Int(fields.Get(1));
		dev->Special(action);

		return true;
	}

	return true;
}


Device* DeviceManager::LookupDevice(const char* name)
{
	IoTObject* obj = Globals::ObjectRegister->LookupName(name);
	if (NULL == obj) return NULL;

	if (obj->Type != ARDJACK_OBJECT_TYPE_DEVICE) return NULL;

	return (Device*)obj;
}


int DeviceManager::LookupSubtype(const char* name)
{
	return Subtypes->LookupName(name, ARDJACK_OBJECT_SUBTYPE_UNKNOWN);
}



bool DeviceManager::SubscribePart(Device* dev, char* aName, bool newState)
{
	// 'aName' can be a PART name, a PART TYPE name, or "ALL" / "*" / "ALLIN".
	Part* parts[ARDJACK_MAX_PARTS];
	uint8_t count;

	if (!dev->GetParts(aName, parts, &count))
		return false;

	// Subscribe/unsubscribe all Parts.
	for (int i = 0; i < count; i++)
	{
		Part* part = parts[i];

		// Only subscribe if the Part's an input.
		if (part->IsInput())
			dev->SetNotify(part, newState);
	}

	// Send a response.
	int oper = newState ? ARDJACK_OPERATION_SUBSCRIBED : ARDJACK_OPERATION_UNSUBSCRIBED;

	return dev->SendResponse(oper, aName, Utils::Bool210(newState));
}

