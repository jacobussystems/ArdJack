/*
	CommandSet.h

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

#include "Globals.h"
#include "IoTMessage.h"


class CommandSet;

#ifdef ARDJACK_INCLUDE_PERSISTENCE
	class PersistentFile;
#endif

typedef bool (CommandSet::* CommandSetCallback)(const char* args);

struct CommandInfo
{
	CommandSetCallback Callback;
	char Name[ARDJACK_MAX_VERB_LENGTH];
};




class CommandSet
{
protected:
	uint8_t _CommandCount;
	CommandInfo* _Commands[ARDJACK_MAX_COMMANDS];

#ifdef ARDJACK_INCLUDE_PERSISTENCE
	PersistentFile* _CurrentPersistentFile;
#endif

	bool command_activate(const char* args);
	bool command_add(const char* args);
	bool command_beep(const char* args);
	bool command_configure(const char* args);
	bool command_connection(const char* args);
	bool command_deactivate(const char* args);
	bool command_define(const char* args);
	bool command_delay(const char* args);
	bool command_delete(const char* args);
	bool command_device(const char* args);
	bool command_display(const char* args);
	bool command_exit(const char* args);
	bool command_help(const char* args);
	bool command_nyi(const char* args);
	bool command_repeat(const char* args);
	bool command_reset(const char* args);
	bool command_send(const char* args);
	bool command_set(const char* args);

#ifdef ARDJACK_INCLUDE_PERSISTENCE
	bool command_file(const char* args);
	bool command_restore(const char* args);
	bool command_save(const char* args);
#endif

#ifdef ARDJACK_NETWORK_AVAILABLE
	bool command_net(const char* args);
#endif

	bool command_test(const char* args);

#ifdef ARDJACK_INCLUDE_TESTS
	bool command_tests(const char* args);
#endif

public:
	CommandSet();
	~CommandSet();

	CommandInfo* AddCommand(const char* name, CommandSetCallback callback);
	bool Handle(const char* line, const char* verb, const char* remainder);
};

