#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "FieldReplacer.h"
#include "Globals.h"
#include "Utils.h"



namespace UnitTest1
{
	TEST_CLASS(Test_FieldReplacer)
	{
	public:
		TEST_METHOD(Test_FieldReplacer_1)
		{
			// Arrange.
			FieldReplacer* replacer = new FieldReplacer();

			Dictionary* args = new Dictionary("Test_FieldReplacer_1");;
			args->Add("animal1", "quick brown Fox");
			args->Add("animal2", "lazy doG");

			char expected[102];
			char input[102];
			char output[102];

			strcpy(input, "The [animal1] jumped over the [animal2], watched by [watcher].");
			strcpy(expected, "The quick brown Fox jumped over the lazy doG, watched by (watcher).");

			// Act.
			replacer->ReplaceFields(input, args, &FieldReplacer_ReplaceField, output);

			// Assert.
			Assert::IsTrue(Utils::StringEquals(output, expected));
		}

	};
}

