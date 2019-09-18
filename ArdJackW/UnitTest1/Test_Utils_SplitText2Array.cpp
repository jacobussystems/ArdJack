#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Globals.h"
#include "Utils.h"



namespace UnitTest1
{
	TEST_CLASS(Test_Utils_SplitText2Array)
	{
	public:
		TEST_METHOD(Test_SplitText1)
		{
			// Arrange.
			char fields[ARDJACK_MAX_VALUES][ARDJACK_MAX_VALUE_LENGTH];
			char string1[80];

			// Act.
			strcpy(string1, "1 22 333");
			int count = Utils::SplitText2Array(string1, ' ', fields, ARDJACK_MAX_VALUES, ARDJACK_MAX_VALUE_LENGTH, true);

			// Assert.
			Assert::AreEqual(3, count);
			Assert::AreEqual(0, _stricmp(fields[0], "1"));
			Assert::AreEqual(0, _stricmp(fields[1], "22"));
			Assert::AreEqual(0, _stricmp(fields[2], "333"));
		}


		TEST_METHOD(Test_SplitText2)
		{
			// Arrange.
			char fields[ARDJACK_MAX_VALUES][ARDJACK_MAX_VALUE_LENGTH];
			char string1[80];

			// Act.
			strcpy(string1, "  1 \" 2 2 \" 333  ");
			int count = Utils::SplitText2Array(string1, ' ', fields, ARDJACK_MAX_VALUES, ARDJACK_MAX_VALUE_LENGTH, false);

			// Assert.
			Assert::AreEqual(3, count);
			Assert::AreEqual(0, _stricmp(fields[0], "1"));
			Assert::AreEqual(0, _stricmp(fields[1], " 2 2 "));
			Assert::AreEqual(0, _stricmp(fields[2], "333"));
		}


		TEST_METHOD(Test_SplitText3)
		{
			// Arrange.
			char fields[ARDJACK_MAX_VALUES][ARDJACK_MAX_VALUE_LENGTH];
			char string1[80];

			// Act.
			strcpy(string1, "  1 \" 2 2 \" 333  ");
			int count = Utils::SplitText2Array(string1, ' ', fields, ARDJACK_MAX_VALUES, ARDJACK_MAX_VALUE_LENGTH, true);

			// Assert.
			Assert::AreEqual(3, count);
			Assert::AreEqual(0, _stricmp(fields[0], "1"));
			Assert::AreEqual(0, _stricmp(fields[1], "2 2"));
			Assert::AreEqual(0, _stricmp(fields[2], "333"));
		}


		TEST_METHOD(Test_SplitText4)
		{
			// Arrange.
			char fields[ARDJACK_MAX_VALUES][ARDJACK_MAX_VALUE_LENGTH];
			char string1[80];

			// Act.
			strcpy(string1, "  1 ' 2 2 ' 333  ");
			int count = Utils::SplitText2Array(string1, ' ', fields, ARDJACK_MAX_VALUES, ARDJACK_MAX_VALUE_LENGTH, false);

			// Assert.
			Assert::AreEqual(3, count);
			Assert::AreEqual(0, _stricmp(fields[0], "1"));
			Assert::AreEqual(0, _stricmp(fields[1], " 2 2 "));
			Assert::AreEqual(0, _stricmp(fields[2], "333"));
		}


		TEST_METHOD(Test_SplitText5)
		{
			// Arrange.
			char fields[ARDJACK_MAX_VALUES][ARDJACK_MAX_VALUE_LENGTH];
			char string1[80];

			// Act.
			strcpy(string1, "  1 ' 2 2 ' 333  ");
			int count = Utils::SplitText2Array(string1, ' ', fields, ARDJACK_MAX_VALUES, ARDJACK_MAX_VALUE_LENGTH, true);

			// Assert.
			Assert::AreEqual(3, count);
			Assert::AreEqual(0, _stricmp(fields[0], "1"));
			Assert::AreEqual(0, _stricmp(fields[1], "2 2"));
			Assert::AreEqual(0, _stricmp(fields[2], "333"));
		}


		TEST_METHOD(Test_SplitText6)
		{
			// Arrange.
			char fields[ARDJACK_MAX_VALUES][ARDJACK_MAX_VALUE_LENGTH];
			char string1[80];

			// Act.
			strcpy(string1, "  1 = ' 2 = 2 ' = 333  ");
			int count = Utils::SplitText2Array(string1, '=', fields, ARDJACK_MAX_VALUES, ARDJACK_MAX_VALUE_LENGTH, true);

			// Assert.
			Assert::AreEqual(3, count);
			Assert::AreEqual(0, _stricmp(fields[0], "1"));
			Assert::AreEqual(0, _stricmp(fields[1], "2 = 2"));
			Assert::AreEqual(0, _stricmp(fields[2], "333"));
		}


		TEST_METHOD(Test_SplitText7)
		{
			// WARNING - NOT REALLY OK!

			// Arrange.
			char fields[ARDJACK_MAX_VALUES][ARDJACK_MAX_VALUE_LENGTH];
			char string1[80];

			// Act.
			strcpy(string1, "  1 = ' 2 = 2 ' = 333  ");
			int count = Utils::SplitText2Array(string1, '=', fields, ARDJACK_MAX_VALUES, ARDJACK_MAX_VALUE_LENGTH, false);

			// Assert.
			Assert::AreEqual(3, count);
			Assert::AreEqual(0, _stricmp(fields[0], "1 "));
			Assert::AreEqual(0, _stricmp(fields[1], " 2 = 2 "));
			Assert::AreEqual(0, _stricmp(fields[2], " 333"));
		}

	};
}
