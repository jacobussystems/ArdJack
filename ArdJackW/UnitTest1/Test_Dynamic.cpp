#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Dynamic.h"
#include "Utils.h"



namespace UnitTest1
{
	TEST_CLASS(Test_Dynamic)
	{
	public:
		TEST_METHOD(Test_Ctor)
		{
			// Arrange.
			Dynamic d1;
			Dynamic d2;

			// Act.
			d2.SetBool(true);

			// Assert.
			Assert::AreEqual(ARDJACK_DATATYPE_EMPTY, d1.DataType());

			Assert::AreEqual(ARDJACK_DATATYPE_BOOLEAN, d2.DataType());
			Assert::AreEqual(true, d2.AsBool());
		}


		TEST_METHOD(Test_Empty)
		{
			// Arrange.
			Dynamic d1;
			Dynamic d2;

			// Act.
			d2.SetBool(true);

			// Assert.
			Assert::IsTrue(d1.IsEmpty());
			Assert::IsFalse(d2.IsEmpty());
		}


		TEST_METHOD(Test_Equals)
		{
			// Arrange.
			Dynamic d1;
			Dynamic d2;
			Dynamic d3;

			// Act.
			d1.SetInt(123);
			d2.SetInt(456);
			d3.SetInt(123);

			// Assert.
			Assert::IsFalse(d1.Equals(&d2));
			Assert::IsTrue(d1.Equals(&d3));
			Assert::IsFalse(d2.Equals(&d3));
		}

	};
}

