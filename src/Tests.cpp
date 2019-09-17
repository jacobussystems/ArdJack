/*
	Tests.cpp

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
	#include "ClipboardConnection.h"
	//#include "PipeConnection.h"
	#include "WinDevice.h"
#endif

#include "CmdInterpreter.h"
#include "Connection.h"
#include "Device.h"
#include "Displayer.h"
#include "Globals.h"
#include "Route.h"
#include "Log.h"
#include "SerialConnection.h"
#include "Tests.h"
#include "UdpConnection.h"
#include "Utils.h"


#ifdef ARDJACK_INCLUDE_TESTS

//void Route_Callback_Command(Connection *connection, Route* route, IoTMessage* msg);
//void Route_Callback_Device(Connection *connection, Route* route, IoTMessage* msg);

void Test1(int arg1, int arg2);
void Test2(int arg1, int arg2);



//void Route_Callback_Command(Connection *connection, Route* route, IoTMessage* msg)
//{
//	if (Globals::Verbosity > 1)
//	{
//		char buffer[100];
//		sprintf(buffer, "Route_Callback_Command: connection '%s', route '%s', ", connection->Name, route->Name);
//		Log::LogInfo(buffer);
//		msg->LogIt();
//	}
//
//	CmdInterpreter interpreter;
//
//	CommandSet commandSet;
//	interpreter.AddCommandSet(&commandSet);
//
//	interpreter.Execute(msg->Text);
//}
//
//
//void Route_Callback_Device(Connection *connection, Route* route, IoTMessage* msg)
//{
//	if (Globals::Verbosity > 1)
//	{
//		char buffer[100];
//		sprintf(buffer, "Route_Callback_Device: connection '%s', route '%s', ", connection->Name, route->Name);
//		Log::LogInfo(buffer);
//		msg->LogIt();
//	}
//
//	Device *dev = Globals::DefaultDevice;
//
//	Globals::HandleDeviceRequest(dev, msg->Text, msg);
//}


void RunTest(int number, int arg1, int arg2, int verbosity)
{
	Log::LogInfo(PRM("RunTest"));

	int saveVerbosity = Globals::Verbosity;
	Globals::Verbosity = verbosity;
	Displayer::DisplayMemory();

	switch (number)
	{
	case 1:
		Test1(arg1, arg2);
		break;

	case 2:
		Test2(arg1, arg2);
		break;
	}

	Log::LogInfo(PRM("RunTest done"));
	Displayer::DisplayMemory();
	Globals::Verbosity = saveVerbosity;
}


void RunTests(int verbosity)
{
	Log::LogInfo(PRM("RunTests"));

	int saveVerbosity = Globals::Verbosity;
	Globals::Verbosity = verbosity;
	Displayer::DisplayMemory();

	//if (Test_FifoBuffer1())
	//	Log::LogInfo(PRM("Test_FifoBuffer1: PASS"));
	//else
	//	Log::LogError(PRM("Test_FifoBuffer1: FAIL"));

	//if (Test_FifoBuffer2())
	//	Log::LogInfo(PRM("Test_FifoBuffer2: PASS"));
	//else
	//	Log::LogError(PRM("Test_FifoBuffer2: FAIL"));

	//Test1();
	//Test2();
	//Test3();
	//Test4();
	//Test5();
	//Test6();
	//Test7();
	//Test8();

	Log::LogInfo(PRM("RunTests done"));
	Displayer::DisplayMemory();
	Globals::Verbosity = saveVerbosity;
}


//bool Test_FifoBuffer1()
//{
//	Log::LogInfo("Test_FifoBuffer1");
//
//	// Arrange.
//	FifoBuffer buff = FifoBuffer(sizeof(IoTMessage), 6);
//	FifoBuffer *buffer = &buff;
//	IoTMessage msg1;
//	IoTMessage msg2;
//	IoTMessage out;
//
//	strcpy(msg1.WireText, "msg1");
//	strcpy(msg2.WireText, "msg2");
//
//	// Act / Assert.
//	if (buffer->IsFull()) return false;
//	if (!buffer->IsEmpty()) return false;
//
//	if (!buffer->Push(&msg1)) return false;
//	if (!buffer->Push(&msg2)) return false;
//
//	if (buffer->IsFull()) return false;
//	if (buffer->IsEmpty()) return false;
//
//	if (!buffer->Pop(&out)) return false;
//	if (strcmp(out.WireText, "msg1") != 0) return false;
//
//	if (!buffer->Pop(&out)) return false;
//	if (strcmp(out.WireText, "msg2") != 0) return false;
//
//	if (buffer->IsFull()) return false;
//	if (!buffer->IsEmpty()) return false;
//
//	return true;
//}
//
//
//bool Test_FifoBuffer2()
//{
//	Log::LogInfo("Test_FifoBuffer2");
//
//	// Arrange.
//	FifoBuffer buff = FifoBuffer(200, 6);
//	FifoBuffer *buffer = &buff;
//	char out[200];
//
//	// Act / Assert.
//	if (buffer->IsFull()) return false;
//	if (!buffer->IsEmpty()) return false;
//
//	if (!buffer->Push("Line 1.")) return false;
//	if (!buffer->Push("Line 2.")) return false;
//
//	if (buffer->IsFull()) return false;
//	if (buffer->IsEmpty()) return false;
//
//	if (!buffer->Pop(out)) return false;
//	if (strcmp(out, "Line 1.") != 0) return false;
//
//	if (!buffer->Pop(out)) return false;
//	if (strcmp(out, "Line 2.") != 0) return false;
//
//	if (buffer->IsFull()) return false;
//	if (!buffer->IsEmpty()) return false;
//
//	return true;
//}


void Test1(int arg1, int arg2)
{
	int size = arg1;
	int count = arg2;

	char temp[102];
	sprintf(temp, PRM("Test1: size %d, count %d"), size, count);
	Log::LogInfo(temp);

	void* ptr[50];

	for (int i = 0; i < count; i++)
	{
		ptr[i] = Utils::MemMalloc(size);
		sprintf(temp, PRM("[%d] %d bytes @ %p"), i, size, ptr[i]);
		Log::LogInfo(temp);
	}

	Displayer::DisplayMemory();

	for (int i = 0; i < count; i++)
	{
		if (NULL != ptr[i])
			Utils::MemFree(ptr[i]);
	}

	Log::LogInfo(PRM("Test1: Exit"));
}


void Test2(int arg1, int arg2)
{
	int size = arg1;
	int count = arg2;

	char temp[202];
	sprintf(temp, PRM("Test2: size %d, count %d"), size, count);
	Log::LogInfo(temp);

	StringList sl1;
	char work[200];

	for (int i = 0; i < count; i++)
	{
		Utils::RepeatChar(work, '=', size);
		sprintf(temp, PRM("[%d] '%s'"), i, work);
		Log::LogInfo(temp);
		sl1.Add(temp);
	}

	Displayer::DisplayMemory();

	for (int i = 0; i < count; i++)
	{
		sprintf(temp, PRM("[%d] '%s'"), i, sl1.Get(i));
		Log::LogInfo(temp);
	}
}







//void Test1()
//{
//	Log::LogInfo(PRM("Test1"));
//
//	UdpConnection udp0 = UdpConnection("test_udp0");
//
//	udp0.AddConfig();
//	udp0.SetActive(true);
//
//	if (!udp0.Active()) return;
//
//	//CmdInterpreter interpreter;
//	//CommandSet commandSet;
//	//interpreter.AddCommandSet(commandSet);
//
//	//IoTMessage msg;
//
//	// Wait for commands.
//	//while (true)
//	//{
//	//	udp0.Poll();
//	//	//dev0.poll();
//
//	//	if (!Globals::CommandBuffer->IsEmpty())
//	//	{
//	//		msg.Text[0] = NULL;
//	//		Globals::CommandBuffer->Pop(&msg);
//
//	//		if (strlen(msg.Text) > 0)
//	//			interpreter.Execute(msg.Text);
//	//	}
//
//	//	if (!Globals::RequestBuffer.IsEmpty())
//	//	{
//	//		msg.Text[0] = NULL;
//	//		Globals::RequestBuffer.Pop(&msg);
//
//	//		if (strlen(msg.Text) > 0)
//	//			interpreter.CommandSets[0].ExecuteRequest(msg.Text, msg);
//	//	}
//
//		//Utils::Sleep_ms(1000);				// 1s
//	//	printf("-"));
//	//}
//
//	//Log::LogInfo(PRM("SKIPPED"));
//
//	Log::Flush();
//}


//void Test2()
//{
//	Log::LogInfo(PRM("Test2"));
//
//#ifdef ARDUINO
//	SerialConnection ser0 = SerialConnection("ser0");
//	ser0.SetActive(true);
//
//	ser0.OutputText("from ser0");
//#else
//	Log::LogInfo(PRM("SKIPPED"));
//#endif
//
//	Log::Flush();
//}
//
//
//void Test3()
//{
//	Log::LogInfo(PRM("Test3"));
//
//	//char values[1][MAX_FIELD_SIZE];
//	//strcpy(values[0], "1");
//
//	//(Globals::DefaultDevice)->Open();
//
//	//(Globals::DefaultDevice)->Read(PART_TYPE_LED, 0, values);
//	//Utils::DelayMs(500);
//	//(Globals::DefaultDevice)->Write(PART_TYPE_LED, 0, values);
//
//	Log::LogInfo(PRM("SKIPPED"));
//
//	Log::Flush();
//}
//
//
//void Test4()
//{
//	Log::LogInfo(PRM("Test4"));
//
//#ifdef ARDUINO
//	#ifdef WIFILINK
//		Test11A();
//	#else
//		Log::LogInfo(PRM("SKIPPED"));
//	#endif
//#else
//	Test4A();
//#endif
//}
//
//
//void Test4A()
//{
//	//UdpConnection udp0 = UdpConnection("udp0");
//
//	//Route route = Route("Test10");
//	//route.Callback = Route_Callback;
//	//route.Filter.FromFilter.Operation = STRING_COMPARE_EQUALS;
//	//strcpy(route.Filter.FromFilter.Text, "DELL-9020");
//	//udp0.AddRoute(&route);
//
//	////udp0.Send("Hello World!");
//
//	//// Poll for 10s.
//	//Log::LogInfo(PRM("Polling UDP input for 10 seconds:"));
//
//	//for (int i = 0; i < 100; i++)
//	//{
//	//	udp0.PollInputs();
//	//	Utils::Sleep_ms(100);
//	//}
//
//	Log::LogInfo(PRM("SKIPPED"));
//
//	Log::Flush();
//}
//
//
//void Test5()
//{
//	Log::LogInfo(PRM("Test5"));
//
///*
//#ifdef ARDUINO
//#else
//	//Create the server pipe
//	CNamedPipe serverPipe;
//
//	if (!serverPipe.Create("\\\\.\\PIPE\\IoTWAC1", PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_WAIT, 1, 4096, 4096, 1, nullptr))
//	{
//		Log::LogError("Failed in call to CNamedPipe::Create");
//		return;
//	}
//
//	//Connect the client  
//	Log::LogInfo("Waiting for a client to connect...");
//	if (!serverPipe.Connect())
//	{
//		Log::LogError("Failed in call to CNamedPipe::Connect");
//		return;
//	}
//
//	// Get the current local time
//	SYSTEMTIME st;
//	memset(&st, 0, sizeof(st));
//	GetLocalTime(&st);
//
//	char sTime[256];
//	sTime[0] = '\0';
//	GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, nullptr, sTime, 256);
//
//	char msg[256];
//	sprintf(msg, "Time: %s", sTime);
//
//	// Write to the pipe.
//	DWORD nBytes = strlen(msg) + 1;
//	DWORD nBytesWritten = 0;
//
//	if (!serverPipe.Write(msg, nBytes, &nBytesWritten))
//	{
//		Log::LogError("Failed in call to CNamedPipe::Write");
//		return;
//	}
//
//	// Print what we just did.
//	char temp[200];
//	sprintf(temp, PRM("  Sent: '%s' (%d chars, %d bytes)"), msg, strlen(msg), nBytes);
//	Log::LogInfo(temp);
//
//	//Flush the pipe before we disconnect the client
//	serverPipe.Flush();
//
//	//Disconnect the client
//	Log::LogInfo("  Disconnecting");
//	serverPipe.Disconnect();
//#endif
//*/
//
//	Log::Flush();
//}
//
//
//void Test6()
//{
//	Log::LogInfo(PRM("Test6"));
//
///*
//#ifdef ARDUINO
//#else
//	PipeConnection pipe0 = PipeConnection("pipe0");
//
//	Route route1 = Route("Test13_Command", NULL);
//	route1.Callback = Route_Callback_Command;
//	route1.Filter.ReturnFilter.Operation = STRING_COMPARE_STARTS_WITH;
//	strcpy(route1.Filter.ReturnFilter.Text, Globals::CommandPrefix);
//	pipe0.AddRoute(&route1);
//
//	Route route2 = Route("Test13_Device", NULL);
//	route2.Callback = Route_Callback_Device;
//	route2.Filter.ReturnFilter.Operation = STRING_COMPARE_STARTS_WITH;
//	strcpy(route2.Filter.ReturnFilter.Text, Globals::DevicePrefix);
//	pipe0.AddRoute(&route2);
//
//	for (int i = 0; i < 4; i++)
//	{
//		pipe0.PollInputs();
//		Log::WriteNewLine();
//		Utils::DelayMs(500);
//	}
//#endif
//*/
//
//	Log::Flush();
//}
//
//
//void Test7()
//{
//	Log::LogInfo(PRM("Test7"));
//
////#ifdef ARDUINO
////#else
////	Log::LogInfo("Monitoring Clipboard for 10s:");
////
////	Route route1 = Route("Test14_Command", NULL);
////	route1.Callback = Route_Callback_Command;
////	route1.Filter.ReturnFilter.Operation = STRING_COMPARE_STARTS_WITH;
////	strcpy(route1.Filter.ReturnFilter.Text, Globals::CommandPrefix);
////	clip0.AddRoute(&route1);
////
////	clip0.SetActive(true);
////
////	for (int i = 0; i < 100; i++)
////	{
////		clip0.PollInputs();
////		Utils::DelayMs(100);
////	}
////#endif
//
//	Log::LogInfo("Done");
//	Log::Flush();
//}
//
//
//void Test8()
//{
//	Log::LogInfo(PRM("Test8"));
//
//#ifdef ARDUINO
//#else
//#endif
//
//	Log::LogInfo("Done");
//	Log::Flush();
//}

#endif

