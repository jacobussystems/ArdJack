/*
	ShieldManager.cpp

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

#ifdef ARDUINO
	#define _strlwr strlwr
#else
	#include "stdafx.h"
	#include <typeinfo>
#endif

#ifdef ARDJACK_INCLUDE_SHIELDS
	#ifdef ARDUINO
		#ifdef ARDJACK_INCLUDE_ARDUINO_MF_SHIELD
			#include "ArduinoMFShield.h"
		#endif
	#endif
#endif

#include "Enumeration.h"
#include "IoTObject.h"
#include "Log.h"
#include "Shield.h"
#include "ShieldManager.h"
#include "ThinkerShield.h"
#include "Utils.h"



#ifdef ARDJACK_INCLUDE_SHIELDS

ShieldManager::ShieldManager()
	: IoTManager()
{
	if (Globals::Verbosity > 7)
		Log::LogInfo(PRM("ShieldManager ctor"));

	ObjectType = ARDJACK_OBJECT_TYPE_SHIELD;

	if (NULL == Subtypes)
	{
		Subtypes = new Enumeration(PRM("Subtypes"));
		Subtypes->Add(PRM("AMFS"), ARDJACK_SHIELD_SUBTYPE_ARDUINO_MF_SHIELD);
		Subtypes->Add(PRM("Thinker"), ARDJACK_SHIELD_SUBTYPE_THINKERSHIELD);
	}
}


Shield* ShieldManager::CreateShield(const char* name, int subtype)
{
	Shield* result = NULL;

	switch (subtype)
	{
#ifdef ARDUINO
	#ifdef ARDJACK_INCLUDE_ARDUINO_MF_SHIELD
		case ARDJACK_SHIELD_SUBTYPE_ARDUINO_MF_SHIELD:
			result = new ArduinoMFShield(name);
			break;
	#endif
#endif

	case ARDJACK_SHIELD_SUBTYPE_THINKERSHIELD:
		result = new ThinkerShield(name);
		break;
	}

	char temp[202];

	if (NULL == result)
	{
		sprintf(temp, PRM("Unable to create Shield '%s', subtype '%s'"), name,
			Register::ObjectSubtypeName(ARDJACK_OBJECT_TYPE_SHIELD, subtype));
		Log::LogError(temp);
		return NULL;
	}

	strcpy(result->Name, name);

	if (Globals::Verbosity > 5)
	{
		sprintf(temp, PRM("Created Shield '%s', subtype '%s')"), name,
			Register::ObjectSubtypeName(ARDJACK_OBJECT_TYPE_SHIELD, subtype));
		Log::LogInfo(temp);
	}

	return result;
}


int ShieldManager::LookupSubtype(const char* name)
{
	return Subtypes->LookupName(name, ARDJACK_OBJECT_SUBTYPE_UNKNOWN);
}

#endif
