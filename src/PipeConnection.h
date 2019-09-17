#pragma once


#ifdef ARDUINO
#else

#include "stdafx.h"
#include "npipe.h"

#include "Connection.h"
#include "Globals.h"


class PipeConnection :
	public Connection
{
private:
	CNamedPipe InPipe;
	CNamedPipe OutPipe;

public:
	char InPipeName[MAX_NAME_SIZE];
	char OutPipeName[MAX_NAME_SIZE];

	PipeConnection(const char *name);
	~PipeConnection();

	bool OutputMessage(IoTMessage *msg);
	bool PollInput(int maxCount=5);
	bool PollOutput(int maxCount=5);
	bool SendQueuedOutput(IoTMessage *msg);
};

#endif

