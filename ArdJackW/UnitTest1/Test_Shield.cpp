#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Device.h"
#include "Part.h"
#include "PartManager.h"
#include "Shield.h"
#include "ThinkerShield.h"




namespace UnitTest1
{
	TEST_CLASS(Test_Shield)
	{
	public:
		TEST_CLASS_INITIALIZE(InitForAllTests)
		{
			// Do something exactly once for all tests in this class, before the first test starts.

			WriteLog2("--- TEST_CLASS_INITIALIZE(InitForAllTests) ---");

			Log::InUnitTest = true;
			Log::LoggerCallback = &WriteLog2;

			Globals::InitialisedStatic = true;
			Globals::PartMgr = new PartManager();
			Globals::Verbosity = 7;
		}


		TEST_METHOD(Test_Shield_1)
		{
			// Arrange.
			Device* dev = new VellemanK8055Device("vell0");

			Shield* thinker = new ThinkerShield("thinker0");
			thinker->Owner = dev;

			// Act / Assert.
			Assert::AreEqual(17, (int)dev->PartCount);

			thinker->CreateInventory();
			Assert::AreEqual(17, (int)dev->PartCount);

		}


		static void WriteLog2(const char* text)
		{
			Logger::WriteMessage(text);
			Logger::WriteMessage("\n");
		}

	};
}

