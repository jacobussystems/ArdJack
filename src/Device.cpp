/*
	Device.cpp

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
	#define _strlwr strlwr
#else
	#include "stdafx.h"
	#include <typeinfo>
#endif

#include "Connection.h"
#include "Device.h"
#include "DeviceCodec1.h"
#include "Dictionary.h"
#include "Dynamic.h"
#include "Enumeration.h"
#include "FieldReplacer.h"
#include "Globals.h"
#include "IoTMessage.h"
#include "IoTObject.h"
#include "Log.h"
#include "Part.h"
#include "PartManager.h"
#include "Route.h"
#include "Shield.h"
#include "ShieldManager.h"
#include "StringList.h"
#include "Utils.h"




Device::Device(const char* name)
	: IoTObject(name)
{
	_IsOpen = false;
	_MessageFormat = ARDJACK_MESSAGE_FORMAT_1;
	strcpy(_MessagePrefix, "$rem0:");
	strcpy(_MessageToPath, "\\\\pcname\\rem0");							// a UNC-like path: "\\computer\resource"

#ifdef ARDJACK_INCLUDE_SHIELDS
	DeviceShield = NULL;
	_ShieldName[0] = NULL;
#endif

	strcpy(DeviceClass, "Device");
	DeviceCodec = Globals::DeviceCodec;
	strcpy(DeviceType, "-");
	strcpy(DeviceVersion, "-");
	InputConnection = NULL;
	InputTimeout = 500;													// ms
	OutputConnection = NULL;
	PartCount = 0;
	ReadEvents = 0;
	WriteEvents = 0;

	for (int i = 0; i < ARDJACK_MAX_PARTS; i++)
		Parts[i] = NULL;
}


Device::~Device()
{
	ClearInventory();

#ifdef ARDJACK_INCLUDE_SHIELDS
	if (NULL != DeviceShield)
	{
		Globals::ObjectRegister->DeleteObject(DeviceShield);
		DeviceShield = NULL;
	}
#endif
}


bool Device::Activate()
{
	if (Globals::Verbosity > 3)
		Log::LogInfoF(PRM("Device::Activate: '%s'"), Name);

	InputConnection = NULL;
	OutputConnection = NULL;

	if (!ValidateConfig())
		return false;

	if (!ApplyConfig())
		return false;

	if (!_IsOpen)
	{
		// Try to 'open' this Device.
		if (!Open())
			return false;
	}

	return true;
}


bool Device::AddConfig()
{
	if (!IoTObject::AddConfig())
		return false;

	if (Globals::Verbosity > 3)
		Log::LogInfo(PRM("Device::AddConfig: "), Name);

	Config->AddStringProp("Input", "Input Connection name.");
	Config->AddStringProp("Output", "Output Connection name.");
	Config->AddIntegerProp("MessageFormat", "Message format (0 or 1).", _MessageFormat);
	Config->AddStringProp("MessagePrefix", "Message prefix (when message format = 0).", _MessagePrefix);
	Config->AddStringProp("MessageTo", "Message 'to' path (when message format = 1).", _MessageToPath);

#ifdef ARDJACK_INCLUDE_SHIELDS
	Config->AddStringProp("Shield", "Shield name (AMFS | Thinker).");
#endif

	return Config->SortItems();
}


Part* Device::AddPart(const char* name, int type, int subtype, int pin)
{
	// Is there an existing Part 'name' with the same Type and (for UserParts) Subtype?
	Part* result = LookupPart(name, type, subtype, true);

	if (NULL != result)
	{
		// Yes - reuse the old Part.
		if (Globals::Verbosity > 4)
			Log::LogInfoF(PRM("Device::AddPart: %s: Reusing Part '%s' (type %d, subtype %d)"), Name, name, type, subtype);

		result->IsNew = true;
		result->Pin = pin;
		result->Subtype = subtype;
	}
	else
	{
		// No - create a new Part.
		result = Globals::PartMgr->CreatePart(name, type, subtype);
		if (NULL == result) return NULL;

		Parts[PartCount++] = result;

		strcpy(result->Name, name);
		result->Pin = pin;
		result->Subtype = subtype;
		result->Type = type;

		result->AddConfig();
	}

	return result;
}


bool Device::AddParts(const char* prefix, int count, int type, int subtype, int startIndex, int startPin)
{
	char name[ARDJACK_MAX_NAME_LENGTH];

	int index = startIndex;
	int pin = startPin;

	for (int i = 0; i < count; i++)
	{
		sprintf(name, "%s%d", prefix, index++);
		AddPart(name, type, subtype, pin++);
	}

	return true;
}


bool Device::ApplyConfig(bool quiet)
{
	// N.B. DON'T call the base class as the methods may overlap.
	if (Globals::Verbosity > 4)
		Log::LogInfo(PRM("Device::ApplyConfig: "), Name);

	char inputName[ARDJACK_MAX_NAME_LENGTH];
	char outputName[ARDJACK_MAX_NAME_LENGTH];

	Config->GetAsString("Input", inputName);
	Config->GetAsInteger("MessageFormat", &_MessageFormat);
	Config->GetAsString("MessagePrefix", _MessagePrefix);
	Config->GetAsString("MessageTo", _MessageToPath);
	Config->GetAsString("Output", outputName);

#ifdef ARDJACK_INCLUDE_SHIELDS
	Config->GetAsString("Shield", _ShieldName);
#endif

	// Any input Connection specified?
	if (strlen(inputName) > 0)
		InputConnection = (Connection*)Globals::ObjectRegister->LookupName(inputName);

	// Any output Connection specified?
	if (strlen(outputName) > 0)
		OutputConnection = (Connection*)Globals::ObjectRegister->LookupName(outputName);

#ifdef ARDJACK_INCLUDE_SHIELDS
	if (!ConfigureForShield())
		return false;
#endif

	if (NULL != InputConnection)
	{
		if (Globals::Verbosity > 4)
			Log::LogInfoF(PRM("Device::ApplyConfig: %s, adding Routes for '%s'"), Name, InputConnection->Name);

		// Setup a route for requests and responses.
		char routeName[50];
		sprintf(routeName, PRM("Device_%s"), Name);
		Route* route = InputConnection->AddRoute(routeName, ARDJACK_ROUTE_TYPE_REQUEST, Globals::DeviceBuffer, "");
		route->Target = this;
		MessageFilter* msgFilter = &route->Filter;

		// Setup the 'Text' filter for 'Format 0' messages, using the Device name.
		MessageFilterItem* textFilter = &msgFilter->TextFilter;
		textFilter->Operation = ARDJACK_STRING_COMPARE_STARTS_WITH;
		sprintf(textFilter->Text, "$%s:", Name);

		// Setup a 'To' filter for 'Format 1' messages, using the Device name.
		MessageFilterItem* toFilter = &msgFilter->ToFilter;
		toFilter->Operation = ARDJACK_STRING_COMPARE_ENDS_WITH;
		sprintf(toFilter->Text, "\\%s", Name);

		// Activate the input Connection.
		InputConnection->SetActive(true);
	}

	if (NULL != OutputConnection)
	{
		// Activate the output Connection.
		OutputConnection->SetActive(true);
	}

	return true;
}


bool Device::CheckInput(Part* part, bool* change)
{
	// Read input 'part' and check for a change, notifying if required.
	*change = false;

	if (!part->Notifying)
		return true;
		
	if (!part->IsInput())
		return true;

	Dynamic newValue;

	if (!Read(part, &newValue))
		return false;

	if (part->CheckChange())
	{
		*change = true;
		SignalChange_Value(part);
	}

	return true;
}


bool Device::ClearInventory()
{
	if (Globals::Verbosity > 3)
		Log::LogInfoF(PRM("Device::ClearInventory: '%s', %d Parts"), Name, PartCount);

	for (int i = 0; i < PartCount; i++)
	{
		if (NULL != Parts[i])
			delete Parts[i];
	}

	PartCount = 0;

	for (int i = 0; i < ARDJACK_MAX_PARTS; i++)
		Parts[i] = NULL;

	return true;
}


bool Device::Close()
{
	if (_IsOpen)
	{
		if (Globals::Verbosity > 3)
			Log::LogInfoF(PRM("Device::Close: '%s'"), Name);

		if (!CloseParts())
			return false;

		// Remove any Routes we added to the input Connection.
		if (NULL != InputConnection)
		{
			char temp[50];
			sprintf(temp, PRM("Device_%s"), Name);

			InputConnection->RemoveRoute(temp);
		}

		// Deactivate the input Connection?
		// NO, not for now.

		_IsOpen = false;
	}

	return true;
}


bool Device::CloseParts()
{
	for (int i = 0; i < PartCount; i++)
	{
		Part* part = Parts[i];

		if (!part->Deactivate())
		{
			Log::LogErrorF(PRM("'%s': Part '%s' failed to deactivate"), Name, part->Name);
			return false;
		}
	}

	return true;
}


bool Device::Configure(const char* partExpr, StringList* settings, int start, int count)
{
	// 'partExpr', if defined, specifies a Part, Part type or a special Part type.
	if (strlen(partExpr) == 0)
		return IoTObject::Configure(partExpr, settings, start, count);

	return ConfigureParts(partExpr, settings, start, count);
}


#ifdef ARDJACK_INCLUDE_SHIELDS

bool Device::ConfigureForShield()
{
	if (Globals::Verbosity > 3)
		Log::LogInfoF(PRM("Device::ConfigureForShield: '%s', Shield '%s'"), Name, _ShieldName);

	bool useShield = true;
	int shieldType = ARDJACK_OBJECT_SUBTYPE_UNKNOWN;

	if (strlen(_ShieldName) == 0)
		useShield = false;
	else
	{
		shieldType = Globals::ShieldMgr->LookupSubtype(_ShieldName);

		if (shieldType == ARDJACK_OBJECT_SUBTYPE_UNKNOWN)
			useShield = false;
		else
		{
			// Update '_ShieldName'.
			strcpy(_ShieldName, Globals::ShieldMgr->Subtypes->LookupValue(shieldType));
		}
	}

	// Remove the old Shield?
	if (NULL != DeviceShield)
	{
		if (DeviceShield->Subtype != shieldType)
		{
			if (Globals::Verbosity > 3)
				Log::LogInfoF(PRM("Device::ConfigureForShield: '%s', deleting Shield '%s'"), Name, DeviceShield->Name);

			Globals::ObjectRegister->DeleteObject(DeviceShield);
			DeviceShield = NULL;

			// We've removed an active Shield.
			if (!useShield)
			{
				// We're not going to use a Shield now, so reset the inventory.
				if (Globals::Verbosity > 3)
					Log::LogInfoF(PRM("Device::ConfigureForShield: '%s', resetting inventory"), Name);

				return CreateDefaultInventory();
			}
		}
	}

	// Create a new Shield?
	if (useShield)
	{
		if (NULL == DeviceShield)
		{
			if (Globals::Verbosity > 3)
				Log::LogInfoF(PRM("Device::ConfigureForShield: '%s', creating a '%s' Shield"), Name, _ShieldName);

			DeviceShield = (Shield*)Globals::AddObject(ARDJACK_OBJECT_TYPE_SHIELD, shieldType, _ShieldName);
			if (NULL == DeviceShield) return false;

			DeviceShield->Owner = this;
		}
		else
		{
			if (Globals::Verbosity > 3)
				Log::LogInfoF(PRM("Device::ConfigureForShield: '%s', reusing Shield '%s'"), Name, DeviceShield->Name);
		}

		DeviceShield->SetActive(true);
		DeviceShield->CreateInventory();
	}

	return true;
}

#endif


bool Device::ConfigureParts(const char* partExpr, StringList* settings, int start, int count)
{
	// Modify partExpr's configuration from 'settings'.
	if (count == 0)
		return true;

	if (Globals::Verbosity > 3)
		Log::LogInfoF(PRM("Device::ConfigureParts: '%s', partExpr '%s'"), Name, partExpr);

	Part* parts[ARDJACK_MAX_PARTS];
	uint8_t partCount;

	GetParts(partExpr, parts, &partCount);

	if (partCount == 0)
	{
		Log::LogErrorF(PRM("%s: Neither a Part nor a Part type: '%s'"), Name, partExpr);
		return false;
	}

	// Deactivate - Configure - Activate.
	bool wasActive = Active();

	// Deactivate the Device?
	if (!SetActive(false))
		return false;

	for (int i = 0; i < partCount; i++)
	{
		if (!parts[i]->Configure(this, settings, start, count))
			break;
	}

	// Reactivate the Device?
	if (!SetActive(wasActive))
		return false;

	return true;
}


bool Device::CreateDefaultInventory()
{
	return true;
}


bool Device::Deactivate()
{
	if (Globals::Verbosity > 3)
		Log::LogInfoF(PRM("Device::Deactivate: '%s'"), Name);

	Close();

	return true;
}


bool Device::DoBeep(int index, int freq_hz, int dur_ms)
{
	return true;
}


bool Device::DoFlash(const char* name, int durMs)
{
	Part* part = LookupPart(name);
	if (NULL == part) return false;

	Dynamic value;
	value.SetBool(true);
	if (!Write(part, &value)) return false;

	Utils::DelayMs(durMs);

	value.SetBool(false);
	if (!Write(part, &value)) return false;

	return true;
}


int Device::GetCount(int partType)
{
	int count = 0;

	for (int i = 0; i < PartCount; i++)
	{
		Part *part = Parts[i];

		if (part->Type == partType)
			count++;
	}

	return count;
}


bool Device::GetParts(const char* expr, Part* parts[], uint8_t* count, bool quiet)
{
	// 'expr' is one of:
	//		A special case like "All", "*", "AllIn", etc.
	//		A Part's name.
	//		A Part type.
	// Populate 'parts' with the corresponding Part(s), and 'count' with the count.

	*count = 0;

	// Special case?
	char useExpr[20];
	strncpy(useExpr, expr, 20);
	useExpr[strlen(expr)] = NULL;
	Utils::Trim(useExpr);
	strlwr(useExpr);

	if (Utils::StringEquals(useExpr, "all", false) || Utils::StringEquals(useExpr, "*", false))
	{
		for (int i = 0; i < PartCount; i++)
			parts[(*count)++] = Parts[i];

		return true;
	}

	if (Utils::StringEquals(useExpr, "allanalog", false))
	{
		for (int i = 0; i < PartCount; i++)
		{
			Part* part = Parts[i];

			if (Globals::PartMgr->IsAnalogType(part->Type))
				parts[(*count)++] = part;
		}

		return true;
	}

	if (Utils::StringEquals(useExpr, "allanalogin", false))
	{
		for (int i = 0; i < PartCount; i++)
		{
			Part* part = Parts[i];

			if (Globals::PartMgr->IsAnalogType(part->Type) && Globals::PartMgr->IsInputType(part->Type))
				parts[(*count)++] = part;
		}

		return true;
	}

	if (Utils::StringEquals(useExpr, "alldigital", false))
	{
		for (int i = 0; i < PartCount; i++)
		{
			Part* part = Parts[i];

			if (Globals::PartMgr->IsDigitalType(part->Type))
				parts[(*count)++] = part;
		}

		return true;
	}

	if (Utils::StringEquals(useExpr, "alldigitalin", false))
	{
		for (int i = 0; i < PartCount; i++)
		{
			Part* part = Parts[i];

			if (Globals::PartMgr->IsDigitalType(part->Type) && Globals::PartMgr->IsInputType(part->Type))
				parts[(*count)++] = part;
		}

		return true;
	}

	if (Utils::StringEquals(useExpr, "alldigitalout", false))
	{
		for (int i = 0; i < PartCount; i++)
		{
			Part* part = Parts[i];

			if (Globals::PartMgr->IsDigitalType(part->Type) && Globals::PartMgr->IsOutputType(part->Type))
				parts[(*count)++] = part;
		}

		return true;
	}

	if (Utils::StringEquals(useExpr, "allin", false))
	{
		for (int i = 0; i < PartCount; i++)
		{
			Part* part = Parts[i];

			if (Globals::PartMgr->IsInputType(part->Type))
				parts[(*count)++] = part;
		}

		return true;
	}

	if (Utils::StringEquals(useExpr, "allinout", false))
	{
		for (int i = 0; i < PartCount; i++)
		{
			Part* part = Parts[i];

			if (Globals::PartMgr->IsInputType(part->Type) || Globals::PartMgr->IsOutputType(part->Type))
					parts[(*count)++] = part;
		}

		return true;
	}

	if (Utils::StringEquals(useExpr, "allout", false))
	{
		for (int i = 0; i < PartCount; i++)
		{
			Part* part = Parts[i];

			if (Globals::PartMgr->IsOutputType(part->Type))
				parts[(*count)++] = part;
		}

		return true;
	}

	if (Utils::StringEquals(useExpr, "firstbuttonorswitch", false))
	{
		for (int i = 0; i < PartCount; i++)
		{
			Part* part = Parts[i];

			if ((part->Type == ARDJACK_PART_TYPE_BUTTON) || (part->Type == ARDJACK_PART_TYPE_SWITCH))
			{
				parts[(*count)++] = part;
				break;
			}
		}

		if ((*count) == 0)
		{
			// Nothing found - fall back to 'firstdigitalin'.
			return GetParts("firstdigitalin", parts, count, quiet);
		}

		return true;
	}

	if (Utils::StringEquals(useExpr, "firstdigitalin", false))
	{
		for (int i = 0; i < PartCount; i++)
		{
			Part* part = Parts[i];

			if (Globals::PartMgr->IsDigitalType(part->Type) && Globals::PartMgr->IsInputType(part->Type))
			{
				parts[(*count)++] = part;
				break;
			}
		}

		return true;
	}

	// Part name?
	Part* part = LookupPart(useExpr, true);

	if (NULL != part)
	{
		parts[(*count)++] = part;
		return true;
	}

	// Part type?
	int partType = Globals::PartMgr->LookupType(useExpr);

	if (partType != ARDJACK_PART_TYPE_NONE)
		return GetPartsOfType(partType, parts, count);

	if (!quiet)
		Log::LogErrorF(PRM("GetParts: Expression '%s' returned no Parts"), expr);

	return true;
}


bool Device::GetPartsOfType(int partType, Part* parts[], uint8_t* count)
{
	// From 'partType', populate 'parts' with the Parts of that type, and 'count' with the count.
	*count = 0;

	for (int i = 0; i < PartCount; i++)
	{
		Part* part = Parts[i];

		if (part->Type == partType)
			parts[(*count)++] = part;
	}

	return true;
}


int Device::LookupOperation(const char* name)
{
	char useName[ARDJACK_MAX_NAME_LENGTH];
	strcpy(useName, name);
	_strlwr(useName);

	if (strcmp(useName, PRM("activate")) == 0)
		return ARDJACK_OPERATION_ACTIVATE;

	if (strcmp(useName, PRM("add")) == 0)
		return ARDJACK_OPERATION_ADD;

	if (strcmp(useName, PRM("beep")) == 0)
		return ARDJACK_OPERATION_BEEP;

	if (strcmp(useName, PRM("clear")) == 0)
		return ARDJACK_OPERATION_CLEAR;

	if (strcmp(useName, PRM("configure")) == 0)
		return ARDJACK_OPERATION_CONFIGURE;

	if (strcmp(useName, PRM("configurepart")) == 0)
		return ARDJACK_OPERATION_CONFIGUREPART;

	if (strcmp(useName, PRM("deactivate")) == 0)
		return ARDJACK_OPERATION_DEACTIVATE;

	if (strcmp(useName, PRM("flash")) == 0)
		return ARDJACK_OPERATION_FLASH;

	if (strcmp(useName, PRM("error")) == 0)
		return ARDJACK_OPERATION_ERROR;

	if (strcmp(useName, PRM("getconfig")) == 0)
		return ARDJACK_OPERATION_GET_CONFIG;

	if (strcmp(useName, PRM("getcount")) == 0)
		return ARDJACK_OPERATION_GET_COUNT;

	if (strcmp(useName, PRM("getglobal")) == 0)
		return ARDJACK_OPERATION_GET_GLOBAL;

	if (strcmp(useName, PRM("getinfo")) == 0)
		return ARDJACK_OPERATION_GET_INFO;

	if (strcmp(useName, PRM("getinventory")) == 0)
		return ARDJACK_OPERATION_GET_INVENTORY;

	if (strcmp(useName, PRM("getpartconfig")) == 0)
		return ARDJACK_OPERATION_GET_PART_CONFIG;

	if (strcmp(useName, PRM("reactivate")) == 0)
		return ARDJACK_OPERATION_REACTIVATE;

	if (strcmp(useName, PRM("read")) == 0)
		return ARDJACK_OPERATION_READ;

	if (strcmp(useName, PRM("setglobal")) == 0)
		return ARDJACK_OPERATION_SET_GLOBAL;

	if (strcmp(useName, PRM("subscribe")) == 0)
		return ARDJACK_OPERATION_SUBSCRIBE;

	if (strcmp(useName, PRM("unsubscribe")) == 0)
		return ARDJACK_OPERATION_UNSUBSCRIBE;

	if (strcmp(useName, PRM("update")) == 0)
		return ARDJACK_OPERATION_UPDATE;

	if (strcmp(useName, PRM("write")) == 0)
		return ARDJACK_OPERATION_WRITE;

	Log::LogWarningF(PRM("Unknown Device Operation: '%s'"), useName);

	return ARDJACK_OPERATION_NONE;
}


Part* Device::LookupPart(const char* name, bool quiet)
{
	Part* result = NULL;

	for (int i = 0; i < PartCount; i++)
	{
		Part* part = Parts[i];

		if (Utils::StringEquals(part->Name, name))
			return part;
	}

	if (!quiet)
		Log::LogErrorF(PRM("LookupPart: Device '%s' has no Part '%s'"), Name, name);

	return NULL;
}


Part* Device::LookupPart(const char* name, int type, int subtype, bool quiet)
{
	// Is there a Part called 'name', with type 'type' and (for UserParts) subtype 'subtype'?
	// If yes, returns it.
	// If no, removes any Part called 'name'.

	Part* result = NULL;

	// Copy the Parts info.
	int oldPartCount = PartCount;
	Part* oldParts[ARDJACK_MAX_PARTS];

	for (int i = 0; i < oldPartCount; i++)
		oldParts[i] = Parts[i];

	// Rebuild the Parts info.
	PartCount = 0;

	for (int i = 0; i < oldPartCount; i++)
	{
		Part* part = Parts[i];
		bool copy = true;

		if (Utils::StringEquals(part->Name, name))
		{
			if ((part->Type == type) && ((part->Type != ARDJACK_PART_TYPE_USER) || (part->Subtype == subtype)))
				result = part;
			else
				copy = false;
		}

		if (copy)
			Parts[PartCount++] = part;
	}

	if ((NULL == result) && !quiet)
		Log::LogErrorF(PRM("LookupPart: Device '%s' has no Part '%s'"), Name, name);

	return result;
}


bool Device::LookupParts(const char* names, Part* parts[], uint8_t* count)
{
	// From a space-separated list of part names in 'names', populate 'parts' with the Parts, and 'count' with the count.
	*count = 0;
	StringList fields;

	int fieldCount = Utils::SplitText(names, ' ', &fields, ARDJACK_MAX_VALUES, ARDJACK_MAX_VALUE_LENGTH);

	for (int i = 0; i < fieldCount; i++)
	{
		Part* part = LookupPart(fields.Get(i));

		if (NULL != part)
			parts[(*count)++] = part;
	}

	return true;
}


bool Device::Open()
{
	if (Globals::Verbosity > 3)
	{
#ifdef ARDJACK_INCLUDE_SHIELDS
		Log::LogInfoF(PRM("Device::Open: '%s', Shield '%s'"), Name, _ShieldName);
#else
		Log::LogInfoF(PRM("Device::Open: '%s'"), Name);
#endif
	}

	if (!OpenParts())
		return false;

	_IsOpen = true;

	return true;
}


bool Device::OpenParts()
{
	if (Globals::Verbosity > 3)
		Log::LogInfoF(PRM("Device::OpenParts: '%s'"), Name);

	Dynamic value;

	for (int i = 0; i < PartCount; i++)
	{
		Part* part = Parts[i];

		if (!part->Activate())
		{
			Log::LogErrorF(PRM("'%s': Part '%s' failed to activate"), Name, part->Name);
			return false;
		}

		value.Clear();

		Read(part, &value);
	}

	return true;
}


bool Device::Poll()
{
#ifdef ARDJACK_INCLUDE_SHIELDS
	// Using a shield?
	if (NULL != DeviceShield)
		DeviceShield->Poll();
#endif

	PollInputs();
	PollOutputs();
	PollParts();

	return true;
}


bool Device::PollInputs()
{
	bool changes;

	return ScanInputsOnce(&changes, true);
}


bool Device::PollOutputs()
{

	return true;
}


bool Device::PollParts()
{
	if (Globals::Verbosity > 6)
		Log::LogInfo(Name);

	for (int i = 0; i < PartCount; i++)
	{
		Part* part = Parts[i];

		if (part->Notifying)
			part->Poll();
	}

	return true;
}


bool Device::PrepareForCreateInventory()
{
	// Prepare to add Parts to this Device.
	// N.B. Don't clear the existing Parts, as we may be able to reuse them and keep their configuration, e.g. Filters.

	if (Globals::Verbosity > 3)
		Log::LogInfoF(PRM("Device::PrepareForCreateInventory: '%s', %d existing Parts"), Name, PartCount);

	// Mark the existing Parts.
	for (int i = 0; i < PartCount; i++)
	{
		Part* part = Parts[i];
		part->IsNew = false;
	}

	return true;
}


bool Device::Read(Part* part, Dynamic* value)
{
	value->Clear();

	if (Globals::Verbosity > 5)
		Log::LogInfoF(PRM("Device::Read: '%s', part '%s'"), Name, part->Name);

#ifdef ARDJACK_INCLUDE_SHIELDS
	// Using a shield?
	if (NULL != DeviceShield)
	{
		bool handled = false;

		// Offer this Read to the Shield.
		if (!DeviceShield->ReadPart(part, value, &handled))
			return false;

		if (handled)
			return true;
	}
#endif

	switch (part->Type)
	{
#ifdef ARDJACK_INCLUDE_MULTI_PARTS
	case ARDJACK_PART_TYPE_MULTI:
			return ReadMulti(part, value);
#endif

		default:
			return part->Read(value);
	}
}


#ifdef ARDJACK_INCLUDE_MULTI_PARTS

	bool Device::ReadMulti(Part* part, char* value)
	{
		// N.B. 'values' will contain one element per item in the Part, even if some item(s) are not readable.
		values->Add("0");

		StringList itemValues;

		for (int i = 0; i < part->Count; i++)
		{
			values->Put(i, "");

			Part *item = part->Items[i];
			itemValues.Put(0, "");

			if (Read(item, &itemValues))
				values->Put(i, itemValues.Get(0));
		}

		return true;
	}

#endif


bool Device::RemoveOldParts()
{
	// Remove and delete any Parts that don't have 'IsNew' set.
	if (Globals::Verbosity > 5)
		Log::LogInfoF(PRM("Device::RemoveOldParts: '%s': Entry, %d Parts"), Name, PartCount);

	// Copy the Parts info.
	Part* startParts[ARDJACK_MAX_PARTS];

	for (int i = 0; i < PartCount; i++)
		startParts[i] = Parts[i];

	// Rebuild the Parts info.
	int startPartCount = PartCount;
	PartCount = 0;

	for (int i = 0; i < startPartCount; i++)
	{
		Part* part = startParts[i];

		if (part->IsNew)
			Parts[PartCount++] = part;
		else
			delete part;
	}

	if (Globals::Verbosity > 5)
		Log::LogInfoF(PRM("Device::RemoveOldParts: '%s': Exit, %d Parts"), Name, PartCount);

	return true;
}


bool Device::ScanInputs(bool *changes, int count, int delay_ms)
{
	*changes = false;

	if (Globals::Verbosity > 5)
		Log::LogInfoF(PRM("ScanInputs: device '%s', count %d, delay %d ms"), Name, count, delay_ms);

	for (int i = 0; i < count; i++)
	{
		bool changed = false;

		if (!ScanInputsOnce(&changed))
			return false;

		if (changed)
			*changes = true;

		if (delay_ms > 0)
			Utils::DelayMs(delay_ms);
	}

	if (Globals::Verbosity > 5)
		Log::LogInfo(PRM("ScanInputs: Done"));

	return true;
}


bool Device::ScanInputsOnce(bool *changes, bool signal)
{
	// Scan input Parts, storing the latest values in the Parts and setting 'changes' if anything has changed.
	*changes = false;

	bool change;
	bool changeList[ARDJACK_MAX_PARTS];

	for (int i = 0; i < PartCount; i++)
	{
		Part *part = Parts[i];
		changeList[i] = false;

		if (part->Notifying)
		{
			CheckInput(part, &change);

			if (change)
			{
				*changes = true;
				changeList[i] = true;

				//if (signal)
				//	SignalChange_Value(part);
			}
		}
	}

	if (*changes)
	{
		if (Globals::Verbosity > 5)
		{
			char temp[200];
			temp[0] = NULL;

			for (int i = 0; i < PartCount; i++)
			{
				Part* part = Parts[i];

				if (changeList[i])
				{
					strcat(temp, " ");
					strcat(temp, part->Name);
				}
			}

			if (Globals::Verbosity > 5)
				Log::LogInfo(Name, PRM(": Parts changed:"), temp);
		}
	}

	return true;
}


bool Device::SendInventory(bool includePartConfig, bool includeZeroCounts)
{
	char temp[200];

	for (int partType = ARDJACK_MIN_PART_TYPE; partType <= ARDJACK_MAX_PART_TYPE; partType++)
	{
		int count = GetCount(partType);

		if ((count > 0) || includeZeroCounts)
		{
			itoa(count, temp, 10);

			for (int i = 0; i < PartCount; i++)
			{
				Part* part = Parts[i];

				if (part->Type == partType)
				{
					strcat(temp, " ");
					strcat(temp, part->Name);
				}
			}

			if (!SendResponse(ARDJACK_OPERATION_GET_COUNT, PartManager::GetPartTypeName(partType), temp))
				return false;
		}
	}

	if (includePartConfig)
	{
		for (int i = 0; i < PartCount; i++)
		{
			Part* part = Parts[i];

			if (!SendPartConfig(part))
				return false;
		}
	}

	return true;
}


bool Device::SendPartConfig(Part* part)
{
	char temp[140];

	return SendResponse(ARDJACK_OPERATION_GET_PART_CONFIG, part->Name, part->GetConfigStr(temp));
}


bool Device::SendResponse(int oper, const char* aName, const char* text)
{
	char response[120];

	DeviceCodec->EncodeResponse(response, oper, aName, text);

	if (strlen(response) == 0)
	{
		Log::LogErrorF(PRM("'%s': No response text"), Name);
		return false;
	}

	//if (NULL == request)
	//	Log::LogWarning("'", Name, PRM("': No request for response: '"), response, "'");

	if (NULL == OutputConnection)
	{
		Log::LogWarning("'", Name, PRM("': No output Connection to send response: '"), response, "'");
		return false;
	}

	if (!OutputConnection->Active())
	{
		Log::LogWarning("'", Name, PRM("': Output Connection '"), OutputConnection->Name,
			PRM("' isn't active - can't send response: '"), response, "'");
		return false;
	}

	char fromPath[ARDJACK_MAX_MESSAGE_PATH_LENGTH] = "";
	char returnPath[ARDJACK_MAX_MESSAGE_PATH_LENGTH] = "";
	char toPath[ARDJACK_MAX_MESSAGE_PATH_LENGTH] = "";

	strcpy(toPath, _MessageToPath);

	// TEMPORARY: ?
	sprintf(fromPath, "\\\\%s\\%s", Globals::ComputerName, Name);
	strcpy(fromPath, fromPath);
	strcpy(returnPath, fromPath);

	switch (_MessageFormat)
	{
	case ARDJACK_MESSAGE_FORMAT_0:
		char temp[140];
		strcpy(temp, _MessagePrefix);
		strcat(temp, " ");
		strcat(temp, response);

		IoTMessage::CreateMessageToSend(PRM("response"), _MessageFormat, temp, &_ResponseMsg, fromPath, toPath, returnPath);
		break;

	default:
		IoTMessage::CreateMessageToSend(PRM("response"), _MessageFormat, response, &_ResponseMsg, fromPath, toPath, returnPath);
		break;
	}

	return OutputConnection->OutputMessage(&_ResponseMsg);
}


bool Device::SetNotify(Part* part, bool state)
{
	// If 'state', send notifications when 'part' changes value.
	part->Notifying = state;

	if (Globals::Verbosity > 3)
		Log::LogInfoF(PRM("SetNotify: '%s', Part '%s' -> state %d"), Name, part->Name, state);

	return true;
}


bool Device::SetNotify(int partType, bool state)
{
	// If 'state', send notifications when any Parts of type 'PartType' change value.
	for (int i = 0; i < PartCount; i++)
	{
		Part* part = Parts[i];

		if (part->Type == partType)
			part->Notifying = state;
	}

	if (Globals::Verbosity > 2)
		Log::LogInfoF(PRM("SetNotify: '%s', Part type '%s' -> state %d"), Name, PartManager::GetPartTypeName(partType), state);

	return true;
}


bool Device::SignalChange_Configuration(Part* part)
{
	// Send a notification that the configuration of 'part' has changed.
	char temp[200];
	part->GetConfigStr(temp);

	return SendResponse(ARDJACK_OPERATION_CONFIGUREPART, part->Name, temp);
}


bool Device::SignalChange_Value(Part* part)
{
	// Send a notification that the value of 'part' has changed.
	char temp[ARDJACK_MAX_DYNAMIC_STRING_LENGTH];
	part->Value.AsString(temp);

	return SendResponse(ARDJACK_OPERATION_READ, part->Name, temp);
}


bool Device::Update()
{
	return false;
}


bool Device::ValidateConfig(bool quiet)
{
	if (Globals::Verbosity > 4)
		Log::LogInfo(PRM("Device::ValidateConfig"));

	if (!IoTObject::ValidateConfig(quiet))
		return false;

	return true;
}


bool Device::Write(Part* part, Dynamic* value)
{
	if (Globals::Verbosity > 5)
	{
		if (value->IsEmpty())
			Log::LogInfoF(PRM("Device::Write: '%s', part '%s', no value"), Name, part->Name);
		else
		{
			char temp[ARDJACK_MAX_DYNAMIC_STRING_LENGTH];
			Log::LogInfoF(PRM("Device::Write: '%s', part '%s', value '%s'"), Name, part->Name, value->AsString(temp));
		}
	}

#ifdef ARDJACK_INCLUDE_SHIELDS
	// Using a shield?
	if (NULL != DeviceShield)
	{
		bool handled = false;

		// Offer this Write to the Shield.
		if (!DeviceShield->WritePart(part, value, &handled))
			return false;

		if (handled)
			return true;
	}
#endif

	switch (part->Type)
	{
#ifdef ARDJACK_INCLUDE_MULTI_PARTS
	case ARDJACK_PART_TYPE_MULTI:
			return WriteMulti(part, value);
#endif

		default:
			value->Copy(&part->Value);
			break;
	}

	return true;
}


#ifdef ARDJACK_INCLUDE_MULTI_PARTS

	bool Device::WriteMulti(Part* part, const char* value)
	{
		// N.B. 'values' should contain one element per item in the Part, even if some item(s) are not writable.
		// If 'values' is empty, the 'Write' method is called with zero values.
		// If 'values' has fewer elements than there are items in the Part, the last element is used for
		// the remaining items.

		StringList itemValues;

		for (int i = 0; i < part->Count; i++)
		{
			Part *item = part->Items[i];

			if (values->Count == 0)
				Write(item, values);
			else
			{
				int j = Utils::MinInt(i, values->Count - 1);
				itemValues.Put(0, values->Get(j));

				Write(item, &itemValues);
			}
		}

		return true;
	}

#endif

