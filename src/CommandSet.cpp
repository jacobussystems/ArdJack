/*
	CommandSet.cpp

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

#include "Beacon.h"
#include "BeaconManager.h"
#include "Bridge.h"
#include "BridgeManager.h"
#include "CmdInterpreter.h"
#include "DeviceCodec1.h"
#include "CommandSet.h"
#include "Connection.h"
#include "ConnectionManager.h"
#include "Device.h"
#include "DeviceManager.h"
#include "Displayer.h"
#include "Globals.h"
#include "IoTMessage.h"
#include "Log.h"
#include "Part.h"
#include "Utils.h"

#ifdef ARDJACK_INCLUDE_PERSISTENCE
	#include "PersistentFile.h"
	#include "PersistentFileManager.h"
#endif

#ifdef ARDJACK_NETWORK_AVAILABLE
	#include "NetworkInterface.h"
	#include "NetworkManager.h"
#endif

#ifdef ARDJACK_INCLUDE_TESTS
	#include "Tests.h"
#endif



CommandSet::CommandSet()
{
	if (Globals::Verbosity > 7)
		Log::LogInfo(PRM("CommandSet ctor"));

	_CommandCount = 0;

#ifdef ARDJACK_INCLUDE_PERSISTENCE
	_CurrentPersistentFile = NULL;
#endif

	for (int i = 0; i < ARDJACK_MAX_COMMANDS; i++)
		_Commands[i] = NULL;

	AddCommand(PRM("ACTIVATE"), &CommandSet::command_activate);
	AddCommand(PRM("ADD"), &CommandSet::command_add);
	AddCommand(PRM("BEEP"), &CommandSet::command_beep);
	AddCommand(PRM("CONFIGURE"), &CommandSet::command_configure);
	AddCommand(PRM("CONNECTION"), &CommandSet::command_connection);
	AddCommand(PRM("DEACTIVATE"), &CommandSet::command_deactivate);
	AddCommand(PRM("DEFINE"), &CommandSet::command_define);
	AddCommand(PRM("DELAY"), &CommandSet::command_delay);
	AddCommand(PRM("DELETE"), &CommandSet::command_delete);
	AddCommand(PRM("DEVICE"), &CommandSet::command_device);
	AddCommand(PRM("DISPLAY"), &CommandSet::command_display);
	AddCommand(PRM("EXIT"), &CommandSet::command_exit);
#ifdef ARDJACK_INCLUDE_PERSISTENCE
	AddCommand(PRM("FILE"), &CommandSet::command_file);
#endif
	AddCommand(PRM("HELP"), &CommandSet::command_help);
#ifdef ARDJACK_NETWORK_AVAILABLE
	AddCommand(PRM("NET"), &CommandSet::command_net);
#endif
	//AddCommand(PRM("PING"), &CommandSet::command_nyi);							//&command_ping);
	AddCommand(PRM("REPEAT"), &CommandSet::command_repeat);
	AddCommand(PRM("RESET"), &CommandSet::command_reset);
#ifdef ARDJACK_INCLUDE_PERSISTENCE
	AddCommand(PRM("RESTORE"), &CommandSet::command_restore);
	AddCommand(PRM("SAVE"), &CommandSet::command_save);
#endif
	AddCommand(PRM("SEND"), &CommandSet::command_send);
	AddCommand(PRM("SET"), &CommandSet::command_set);
#ifdef ARDJACK_INCLUDE_TESTS
	AddCommand(PRM("TEST"), &CommandSet::command_test);
	AddCommand(PRM("TESTS"), &CommandSet::command_tests);
#endif
}


CommandSet::~CommandSet()
{
	for (int i = 0; i < ARDJACK_MAX_COMMANDS; i++)
	{
		if (NULL != _Commands[i])
		{
			delete _Commands[i];
			_Commands[i] = NULL;
		}
	}
}


CommandInfo* CommandSet::AddCommand(const char* name, CommandSetCallback callback)
{
	if (_CommandCount >= ARDJACK_MAX_COMMANDS)
	{
		return NULL;
	}

	CommandInfo *result = _Commands[_CommandCount];

	if (NULL == result)
	{
		result = new CommandInfo();
		_Commands[_CommandCount] = result;
	}

	_CommandCount++;

	strcpy(result->Name, name);
	result->Callback = callback;

	return result;
}


bool CommandSet::Handle(const char* line, const char* verb, const char* remainder)
{
	if (_CommandCount == 0)
	{
		Log::LogWarning(PRM("Command Set has no commands: '"), line, "'");
		return false;
	}

	// Is this command in '_Commands'?
	if (strlen(verb) >= ARDJACK_MAX_VERB_LENGTH)
		return false;

	char ucVerb[ARDJACK_MAX_VERB_LENGTH];
	strcpy(ucVerb, verb);
	_strupr(ucVerb);

	bool (CommandSet::*pCallback)(const char* args);

	for (int i = 0; i < _CommandCount; i++)
	{
		CommandInfo* info = _Commands[i];

		if (strcmp(ucVerb, info->Name) == 0)
		{
			pCallback = info->Callback;

			if (pCallback == &CommandSet::command_nyi)
				Log::LogWarning("NOT YET IMPLEMENTED:", ucVerb);
			else
				(*this.*pCallback)(remainder);

			return true;
		}
	}

	return false;
}


bool CommandSet::command_activate(const char* args)
{
	// Activate zero or more 'IoTObjects'.
	if ((NULL == args) || (strlen(args) == 0))
	{
		Log::LogWarning(PRM("ACTIVATE command: No arguments"));
		return true;
	}

	return Globals::ActivateObject(args, true);
}


bool CommandSet::command_add(const char* args)
{
	// Add an 'IoTObject'.
	if ((NULL == args) || (strlen(args) == 0))
	{
		Log::LogWarning(PRM("ADD command: No arguments"));
		return true;
	}

	IoTObject* obj = Globals::AddObject(args);
	if (NULL == obj) return false;

	return true;
}


bool CommandSet::command_beep(const char* args)
{
	// beep 1800 200 3
	int freqHz = 1000;
	int durMs = 200;
	int beepCount = 1;

	char fields[3][ARDJACK_MAX_VALUE_LENGTH];
	int count = Utils::SplitText2Array(args, ' ', fields, 3, ARDJACK_MAX_VALUE_LENGTH);

	if (count >= 1)
		freqHz = Utils::String2Int(fields[0], freqHz);

	if (count >= 2)
		durMs = Utils::String2Int(fields[1], durMs);

	if (count >= 3)
		beepCount = Utils::String2Int(fields[2], beepCount);

	for (int i = 0; i < beepCount; i++)
		Utils::DoBeep(freqHz, durMs);

	return true;
}


bool CommandSet::command_configure(const char* args)
{
	// Configure an Object (Bridge, Connection, DataLogger or Device), or a Device Part.
	// Syntax:
	//		configure object item1=value1 item2=value2
	//		configure device.part item1=value1 item2=value2
	// E.g.
	//		'configure ard shield=thinker'
	//		'configure ard.button0 name=value'
	//		'configure udp0 inputport=2390'

	if ((NULL == args) || (strlen(args) == 0))
	{
		Log::LogWarning(PRM("CONFIGURE command: No arguments"));
		return true;
	}

	StringList fields;
	int count = Utils::SplitText(args, ' ', &fields, ARDJACK_MAX_VALUES, ARDJACK_MAX_VALUE_LENGTH);

	// TEMPORARY: why?
	if (count < 2)
	{
		Log::LogError(PRM("Configure: Less than 2 fields: "), args);
		return false;
	}

	// Parse the first field - is it in 'obj.entity' format?
	char fields2[2][ARDJACK_MAX_VALUE_LENGTH];
	int count2 = Utils::SplitText2Array(fields.Get(0), '.', fields2, 2, ARDJACK_MAX_VALUE_LENGTH);

	char objName[ARDJACK_MAX_NAME_LENGTH];
	char entity[ARDJACK_MAX_NAME_LENGTH];
	bool entitySyntax = (count2 > 1);

	if (entitySyntax)
	{
		strcpy(objName, fields2[0]);
		strcpy(entity, fields2[1]);
	}
	else
	{
		strcpy(objName, fields2[0]);
		entity[0] = NULL;
	}

	// Check the Object name.
	IoTObject* obj = Globals::ObjectRegister->LookupName(objName);
	if (NULL == obj)
	{
		Log::LogError(PRM("Configure: Unknown object: '"), objName, "'");
		return false;
	}

	if (Globals::Verbosity > 6)
	{
		char temp[102];
		Log::LogInfoF(PRM("Configure: '%s', entity '%s'"), obj->ToString(temp), entity); 
	}

	return obj->Configure(entity, &fields, 1, count - 1);
}


bool CommandSet::command_connection(const char* args)
{
	if ((NULL == args) || (strlen(args) == 0))
	{
		Log::LogWarning(PRM("CONNECTION command: No arguments"));
		return true;
	}

	return Globals::ConnectionMgr->Interact(args);
}


bool CommandSet::command_deactivate(const char* args)
{
	// Deactivate zero or more 'IoTObjects'.
	if ((NULL == args) || (strlen(args) == 0))
	{
		Log::LogWarning(PRM("DEACTIVATE command: No arguments"));
		return true;
	}

	return Globals::ActivateObject(args, false);
}


bool CommandSet::command_define(const char* args)
{
	// Define a Macro.
	if ((NULL == args) || (strlen(args) == 0))
	{
		Log::LogWarning(PRM("DEFINE command: No arguments"));
		return true;
	}

	char name[ARDJACK_MAX_NAME_LENGTH];
	const char* content = NULL;

	Utils::GetArgs(args, name, &content);

	if (strlen(content) == 0)
		Globals::Interpreter->RemoveMacro(name);
	else
		Globals::Interpreter->AddMacro(name, content);

	return true;
}


bool CommandSet::command_delay(const char* args)
{
	// Delay for a number of milliseconds.
	int delay_ms;
	char fields[2][ARDJACK_MAX_VALUE_LENGTH];

	int count = Utils::SplitText2Array(args, ' ', fields, 2, ARDJACK_MAX_VALUE_LENGTH);
	if (count == 0) return false;

	delay_ms = Utils::String2Int(fields[0], 1000);
	Utils::DelayMs(delay_ms);

	return true;
}


bool CommandSet::command_delete(const char* args)
{
	// Delete zero or more 'IoTObjects'.
	if ((NULL == args) || (strlen(args) == 0))
	{
		Log::LogWarning(PRM("DELETE command: No arguments"));
		return true;
	}

	return Globals::DeleteObject(args);
}


bool CommandSet::command_device(const char* args)
{
	if ((NULL == args) || (strlen(args) == 0))
	{
		Log::LogWarning(PRM("DEVICE command: No arguments"));
		return true;
	}

	return Globals::DeviceMgr->Interact(args);
}


bool CommandSet::command_display(const char* args)
{
	return Displayer::DisplayItem(args);
}


bool CommandSet::command_exit(const char* args)
{
	Globals::UserExit = true;

	return true;
}


#ifdef ARDJACK_INCLUDE_PERSISTENCE

bool CommandSet::command_file(const char* args)
{
	// file open settings						open file 'settings' for writing
	// file write name1=value1
	// file write "name2=value 2"
	// file close

	// file open startup						open file 'startup' for writing
	// file write "command1 yy zz"
	// file write "command2 yy zz"
	// file close

	// file clear								clears all files
	// file list								list the current files
	// file scan								scan the flash storage (Arduino) / disk folder (Windows)

	if ((NULL == args) || (strlen(args) == 0))
	{
		Log::LogWarning(PRM("FILE command: No arguments"));
		return true;
	}

	StringList fields;

	int count = Utils::SplitText(args, ' ', &fields, ARDJACK_MAX_VALUES, ARDJACK_MAX_VALUE_LENGTH);
	if (count == 0)
	{
		Log::LogError(PRM("FILE command: No arguments"));
		return false;
	}

	char action[20];
	strcpy(action, fields.Get(0));
	_strupr(action);

	if (Utils::StringEquals(action, "CLEAR", true))
	{
		return Globals::PersistentFileMgr->Clear();
	}

	if (Utils::StringEquals(action, "CLOSE", true))
	{
		if (NULL == _CurrentPersistentFile)
		{
			Log::LogError(PRM("FILE CLOSE command: No file open"));
			return false;
		}

		return _CurrentPersistentFile->Close();
	}

	if (Utils::StringEquals(action, "LIST", true))
	{
		return Globals::PersistentFileMgr->List();
	}

	if (Utils::StringEquals(action, "LOADINI", true))
	{
		// Load the specified INI file.
		if (count < 2)
		{
			Log::LogError(PRM("FILE LOAD command: Insufficient arguments in '"), args, "'");
			return false;
		}

		return Globals::PersistentFileMgr->LoadIniFile(fields.Get(1));
	}

	if (Utils::StringEquals(action, "OPEN", true))
	{
		if (count < 2)
		{
			Log::LogError(PRM("FILE OPEN command: Insufficient arguments in '"), args, "'");
			return false;
		}

		if (NULL != _CurrentPersistentFile)
		{
			_CurrentPersistentFile->Close();
			_CurrentPersistentFile = NULL;
		}

		char name[ARDJACK_MAX_NAME_LENGTH];
		strcpy(name, fields.Get(1));

		_CurrentPersistentFile = Globals::PersistentFileMgr->Lookup(name, true);
		if (NULL == _CurrentPersistentFile)
		{
			Log::LogError(PRM("FILE OPEN command: No such file: "), name);

			return false;
		}

		return _CurrentPersistentFile->Open("w");
	}

	if (Utils::StringEquals(action, "SCAN", true))
	{
#ifdef ARDUINO
		return Globals::PersistentFileMgr->Scan();
#else
		return Globals::PersistentFileMgr->Scan(Globals::AppDocsFolder);
#endif
	}

	if (Utils::StringEquals(action, "WRITE", true))
	{
		if (NULL == _CurrentPersistentFile)
		{
			Log::LogError(PRM("FILE WRITE command: No file open"));
			return false;
		}

		if (!_CurrentPersistentFile->IsOpen())
		{
			Log::LogError(PRM("FILE WRITE command: File not open: "), _CurrentPersistentFile->Name);
			return false;
		}

		if (count < 2)
			return _CurrentPersistentFile->Puts("");
		else
			return _CurrentPersistentFile->Puts(fields.Get(1));
	}

	Log::LogError(PRM("FILE command: Invalid option: "), action);

	return false;
}

#endif


bool CommandSet::command_help(const char* args)
{
	Log::LogInfo(PRM("Available commands:"));

	for (int i = 0; i < _CommandCount; i++)
	{
		CommandInfo* info = _Commands[i];
		Log::LogInfo("   ", info->Name);
	}

	return true;
}


#ifdef ARDJACK_NETWORK_AVAILABLE

bool CommandSet::command_net(const char* args)
{
	if ((NULL == args) || (strlen(args) == 0))
	{
		Log::LogWarning(PRM("NET command: No arguments"));
		return true;
	}

	return Globals::NetworkMgr->Interact(args);
}

#endif


bool CommandSet::command_nyi(const char* args)
{
	// NOT YET IMPLEMENTED.
	return false;
}


bool CommandSet::command_repeat(const char* args)
{
	if ((NULL == args) || (strlen(args) == 0))
	{
		Log::LogWarning(PRM("REPEAT command: No arguments"));
		return true;
	}

	char firstPart[ARDJACK_MAX_VERB_LENGTH];
	const char* remainder = NULL;

	Utils::GetArgs(args, firstPart, &remainder);

	int count = atoi(firstPart);

	char temp[20];

	for (int i = 1; i <= count; i++)
	{
		if (Globals::Verbosity > 3)
		{
			sprintf(temp, PRM("%d of %d"), i, count);
			Log::LogInfo(PRM("command_repeat: Cycle "), temp);
		}

		Globals::Interpreter->Execute(remainder);
	}

	return true;
}


bool CommandSet::command_reset(const char* args)
{
	Log::LogInfo(PRM("command_reset"));

	void(*resetFunc) (void) = 0;								// declare reset function at address 0
	resetFunc();

	return true;
}


#ifdef ARDJACK_INCLUDE_PERSISTENCE

bool CommandSet::command_restore(const char* args)
{
	// Reload the configuration file (if any).
	return Globals::LoadIniFile("configuration");
}


bool CommandSet::command_save(const char* args)
{
	// Save some basic items of the current configuration to file "configuration".
	return Globals::SaveIniFile("configuration");
}

#endif


bool CommandSet::command_send(const char* args)
{
	// Send text via the specified Connection.
	// Syntax:
	//		send conn "text"

	if ((NULL == args) || (strlen(args) == 0))
	{
		Log::LogWarning(PRM("SEND command: No arguments"));
		return true;
	}

	char fields[2][ARDJACK_MAX_VALUE_LENGTH];

	int count = Utils::SplitText2Array(args, ' ', fields, 2, ARDJACK_MAX_VALUE_LENGTH);

	if (count < 2)
	{
		Log::LogError(PRM("Insufficient fields for SEND command: '"), args, "'");
		return false;
	}

	Connection* conn = Globals::ConnectionMgr->LookupConnection(fields[0]);
	if (NULL == conn)
	{
		Log::LogError(PRM("No such Connection: '"), fields[0], "'");
		return false;
	}

	return conn->SendText(fields[1]);
}


bool CommandSet::command_set(const char* args)
{
	if ((NULL == args) || (strlen(args) == 0))
	{
		Log::LogWarning(PRM("SET command: No arguments"));
		return true;
	}

	char fields[4][ARDJACK_MAX_VALUE_LENGTH];
	bool handled;

	int count = Utils::SplitText2Array(args, ' ', fields, 4, ARDJACK_MAX_VALUE_LENGTH);

	if (count < 2)
	{
		Log::LogError(PRM("Insufficient fields for SET command: '"), args, "'");
		return false;
	}

	return Globals::Set(fields[0], fields[1], &handled);
}


#ifdef ARDJACK_INCLUDE_TESTS

bool CommandSet::command_test(const char* args)
	{
		if ((NULL == args) || (strlen(args) == 0))
		{
			Log::LogWarning(PRM("TEST command: No arguments"));
			return true;
		}

		// "test number arg1 arg2 verbosity"
		char fields[4][ARDJACK_MAX_VALUE_LENGTH];

		int count = Utils::SplitText2Array(args, ' ', fields, 4, ARDJACK_MAX_VALUE_LENGTH);

		int number = 1;
		int arg1 = 0;
		int arg2 = 0;
		int verbosity = 10;

		if (count >= 1)
		{
			number = atoi(fields[0]);

			if (count >= 2)
			{
				arg1 = atoi(fields[1]);

				if (count >= 3)
				{
					arg2 = atoi(fields[2]);

					if (count >= 4)
						verbosity = atoi(fields[3]);
				}
			}
		}

		RunTest(number, arg1, arg2, verbosity);

		return true;
	}


	bool CommandSet::command_tests(const char* args)
	{
		// TEMPORARY:
		RunTests(10);

		return true;
	}

#endif

