#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "UrlEncoder.h"



namespace UnitTest1
{
	TEST_CLASS(Test_UrlEncoder)
	{
	public:
		TEST_METHOD(Test_Decode)
		{
			// Arrange.
			char string1[80];
			char string2[80];
			UrlEncoder encoder;

			// Act / Assert.
			strcpy(string1, "name%20=%20value");
			encoder.Decode(string1, string2, 80);
			Assert::IsTrue(_stricmp(string2, "name = value") == 0);
		}


		TEST_METHOD(Test_Encode)
		{
			// Arrange.
			char string1[80];
			char string2[80];
			UrlEncoder encoder;

			// Act / Assert.
			strcpy(string1, "name = value");
			encoder.Encode(string1, string2, 80);
			Assert::IsTrue(_stricmp(string2, "name%20=%20value") == 0);
		}

	};
}

