#include "pch.h"

#include <iostream>

#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Globals.h"
#include "Log.h"
#include "StringList.h"
#include "Utils.h"


void WriteLog2(const char* text)
{
	Logger::WriteMessage(text);
	Logger::WriteMessage("\n");
}




namespace UnitTest1
{
	TEST_CLASS(Test_StringList)
	{
	public:
		TEST_CLASS_INITIALIZE(InitForAllTests)
		{
			// Do something exactly one time for all tests in this class.
			// This is run before the first test starts.

			WriteLog2("--- TEST_CLASS_INITIALIZE(InitForAllTests) ---");

			Log::InUnitTest = true;
			Log::LoggerCallback = &WriteLog2;

			Globals::InitialisedStatic = true;
			Globals::Verbosity = 10;
		}


		TEST_METHOD(Test_Construction)
		{
			// Arrange.
			StringList sl1;
			StringList sl2(200);

			// Act.

			// Assert.
			Assert::AreEqual(20, (int)sl1.Size);
			Assert::AreEqual(0, (int)sl1.Count);
			Assert::AreEqual(200, (int)sl2.Size);
			Assert::AreEqual(0, (int)sl2.Count);
		}


		TEST_METHOD(Test_Clear)
		{
			// Arrange.
			StringList sl1;

			if (Log::InUnitTest)
				Logger::WriteMessage("Log::InUnitTest is TRUE");
			else
				Logger::WriteMessage("Log::InUnitTest is FALSE");
			Logger::WriteMessage("\n");

			// Act.
			sl1.Add("Hello");
			sl1.Add("cruel");
			sl1.Add("World!");
			sl1.LogIt();

			sl1.Clear();

			// Assert.
			Assert::AreEqual(0, (int)sl1.Count);
			Assert::IsTrue(Utils::StringEquals(sl1.Get(0), ""));
		}


		TEST_METHOD(Test_Get)
		{
			// Arrange.
			StringList sl1;

			// Act.
			sl1.Add("Hello");
			sl1.Add("  World!   ");
			sl1.LogIt();

			// Assert.
			Assert::AreEqual(2, (int)sl1.Count);
			Assert::IsTrue(Utils::StringEquals(sl1.Get(0), "Hello"));
			Assert::IsTrue(Utils::StringEquals(sl1.Get(1), "  World!   "));
			Assert::IsTrue(Utils::StringEquals(sl1.Get(2), ""));
		}


		TEST_METHOD(Test_IndexOf)
		{
			// Arrange.
			StringList sl1;

			// Act.
			sl1.Add("Hello");
			sl1.Add("cruel");
			sl1.Add("World!");
			sl1.LogIt();

			int i = sl1.IndexOf("not there");
			int i0 = sl1.IndexOf("Hello");
			int i1 = sl1.IndexOf("cruel");
			int i2 = sl1.IndexOf("World!");

			// Assert.
			Assert::AreEqual(3, (int)sl1.Count);
			Assert::IsTrue(Utils::StringEquals(sl1.Get(0), "Hello"));
			Assert::IsTrue(Utils::StringEquals(sl1.Get(1), "cruel"));
			Assert::IsTrue(Utils::StringEquals(sl1.Get(2), "World!"));
			Assert::AreEqual(-1, i);
			Assert::AreEqual(0, i0);
			Assert::AreEqual(1, i1);
			Assert::AreEqual(2, i2);
		}


		TEST_METHOD(Test_Put)
		{
			// Arrange.
			Log::InUnitTest = true;
			StringList sl1;

			char temp[100];
			char work[100];

			// Act / Assert.
			int count = 12;

			for (int i = 0; i < count; i++)
			{
				sprintf(temp, "Item %d", i);
				sl1.Add(temp);
			}

			for (int i = 0; i < count; i++)
			{
				sprintf(temp, "Item %d", i);
				Assert::IsTrue(Utils::StringEquals(temp, sl1.Get(i), false));
			}

			Assert::AreEqual(count, (int)sl1.Count);

			// Put longer text into item 'index'.
			int index = 3;
			strcpy(work, "123456789012345678901234567890");
			sl1.Put(index, work);

			for (int i = 0; i < count; i++)
			{
				if (i == index)
					strcpy(temp, work);
				else
					sprintf(temp, "Item %d", i);

				Assert::IsTrue(Utils::StringEquals(temp, sl1.Get(i), false));
			}

			// Put shorter text into item 'index'.
			strcpy(work, "1234567890");
			sl1.Put(index, work);

			for (int i = 0; i < count; i++)
			{
				if (i == index)
					strcpy(temp, work);
				else
					sprintf(temp, "Item %d", i);

				Assert::IsTrue(Utils::StringEquals(temp, sl1.Get(i), false));
			}

			Assert::AreEqual(count, (int)sl1.Count);

			sl1.LogIt();
		}


		TEST_METHOD(Test_Put_Extended)
		{
			// Arrange.
			Log::InUnitTest = true;
			StringList sl1;

			char temp[200];
			char work[200];

			// Act / Assert.
			for (int count = 0; count < 12; count++)
			{
				for (int index = 0; index < count; index++)
				{
					sl1.Clear();

					for (int i = 0; i < count; i++)
					{
						sprintf(temp, "Item %d", i);
						sl1.Add(temp);
					}

					for (int i = 0; i < count; i++)
					{
						sprintf(temp, "Item %d", i);
						Assert::IsTrue(Utils::StringEquals(temp, sl1.Get(i), false));
					}

					// 'Put' longer text to item 'index'.
					strcpy(work, "12345678901234567890");
					sl1.Put(index, work);

					for (int i = 0; i < count; i++)
					{
						if (i == index)
							strcpy(temp, work);
						else
							sprintf(temp, "Item %d", i);

						Assert::IsTrue(Utils::StringEquals(temp, sl1.Get(i), false));
					}

					// 'Put' shorter text to item 'index'.
					strcpy(work, "123");
					sl1.Put(index, work);

					for (int i = 0; i < count; i++)
					{
						if (i == index)
							strcpy(temp, work);
						else
							sprintf(temp, "Item %d", i);

						Assert::IsTrue(Utils::StringEquals(temp, sl1.Get(i), false));
					}
				}
			}

			sl1.LogIt();
		}


		TEST_METHOD(Test_Put_OutOfSync)
		{
			// Arrange.
			Log::InUnitTest = true;
			StringList sl1;

			// Act.
			sl1.Put(3, "Item 3");

			sl1.LogIt();

			// Assert.
			Assert::IsTrue(Utils::StringEquals("", sl1.Get(0), false));
			Assert::IsTrue(Utils::StringEquals("Item 3", sl1.Get(3), false));
		}


		TEST_METHOD(Test_RemoveFirst)
		{
			// Arrange.
			StringList sl1;

			// Act.
			sl1.Add("Hello");
			sl1.Add("cruel");
			sl1.Add("World!");
			sl1.LogIt();

			sl1.Remove(0);
			sl1.LogIt();

			// Assert.
			Assert::AreEqual(2, (int)sl1.Count);
			Assert::IsTrue(Utils::StringEquals(sl1.Get(0), "cruel"));
			Assert::IsTrue(Utils::StringEquals(sl1.Get(1), "World!"));
			Assert::IsTrue(Utils::StringEquals(sl1.Get(2), ""));
		}


		TEST_METHOD(Test_RemoveSecond)
		{
			// Arrange.
			StringList sl1;

			// Act.
			sl1.Add("Hello");
			sl1.Add("cruel");
			sl1.Add("World!");
			sl1.LogIt();

			sl1.Remove(1);
			sl1.LogIt();

			// Assert.
			Assert::AreEqual(2, (int)sl1.Count);
			Assert::IsTrue(Utils::StringEquals(sl1.Get(0), "Hello"));
			Assert::IsTrue(Utils::StringEquals(sl1.Get(1), "World!"));
			Assert::IsTrue(Utils::StringEquals(sl1.Get(2), ""));
		}


		TEST_METHOD(Test_RemoveThird)
		{
			// Arrange.
			StringList sl1;

			// Act.
			sl1.Add("Hello");
			sl1.Add("cruel");
			sl1.Add("World!");
			sl1.LogIt();

			sl1.Remove(2);
			sl1.LogIt();

			// Assert.
			Assert::AreEqual(2, (int)sl1.Count);
			Assert::IsTrue(Utils::StringEquals(sl1.Get(0), "Hello"));
			Assert::IsTrue(Utils::StringEquals(sl1.Get(1), "cruel"));
			Assert::IsTrue(Utils::StringEquals(sl1.Get(2), ""));
		}


		TEST_METHOD(Test_SetBuffer)
		{
			// Arrange.
			StringList sl1;

			sl1.LogIt();

			// Act.
			char temp[40];

			for (int i = 0; i < 50; i++)
			{
				sprintf(temp, "Item %d", i);
				sl1.Add(temp);
			}

			sl1.LogIt();

			// Assert.
			Assert::AreEqual(50, (int)sl1.Count);

			for (int i = 0; i < 50; i++)
			{
				sprintf(temp, "Item %d", i);
				Assert::IsTrue(Utils::StringEquals(temp, sl1.Get(i), false));
			}
		}

	};
}
