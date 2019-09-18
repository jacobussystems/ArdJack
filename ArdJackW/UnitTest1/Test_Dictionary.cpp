#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Dictionary.h"
#include "Globals.h"
#include "Utils.h"



namespace UnitTest1
{
	TEST_CLASS(Test_Dictionary)
	{
	public:
		TEST_METHOD(Test_Dictionary_Add)
		{
			// Arrange.
			Dictionary dict1 = new Dictionary("dict1");
			char key[20];
			char value[40];
			char work[40];
			int count = 100;

			// Act.
			for (int i = 0; i < count; i++)
			{
				sprintf(key, "key%d", i);
				sprintf(value, "Dict item %d", i);
				dict1.Add(key, value);
			}

			// Assert.
			Assert::AreEqual(count, dict1.Count());

			// Lookup each key.
			const char* pValue;

			for (int i = 0; i < count; i++)
			{
				sprintf(key, "key%d", i);
				Assert::IsTrue(dict1.ContainsKey(key));

				pValue = dict1.Get(key);
				sprintf(work, "Dict item %d", i);

				Assert::IsTrue(Utils::StringEquals(pValue, work));
			}

			// Lookup missing key.
			Assert::IsFalse(dict1.ContainsKey("missing?"));
			Assert::AreEqual(NULL, dict1.Get("missing?"));
		}


		TEST_METHOD(Test_Dictionary_Remove)
		{
			// Arrange.
			Dictionary dict1 = new Dictionary("dict1");
			char key[20];
			char value[40];
			int count = 100;

			// Act.
			for (int i = 0; i < count; i++)
			{
				sprintf(key, "key%d", i);
				sprintf(value, "Dict item %d", i);
				dict1.Add(key, value);
			}

			// Remove each key, starting at the first.
			int newCount = count;

			for (int i = 0; i < count; i++)
			{
				sprintf(key, "key%d", i);

				Assert::IsTrue(dict1.ContainsKey(key));
				Assert::AreEqual(newCount, dict1.Count());

				dict1.Remove(key);
				newCount--;

				Assert::IsFalse(dict1.ContainsKey(key));
				Assert::AreEqual(NULL, dict1.Get(key));
				Assert::AreEqual(newCount, dict1.Count());
			}
		}


		TEST_METHOD(Test_Dictionary_Update)
		{
			// Arrange.
			Dictionary dict1 = new Dictionary("dict1");
			char key[20];
			char value[40];
			char work[40];
			int count = 100;

			// Act.
			for (int i = 0; i < count; i++)
			{
				sprintf(key, "key%d", i);
				sprintf(value, "Dict item %d", i);
				dict1.Add(key, value);
			}

			strcpy(key, "key40");
			strcpy(value, "??? Dict item 40 ???");
			dict1.Add(key, value);

			// Assert.
			Assert::AreEqual(count, dict1.Count());

			// Lookup each key.
			const char* pValue;

			for (int i = 0; i < count; i++)
			{
				sprintf(key, "key%d", i);
				Assert::IsTrue(dict1.ContainsKey(key));

				pValue = dict1.Get(key);

				if (i == 40)
					strcpy(work, "??? Dict item 40 ???");
				else
					sprintf(work, "Dict item %d", i);

				Assert::IsTrue(Utils::StringEquals(pValue, work));
			}
		}

	};
}

