#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "ArrayHelpers.h"
#include "Globals.h"
#include "Utils.h"



namespace UnitTest1
{
	TEST_CLASS(Test_ArrayHelpers)
	{
	public:
		TEST_METHOD(Test_ArrayHelpers_Remove)
		{
			// Arrange.
			int* array[100];
			int data[100];

			for (int i = 0; i < 100; i++)
			{
				data[i] = 100 + i;
				array[i] = &data[i];
			}

			// Act.
			ArrayHelpers::RemoveElement((void**)array, 100, 90);

			// Assert.
			for (int i = 0; i < 90; i++)
			{
				int value = *array[i];
				Assert::AreEqual(100 + i, value);
			}

			for (int i = 90; i < 99; i++)
			{
				int value = *array[i];
				Assert::AreEqual(100 + i + 1, value);
			}

			Assert::AreEqual((void*)NULL, (void*)array[99]);
		}

	};
}

