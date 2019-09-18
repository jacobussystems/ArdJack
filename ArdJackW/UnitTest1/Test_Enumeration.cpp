#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Enumeration.h"
#include "Globals.h"
#include "Utils.h"



namespace UnitTest1
{
	TEST_CLASS(Test_Enumeration)
	{
	public:
		TEST_METHOD(Test_Enumeration_Add)
		{
			// Arrange.
			Enumeration* en1 = new Enumeration("en1");
			char work[40];
			int count = 50;

			// Act.

			// Add the first 3 separately to get different 'const char*' locations in memory.
			en1->Add("Enum name 0", 20);
			en1->Add("Enum name 1", 21);
			en1->Add("Enum name 2", 22);

			// Add the rest using a new string on the heap for each one.
			for (int i = 3; i < count; i++)
			{
				char* temp = new char[20];
				sprintf(temp, "Enum name %d", i);
				en1->Add(temp, i + 20);
			}

			// Assert.
			Assert::AreEqual(count, en1->Count());

			for (int i = 0; i < count; i++)
			{
				sprintf(work, "Enum name %d", i);
				Assert::IsTrue(Utils::StringEquals(en1->Names.Get(i), work));

				int val = en1->Values.Get(i);
				Assert::AreEqual(i + 20, val);
			}
		}


		TEST_METHOD(Test_Enumeration_LookupName)
		{
			// Arrange.
			Enumeration* en1 = new Enumeration("en1");
			int value;
			char work[40];
			int count = 50;

			// Act.
			for (int i = 0; i < count; i++)
			{
				char* temp = new char[20];
				sprintf(temp, "Enum name %d", i);
				en1->Add(temp, i + 20);
			}

			// Assert.
			Assert::AreEqual(count, (int)en1->Count());

			for (int i = 0; i < count; i++)
			{
				sprintf(work, "Enum name %d", i);
				value = en1->LookupName(work, -99);
				Assert::AreEqual(i + 20, value);

			}
		}


		TEST_METHOD(Test_Enumeration_LookupValue)
		{
			// Arrange.
			Enumeration* en1 = new Enumeration("en1");
			char work[40];
			int count = 50;

			// Act.
			for (int i = 0; i < count; i++)
			{
				char* temp = new char[20];
				sprintf(temp, "Enum name %d", i);
				en1->Add(temp, i + 20);
			}

			// Assert.
			Assert::AreEqual(count, (int)en1->Count());

			for (int i = 0; i < count; i++)
			{
				const char* value = en1->LookupValue(i + 20, "?");
				sprintf(work, "Enum name %d", i);
				Assert::IsTrue(Utils::StringEquals(value, work));
			}
		}

	};
}

