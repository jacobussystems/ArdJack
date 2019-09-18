#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Globals.h"
#include "Utils.h"




namespace UnitTest1
{		
	TEST_CLASS(Test_Utils_Strings)
	{
	public:
		TEST_METHOD(Test_GetArgs_1)
		{
			// Arrange.
			char string1[] = "verb remainder";

			char first[40];
			const char* remainder = NULL;

			// Act.
			Utils::GetArgs(string1, first, &remainder);

			// Assert.
			Assert::IsTrue(Utils::StringEquals(first, "verb", false));
			Assert::IsTrue(Utils::StringEquals(remainder, "remainder", false));
		}


		TEST_METHOD(Test_GetArgs_2)
		{
			// Arrange.
			char string1[] = "  verb   remainder  of  line  ";

			char first[40];
			const char* remainder = NULL;

			// Act.
			Utils::GetArgs(string1, first, &remainder);

			// Assert.
			Assert::IsTrue(Utils::StringEquals(first, "verb", false));
			Assert::IsTrue(Utils::StringEquals(remainder, "remainder  of  line  ", false));
		}


		TEST_METHOD(Test_GetArgs_3)
		{
			// Arrange.
			char string1[] = "  verb  ;   remainder  of  line  ";

			char first[40];
			const char* remainder = NULL;

			// Act.
			Utils::GetArgs(string1, first, &remainder, ';');

			// Assert.
			Assert::IsTrue(Utils::StringEquals(first, "verb", false));
			Assert::IsTrue(Utils::StringEquals(remainder, "remainder  of  line  ", false));
		}


		TEST_METHOD(Test_StringStartsWith)
		{
			// Arrange.
			char string1[80];

			// Act.
			strcpy(string1, "Iceland");

			// Assert.
			Assert::IsTrue(Utils::StringStartsWith(string1, "Ice", true));
			Assert::IsTrue(Utils::StringStartsWith(string1, "Ice", false));

			Assert::IsTrue(Utils::StringStartsWith(string1, "ICE", true));
			Assert::IsFalse(Utils::StringStartsWith(string1, "ICE", false));

			Assert::IsFalse(Utils::StringStartsWith(string1, "yy", true));
			Assert::IsFalse(Utils::StringStartsWith(string1, "yy", false));
		}


		TEST_METHOD(Test_StringContains)
		{
			// Arrange.
			char string1[80];

			// Act.
			strcpy(string1, "Iceland");

			// Assert.
			Assert::IsTrue(Utils::StringContains(string1, "ela", true));
			Assert::IsTrue(Utils::StringContains(string1, "ela", false));

			Assert::IsTrue(Utils::StringContains(string1, "ela", true));
			Assert::IsFalse(Utils::StringContains(string1, "elA", false));

			Assert::IsFalse(Utils::StringContains(string1, "xx"));
		}

		
		TEST_METHOD(Test_StringEndsWith)
		{
			// Arrange.
			char string1[80];

			// Act.
			strcpy(string1, "Iceland");

			// Assert.
			Assert::IsTrue(Utils::StringEndsWith(string1, "land", true));
			Assert::IsTrue(Utils::StringEndsWith(string1, "land", false));

			Assert::IsTrue(Utils::StringEndsWith(string1, "lanD", true));
			Assert::IsFalse(Utils::StringEndsWith(string1, "lanD", false));

			Assert::IsFalse(Utils::StringEndsWith(string1, "yy", true));
			Assert::IsFalse(Utils::StringEndsWith(string1, "yy", false));
		}


		TEST_METHOD(Test_StringEquals)
		{
			// Arrange.
			char string1[80];

			// Act.
			strcpy(string1, "Iceland");

			// Assert.
			Assert::IsTrue(Utils::StringEquals(string1, "Iceland", true));
			Assert::IsTrue(Utils::StringEquals(string1, "Iceland", false));
			Assert::IsTrue(Utils::StringEquals(string1, "iceland", true));
			Assert::IsFalse(Utils::StringEquals(string1, "iceland", false));
		}


		TEST_METHOD(Test_StringReplace)
		{
			// Arrange.
			char string1[80];
			char string2[80];

			// Act/Assert.
			strcpy(string1, "Iceland");

			Utils::StringReplace(string1, "an", "AN", true, string2, 80);
			Assert::IsTrue(_stricmp(string2, "IcelANd") == 0);

			Utils::StringReplace(string1, "an", "AN", false, string2, 80);
			Assert::IsTrue(_stricmp(string2, "IcelANd") == 0);

			Utils::StringReplace(string1, "an", "", true, string2, 80);
			Assert::IsTrue(_stricmp(string2, "Iceld") == 0);

			Utils::StringReplace(string1, "an", "", false, string2, 80);
			Assert::IsTrue(_stricmp(string2, "Iceld") == 0);
		}


		TEST_METHOD(Test_StringReplaceMultiHex)
		{
			// Arrange.
			char chars[10];
			int count = 0;
			int replacements[10];
			char string1[80];
			char string2[80];

			// Act/Assert.
			strcpy(string1, "Iceland");
			chars[count] = 'e';
			replacements[count++] = 1;
			chars[count] = 'a';
			replacements[count++] = 2;

			Utils::StringReplaceMultiHex(string1, chars, replacements, count, string2, 80);
			Assert::IsTrue(_stricmp(string2, "Ic%01l%02nd") == 0);
		}


		TEST_METHOD(Test_Trim1)
		{
			// Arrange.
			char string1[80];
			char string2[80];
			char string3[80];

			// Act.
			strcpy(string1, "  1");
			Utils::Trim(string1);

			strcpy(string2, "1 ");
			Utils::Trim(string2);

			strcpy(string3, "22  ");
			Utils::Trim(string3);

			// Assert.
			Assert::AreEqual(0, _stricmp(string1, "1"));
			Assert::AreEqual(0, _stricmp(string2, "1"));
			Assert::AreEqual(0, _stricmp(string3, "22"));
		}

	};
}
