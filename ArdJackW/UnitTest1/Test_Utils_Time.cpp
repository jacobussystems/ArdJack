#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "DateTime.h"
#include "Utils.h"



namespace UnitTest1
{
	TEST_CLASS(Test_Utils_Time)
	{
	public:
		TEST_METHOD(Test_SecondsToTime)
		{
			// Arrange.
			DateTime dt1;
			DateTime dt2;

			// Act.
			Utils::SecondsToTime(0, &dt1);
			Utils::SecondsToTime(1542914280, &dt2);

			// Assert.
			Assert::AreEqual(1, (int)dt1.Day);
			Assert::AreEqual(1, (int)dt1.Month);
			Assert::AreEqual(1970, (int)dt1.Year);

			Assert::AreEqual(22, (int)dt2.Day);
			Assert::AreEqual(11, (int)dt2.Month);
			Assert::AreEqual(2018, (int)dt2.Year);
		}


	};
}

