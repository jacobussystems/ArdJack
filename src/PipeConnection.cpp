#include "pch.h"

#ifdef ARDUINO
#else

#include "stdafx.h"

#include <tchar.h>

#include "npipe.h"
#include "Log.h"
#include "PipeConnection.h"
#include "Utils.h"



PipeConnection::PipeConnection(const char *name) : Connection(name)
{
	strcpy(InPipeName, "\\\\.\\PIPE\\IoTW1");
	strcpy(OutPipeName, "\\\\.\\PIPE\\IoTW2");
}


PipeConnection::~PipeConnection()
{
	// Create the output pipe.
	if (!OutPipe.Create(_T(OutPipeName), PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_NOWAIT, 1, 4096, 4096, 1, nullptr))
	{
		Log::WriteError("Creating output PipeConnection");
		return;
	}


}


bool PipeConnection::OutputMessage(IoTMessage *msg)
{
	// Write to the pipe.
	wchar_t msgW[256];

	//DWORD nBytes = wcslen(msgW) * 2 + 2;
	//DWORD nBytesWritten = 0;

	//if (!OutPipe.Write(msgW, nBytes, &nBytesWritten))
	//{
	//	_tprintf(_T("ERROR - Failed to write to named pipe '%s'\n"), OutPipeName);
	//	return 1;
	//}

	return true;
}


bool PipeConnection::PollInput(int maxCount)
{
	if (!WaitNamedPipe(InPipeName, 1000))
	{
		Log::WriteError("Pipe not available: ", InPipeName);
		return false;
	}

	if (!InPipe.Open(InPipeName, GENERIC_READ | GENERIC_WRITE | FILE_SHARE_READ, 0, nullptr, 0))
	{
		Log::WriteError("Failed to connect to pipe: ", InPipeName);
		return false;
	}

	char buffer[260];
	DWORD bytesRead = 0;

	if (!InPipe.Read(buffer, 256, &bytesRead))
	{
		Log::WriteError("Failed to read from pipe: ", InPipeName);
		return false;
	}

	InPipe.Close();

	if (Globals::Verbosity > 2)
	{
		char temp[400];
		sprintf(temp, "%s RX: Read '%s'", Name, buffer);
		Log::WriteLine(temp);
	}

	ProcessInput(buffer);

	return true;
}


bool PipeConnection::PollOutput(int maxCount)
{

	return true;
}


bool PipeConnection::SendQueuedOutput(IoTMessage *msg)
{
	return false;
}

#endif

