#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Globals.h"
#include "StringList.h"
#include "Utils.h"



namespace UnitTest1
{
	TEST_CLASS(Test_Utils_SplitText)
	{
	public:
		TEST_METHOD(Test_SplitText1)
		{
			// Arrange.
			StringList* sl1 = new StringList(IOTJ_MAX_VALUES);
			char string1[80];

			// Act.
			strcpy(string1, "1 22 333");
			int count = Utils::SplitText(string1, ' ', sl1, IOTJ_MAX_VALUES, IOTJ_MAX_VALUE_LENGTH, true);

			// Assert.
			Assert::AreEqual(3, count);
			Assert::AreEqual(3, sl1->Count);
			Assert::AreEqual(0, _stricmp(sl1->Get(0), "1"));
			Assert::AreEqual(0, _stricmp(sl1->Get(1), "22"));
			Assert::AreEqual(0, _stricmp(sl1->Get(2), "333"));
		}


		TEST_METHOD(Test_SplitText2)
		{
			// Arrange.
			StringList* sl1 = new StringList(IOTJ_MAX_VALUES);
			char string1[80];

			// Act.
			strcpy(string1, "  1 \" 2 2 \" 333  ");
			int count = Utils::SplitText(string1, ' ', sl1, IOTJ_MAX_VALUES, IOTJ_MAX_VALUE_LENGTH, false);

			// Assert.
			Assert::AreEqual(3, count);
			Assert::AreEqual(0, _stricmp(sl1->Get(0), "1"));
			Assert::AreEqual(0, _stricmp(sl1->Get(1), " 2 2 "));
			Assert::AreEqual(0, _stricmp(sl1->Get(2), "333"));
		}


		TEST_METHOD(Test_SplitText3)
		{
			// Arrange.
			StringList* sl1 = new StringList(IOTJ_MAX_VALUES);
			char string1[80];

			// Act.
			strcpy(string1, "  1 \" 2 2 \" 333  ");
			int count = Utils::SplitText(string1, ' ', sl1, IOTJ_MAX_VALUES, IOTJ_MAX_VALUE_LENGTH, true);

			// Assert.
			Assert::AreEqual(3, count);
			Assert::AreEqual(0, _stricmp(sl1->Get(0), "1"));
			Assert::AreEqual(0, _stricmp(sl1->Get(1), "2 2"));
			Assert::AreEqual(0, _stricmp(sl1->Get(2), "333"));
		}


		TEST_METHOD(Test_SplitText4)
		{
			// Arrange.
			StringList* sl1 = new StringList(IOTJ_MAX_VALUES);
			char string1[80];

			// Act.
			strcpy(string1, "  1 ' 2 2 ' 333  ");
			int count = Utils::SplitText(string1, ' ', sl1, IOTJ_MAX_VALUES, IOTJ_MAX_VALUE_LENGTH, false);

			// Assert.
			Assert::AreEqual(3, count);
			Assert::AreEqual(0, _stricmp(sl1->Get(0), "1"));
			Assert::AreEqual(0, _stricmp(sl1->Get(1), " 2 2 "));
			Assert::AreEqual(0, _stricmp(sl1->Get(2), "333"));
		}


		TEST_METHOD(Test_SplitText5)
		{
			// Arrange.
			StringList* sl1 = new StringList(IOTJ_MAX_VALUES);
			char string1[80];

			// Act.
			strcpy(string1, "  1 ' 2 2 ' 333  ");
			int count = Utils::SplitText(string1, ' ', sl1, IOTJ_MAX_VALUES, IOTJ_MAX_VALUE_LENGTH, true);

			// Assert.
			Assert::AreEqual(3, count);
			Assert::AreEqual(0, _stricmp(sl1->Get(0), "1"));
			Assert::AreEqual(0, _stricmp(sl1->Get(1), "2 2"));
			Assert::AreEqual(0, _stricmp(sl1->Get(2), "333"));
		}


		TEST_METHOD(Test_SplitText6)
		{
			// Arrange.
			StringList* sl1 = new StringList(IOTJ_MAX_VALUES);
			char string1[80];

			// Act.
			strcpy(string1, "  1 = ' 2 = 2 ' = 333  ");
			int count = Utils::SplitText(string1, '=', sl1, IOTJ_MAX_VALUES, IOTJ_MAX_VALUE_LENGTH, true);

			// Assert.
			Assert::AreEqual(3, count);
			Assert::AreEqual(0, _stricmp(sl1->Get(0), "1"));
			Assert::AreEqual(0, _stricmp(sl1->Get(1), "2 = 2"));
			Assert::AreEqual(0, _stricmp(sl1->Get(2), "333"));
		}


		TEST_METHOD(Test_SplitText7)
		{
			// WARNING - NOT REALLY OK!

			// Arrange.
			StringList* sl1 = new StringList(IOTJ_MAX_VALUES);
			char string1[80];

			// Act.
			strcpy(string1, "  1 = ' 2 = 2 ' = 333  ");
			int count = Utils::SplitText(string1, '=', sl1, IOTJ_MAX_VALUES, IOTJ_MAX_VALUE_LENGTH, false);

			// Assert.
			Assert::AreEqual(3, count);
			Assert::AreEqual(0, _stricmp(sl1->Get(0), "1 "));
			Assert::AreEqual(0, _stricmp(sl1->Get(1), " 2 = 2 "));
			Assert::AreEqual(0, _stricmp(sl1->Get(2), " 333"));
		}

	};
}
