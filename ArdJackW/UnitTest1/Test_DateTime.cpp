#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "DateTime.h"
#include "Utils.h"



namespace UnitTest1
{
	TEST_CLASS(Test_DateTime)
	{
	public:
		TEST_METHOD(Test_Ctor)
		{
			// Arrange.
			DateTime dt1;
			DateTime dt2;

			// Act.

			// Assert.
			Assert::AreEqual(1, (int)dt1.Day);
			Assert::AreEqual(0, (int)dt1.Month);
			Assert::AreEqual(0, (int)dt1.Year);

			Assert::AreEqual(1, (int)dt2.Day);
			Assert::AreEqual(0, (int)dt2.Month);
			Assert::AreEqual(0, (int)dt2.Year);
		}


		TEST_METHOD(Test_Copy)
		{
			// Arrange.
			DateTime dt1;
			DateTime dt2;

			// Act.
			dt2.Day = 10;
			dt2.Month = 3;
			dt1.Copy(&dt2);

			// Assert.
			Assert::AreEqual(10, (int)dt2.Day);
			Assert::AreEqual(3, (int)dt2.Month);
			Assert::AreEqual(0, (int)dt2.Year);

			Assert::AreEqual(10, (int)dt1.Day);
			Assert::AreEqual(3, (int)dt1.Month);
			Assert::AreEqual(0, (int)dt1.Year);
		}

	};
}

