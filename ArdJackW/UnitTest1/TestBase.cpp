#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Globals.h"
#include "Log.h"
#include "Utils.h"



namespace UnitTest1
{
	TEST_CLASS(TestBase)
	{
	public:
		TEST_METHOD(TestBase_1)
		{
			Log::InUnitTest = true;
		}


		static void WriteLog2(const char* text)
		{
			Logger::WriteMessage(text);
			Logger::WriteMessage("\n");
		}

	};
}

