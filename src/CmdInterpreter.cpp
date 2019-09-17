/*
	CmdInterpreter.cpp

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

#include "CmdInterpreter.h"
#include "CommandSet.h"
#include "Displayer.h"
#include "Log.h"
#include "Globals.h"
#include "Part.h"
#include "Utils.h"



CmdInterpreter::CmdInterpreter()
{
	if (Globals::Verbosity > 7)
		Log::LogInfoF(PRM("CmdInterpreter ctor"));

	CommandSetCount = 0;
	MacroCount = 0;
	
	for (int i = 0; i < ARDJACK_MAX_COMMAND_SETS; i++)
		CommandSets[i] = NULL;

	for (int i = 0; i < ARDJACK_MAX_MACROS; i++)
		Macros[i] = NULL;
}


CmdInterpreter::~CmdInterpreter()
{
	for (int i = 0; i < ARDJACK_MAX_MACROS; i++)
	{
		if (NULL != Macros[i])
		{
			delete Macros[i];
			Macros[i] = NULL;
		}
	}
}


bool CmdInterpreter::AddCommandSet(CommandSet* commandSet)
{
	CommandSets[CommandSetCount++] = commandSet;
	return true;
}


MacroDef* CmdInterpreter::AddMacro(const char* name, const char* content)
{
	MacroDef* result = LookupMacro(name);

	if (NULL == result)
	{
		if (Globals::Verbosity > 2)
			Log::LogInfo(PRM("AddMacro: Adding Macro '"), name, "'");

		result = new MacroDef();
		strcpy(result->Name, name);
		strcpy(result->Content, content);
		Macros[MacroCount++] = result;
	}
	else
		strcpy(result->Content, content);

	return result;
}


bool CmdInterpreter::Execute(const char* multiLine)
{
	if (Utils::StringStartsWith(multiLine, Globals::CommentPrefix))
		return true;

	if (strlen(Globals::CommandSeparator) == 0)
		return ExecuteCommand(multiLine);

	if (Utils::StringStartsWith(multiLine, Globals::OneCommandPrefix))
	{
		const char* start = Utils::FindFirstNonWhitespace(multiLine + 1);
		return ExecuteCommand(start);
	}

	// TEMPORARY: only using first character of Command Separator.
	char sep = Globals::CommandSeparator[0];

	const char* ptr = strchr(multiLine, (int)sep);

	if (NULL == ptr)
	{
		// 'multiLine' is a single command.
		return ExecuteCommand(multiLine);
	}

	// Split 'multiLine' into separate commands and execute them.
	char temp[ARDJACK_MAX_COMMAND_LENGTH];

	strcpy(temp, multiLine);
	char command[ARDJACK_MAX_COMMAND_LENGTH];

	while (strlen(temp) > 0)
	{
		if (!Utils::GetFirstField(temp, sep, command, ARDJACK_MAX_COMMAND_LENGTH, true))
			break;

		ExecuteCommand(command);
	}

	return true;
}


bool CmdInterpreter::ExecuteCommand(const char* line)
{
	if (strlen(line) == 0)
		return true;

	if (Utils::StringStartsWith(line, Globals::CommentPrefix))
		return true;

	// Special case.
	if (Utils::StringEquals(line, "mem"))
		return Displayer::DisplayItem("memory");

	if (Globals::Verbosity > 2)
		Log::LogInfo(PRM("Command: '"), line, "'");

	// Split the line into 2 fields, i.e. the 'verb' and the remainder.
	char verb[40];
	const char* remainder = NULL;

	Utils::GetArgs(line, verb, &remainder);

	if (strlen(verb) == 0)
		return true;

	if (CommandSetCount == 0)
	{
		Log::LogWarning(PRM("Interpreter has no Command Set: "), line);
		return false;
	}

	// Do we recognise this command?
	for (int i = 0; i < CommandSetCount; i++)
	{
		if (CommandSets[i]->Handle(line, verb, remainder))
			return true;
	}

	if (Globals::Verbosity > 7)
		Log::LogInfo(PRM("Not handled by CommandSet(s): '"), verb, "'");

	// Is it a Macro?
	MacroDef* pMacro = LookupMacro(verb);

	if (NULL != pMacro)
		return ExecuteMacro(pMacro->Name, pMacro->Content, remainder);

	// Is it an Object name, or 'Device.Part' syntax?
	bool saveLogIncludeMemory = Log::IncludeMemory;
	bool saveLogIncludeTime = Log::IncludeTime;
	Log::IncludeMemory = false;
	Log::IncludeTime = false;

	bool status = Displayer::DisplayObject2(verb, true);

	Log::IncludeMemory = saveLogIncludeMemory;
	Log::IncludeTime = saveLogIncludeTime;

	if (status)
		return true;

	// Give up.
	Log::LogError(PRM("Invalid command: '"), line, "'");

	return false;
}


bool CmdInterpreter::ExecuteMacro(const char* name, const char* content, const char* remainder)
{
	if (Globals::Verbosity > 2)
		Log::LogInfo(PRM("ExecuteMacro: '"), name, "'");

	return Execute(content);
}


MacroDef* CmdInterpreter::LookupMacro(const char* name)
{
	MacroDef* pMacro;

	for (int i = 0; i < MacroCount; i++)
	{
		pMacro = Macros[i];

		if (Utils::StringEquals(name, pMacro->Name))
			return pMacro;
	}

	if (Globals::Verbosity > 7)
		Log::LogInfo(PRM("CmdInterpreter::LookupMacro: Not a Macro: '"), name, "'");

	return NULL;
}


bool CmdInterpreter::RemoveMacro(const char* name)
{
	MacroDef* pMacro = LookupMacro(name);

	if (NULL == pMacro)
	{
		Log::LogWarning("RemoveMacro: No such Macro: ", name);
		return false;
	}

	// TODO:

	return true;
}

