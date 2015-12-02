#include "stdafx.h"
#include "CppUnitTest.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace register_procedures_test
{
	TEST_CLASS(UnitTest1)
	{
	public:

		TEST_METHOD(TestMethod1)
		{
			using namespace reg;
			Status = 0;
			SetFlagC(true);
			Assert::AreEqual(GetFlagC(), true);
			SetFlagC(false);
			Assert::AreEqual(GetFlagC(), false);

			SetFlagZ(true);
			Assert::AreEqual(GetFlagZ(), true);
			SetFlagZ(false);
			Assert::AreEqual(GetFlagZ(), false);

			SetFlagI(true);
			Assert::AreEqual(GetFlagI(), true);
			SetFlagI(false);
			Assert::AreEqual(GetFlagI(), false);

			SetFlagD(true);
			Assert::AreEqual(GetFlagD(), true);
			SetFlagD(false);
			Assert::AreEqual(GetFlagD(), false);

			SetFlagB(true);
			Assert::AreEqual(GetFlagB(), true);
			SetFlagB(false);
			Assert::AreEqual(GetFlagB(), false);

			SetFlagV(true);
			Assert::AreEqual(GetFlagV(), true);
			SetFlagV(false);
			Assert::AreEqual(GetFlagV(), false);

			SetFlagS(true);
			Assert::AreEqual(GetFlagS(), true);
			SetFlagS(false);
			Assert::AreEqual(GetFlagS(), false);
		}

	};
}