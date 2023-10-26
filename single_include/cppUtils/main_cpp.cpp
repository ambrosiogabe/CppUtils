// ===================================================================================
// Test main
// This is just a testing workspace for me while developing this
// do not define GABE_CPP_UTILS_TEST_MAIN unless you are developing 
// this single_include file.
// ===================================================================================
#ifdef GABE_CPP_UTILS_TEST_MAIN

#define GABE_CPP_PRINT_IMPL
#include <cppUtils/cppPrint.hpp>
#undef GABE_CPP_PRINT_IMPL

#define GABE_CPP_UTILS_IMPL
#include <cppUtils/cppUtils.hpp>
#undef GABE_CPP_UTILS_IMPL

#define GABE_CPP_TESTS_IMPL
#include <cppUtils/cppTests.hpp>
#undef GABE_CPP_TESTS_IMPL

#define GABE_CPP_STRING_IMPL
#include <cppUtils/cppStrings.hpp>
#undef GABE_CPP_STRING_IMPL

using namespace CppUtils;

#include <cppUtils/cppMaybe.hpp>

// -------------------- String Test Suite --------------------
namespace StringTestSuite
{
	DEFINE_TEST(utf8String_C080_ShouldBeBad)
	{
		const uint8_t invalidUtf8Data[] = { 0xc0, 0x80, 0x00 };
		auto badString = String::makeString((const char*)invalidUtf8Data);

		ASSERT_FALSE(badString.hasValue());
		ASSERT_EQUAL(badString.error(), Utf8ErrorCode::InvalidString)

			// NOTE: This is unnecessary since the string won't allocate if it's an invalid UTF8 string
			//       but it's nice in case you're not checking for that type of stuff and just want to
			//       pass the data forwards whether it's valid or not
			String::free(badString);

		END_TEST;
	}

	DEFINE_TEST(utf8String_EDA18CEDBEB4_ShouldBeBad)
	{
		const uint8_t invalidUtf8Data[] = { 0xED, 0xA1, 0x8C, 0xED, 0xBE, 0xB4 };
		auto badString = String::makeString((const char*)invalidUtf8Data);

		ASSERT_FALSE(badString.hasValue());
		ASSERT_EQUAL(badString.error(), Utf8ErrorCode::InvalidString)

			END_TEST;
	}

	DEFINE_TEST(validUtf8String_ShouldSucceed)
	{
		const char rawStringLiteral[] = u8"Hello World!";
		auto maybeString = String::makeString(rawStringLiteral);

		ASSERT_TRUE(maybeString.hasValue());

		const BasicString& string = maybeString.value();

		ASSERT_EQUAL(string.numCharacters, sizeof(rawStringLiteral) - 1);
		ASSERT_EQUAL(string.numBytes, sizeof(rawStringLiteral) - 1);
		ASSERT_EQUAL(string, String::makeConstantString("Hello World!").value());

		String::free(maybeString.mut_value());

		END_TEST;
	}

	DEFINE_TEST(validUtf8String_ShouldSucceedWithUnicodeChars)
	{
		const uint32 numCharacters = 29;
		const char rawStringLiteral[] = u8"∏ Test some unicode strings ∏";
		auto maybeString = String::makeString(rawStringLiteral);

		ASSERT_TRUE(maybeString.hasValue());

		const BasicString& string = maybeString.value();

		ASSERT_EQUAL(string.numCharacters, numCharacters);
		ASSERT_EQUAL(string.numBytes, sizeof(rawStringLiteral) - 1);
		ASSERT_EQUAL(string, String::makeConstantString(u8"\u220f Test some unicode strings \u220f").value());

		String::free(maybeString.mut_value());

		END_TEST;
	}

	// Make iter tests
	DEFINE_TEST(utf8Iter_MakeIterShouldStartAt0_WithAscii)
	{
		const uint8_t rawStringLiteral[] = u8"Test some unicode strings";
		auto iter = String::makeIter(rawStringLiteral, sizeof(rawStringLiteral) - 1);

		ASSERT_EQUAL(iter.bytePos, 0);
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	DEFINE_TEST(utf8Iter_MakeIterShouldStartAt0_WithUtf8)
	{
		const uint8_t rawStringLiteral[] = u8"∏ Test some unicode strings ∏";
		auto iter = String::makeIter(rawStringLiteral, sizeof(rawStringLiteral) - 1);

		ASSERT_EQUAL(iter.bytePos, 0);
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	DEFINE_TEST(utf8Iter_MakeIterShouldStartAtFirstValidBoundary_WithAscii)
	{
		const uint8_t rawStringLiteral[] = u8"Test some unicode strings";
		auto iter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, 1);

		ASSERT_EQUAL(iter.bytePos, 1);
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	DEFINE_TEST(utf8Iter_MakeIterShouldStartAtFirstValidBoundary_WithUtf8)
	{
		const uint8_t rawStringLiteral[] = u8"∏ Test some unicode strings ∏";
		auto iter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, 1);

		auto firstCharNumBytes = String::getNumOctets(rawStringLiteral, sizeof(rawStringLiteral) - 1, 0);

		ASSERT_TRUE(firstCharNumBytes.hasValue());
		ASSERT_EQUAL(iter.bytePos, firstCharNumBytes.value());
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	DEFINE_TEST(utf8Iter_MakeIterWithEmptyStringShouldBeFine)
	{
		const uint8_t rawStringLiteral[] = u8"";
		auto iter = String::makeIter(rawStringLiteral, sizeof(rawStringLiteral) - 1);

		ASSERT_EQUAL(iter.bytePos, 0);
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	DEFINE_TEST(utf8Iter_MakeIterWithOverflowShouldClamp)
	{
		const uint8_t rawStringLiteral[] = u8"Hello";
		auto iter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, sizeof(rawStringLiteral));

		ASSERT_EQUAL(iter.bytePos, sizeof(rawStringLiteral) - 1);
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	// ++ operator tests
	DEFINE_TEST(utf8Iter_IterIncrements1Char_WithAscii)
	{
		const uint8_t rawStringLiteral[] = u8"Test some unicode strings";
		auto iter = String::makeIter(rawStringLiteral, sizeof(rawStringLiteral) - 1);

		ASSERT_EQUAL(iter.bytePos, 0);

		++iter;

		auto firstCharNumBytes = String::getNumOctets(rawStringLiteral, sizeof(rawStringLiteral) - 1, 0);

		ASSERT_TRUE(firstCharNumBytes.hasValue());
		ASSERT_EQUAL(iter.bytePos, firstCharNumBytes.value());
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	DEFINE_TEST(utf8Iter_IterIncrements1Char_WithUtf8)
	{
		const uint8_t rawStringLiteral[] = u8"∏ Test some unicode strings ∏";
		auto iter = String::makeIter(rawStringLiteral, sizeof(rawStringLiteral) - 1);

		ASSERT_EQUAL(iter.bytePos, 0);

		++iter;

		auto firstCharNumBytes = String::getNumOctets(rawStringLiteral, sizeof(rawStringLiteral) - 1, 0);

		ASSERT_TRUE(firstCharNumBytes.hasValue());
		ASSERT_EQUAL(iter.bytePos, firstCharNumBytes.value());
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	DEFINE_TEST(utf8Iter_IterIncrementsClampsWithOverflow_WithAscii)
	{
		const uint8_t rawStringLiteral[] = u8"Test some unicode strings";
		const size_t bytePosStart = sizeof(rawStringLiteral) - 1;
		auto iter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, bytePosStart);

		ASSERT_EQUAL(iter.bytePos, bytePosStart);

		++iter;

		ASSERT_EQUAL(iter.bytePos, sizeof(rawStringLiteral) - 1);
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	DEFINE_TEST(utf8Iter_IterIncrementsClampsWithOverflow_WithUtf8)
	{
		const uint8_t rawStringLiteral[] = u8"∏ Test some unicode strings ∏";
		const size_t bytePosStart = sizeof(rawStringLiteral) - 1;
		auto iter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, bytePosStart);

		ASSERT_EQUAL(iter.bytePos, bytePosStart);

		++iter;

		ASSERT_EQUAL(iter.bytePos, sizeof(rawStringLiteral) - 1);
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	// -- operator tests
	DEFINE_TEST(utf8Iter_IterDecrements1Char_WithAscii)
	{
		const uint8_t rawStringLiteral[] = u8"Test some unicode strings";
		const size_t bytePosStart = 1;
		auto iter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, bytePosStart);

		ASSERT_EQUAL(iter.bytePos, bytePosStart);

		--iter;

		ASSERT_EQUAL(iter.bytePos, 0);
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	DEFINE_TEST(utf8Iter_IterDecrements1Char_WithUtf8)
	{
		// ∏ is 3 bytes long
		const uint8_t rawStringLiteral[] = u8"∏ Test some unicode strings ∏";
		const size_t bytePosStart = 3;
		auto iter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, bytePosStart);

		ASSERT_EQUAL(iter.bytePos, bytePosStart);

		--iter;

		ASSERT_EQUAL(iter.bytePos, 0);
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	DEFINE_TEST(utf8Iter_IterDecrementsClampsWithUnderflow_WithAscii)
	{
		const uint8_t rawStringLiteral[] = u8"Test some unicode strings";
		const size_t bytePosStart = 0;
		auto iter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, bytePosStart);

		ASSERT_EQUAL(iter.bytePos, bytePosStart);

		--iter;

		ASSERT_EQUAL(iter.bytePos, 0);
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	DEFINE_TEST(utf8Iter_IterDecrementsClampsWithUnderflow_WithUtf8)
	{
		const uint8_t rawStringLiteral[] = u8"∏ Test some unicode strings ∏";
		const size_t bytePosStart = 0;
		auto iter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, bytePosStart);

		ASSERT_EQUAL(iter.bytePos, bytePosStart);

		--iter;

		ASSERT_EQUAL(iter.bytePos, 0);
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	// + operator tests
	DEFINE_TEST(utf8Iter_PlusOperatorWithMultipleCharsIsValid_WithAscii)
	{
		const uint8_t rawStringLiteral[] = u8"Test some unicode strings";
		constexpr uint8_t bytePosStart = 4;
		auto iter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, bytePosStart);

		ASSERT_EQUAL(iter.bytePos, bytePosStart);

		const uint8_t numCharsToInc = 2;
		uint8_t numBytesToInc = 0;
		for (size_t i = 0; i < numCharsToInc; i++)
		{
			auto numBytes = String::getNumOctets(iter.data, sizeof(rawStringLiteral) - 1, bytePosStart + i);
			ASSERT_TRUE(numBytes.hasValue());

			numBytesToInc += numBytes.value();
		}

		iter = iter + numCharsToInc;

		ASSERT_EQUAL(iter.bytePos, bytePosStart + numBytesToInc);
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	DEFINE_TEST(utf8Iter_PlusOperatorWithMultipleCharsIsValid_WithUtf8)
	{
		const uint8_t rawStringLiteral[] = u8"Test ∏some unicode strings";
		constexpr uint8_t bytePosStart = 4;
		auto iter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, bytePosStart);

		ASSERT_EQUAL(iter.bytePos, bytePosStart);

		const uint8_t numCharsToInc = 2;
		uint8_t numBytesToInc = 0;
		for (size_t i = 0; i < numCharsToInc; i++)
		{
			auto numBytes = String::getNumOctets(iter.data, sizeof(rawStringLiteral) - 1, bytePosStart + i);
			ASSERT_TRUE(numBytes.hasValue());

			numBytesToInc += numBytes.value();
		}

		iter = iter + numCharsToInc;

		ASSERT_EQUAL(iter.bytePos, bytePosStart + numBytesToInc);
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	DEFINE_TEST(utf8Iter_PlusOperatorOverflowClamps_WithAscii)
	{
		const uint8_t rawStringLiteral[] = u8"Test some unicode strings";
		constexpr uint8_t bytePosStart = sizeof(rawStringLiteral) - 1;
		auto iter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, bytePosStart);

		ASSERT_EQUAL(iter.bytePos, bytePosStart);

		iter = iter + 1;

		ASSERT_EQUAL(iter.bytePos, sizeof(rawStringLiteral) - 1);
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	DEFINE_TEST(utf8Iter_PlusOperatorOverflowClamps_WithUtf8)
	{
		const uint8_t rawStringLiteral[] = u8"Test ∏some unicode strings";
		constexpr uint8_t bytePosStart = sizeof(rawStringLiteral) - 1;
		auto iter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, bytePosStart);

		ASSERT_EQUAL(iter.bytePos, bytePosStart);

		iter = iter + 1;

		ASSERT_EQUAL(iter.bytePos, sizeof(rawStringLiteral) - 1);
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	// - operator tests
	DEFINE_TEST(utf8Iter_MinusOperatorWithMultipleCharsIsValid_WithAscii)
	{
		const uint8_t rawStringLiteral[] = u8"Test some unicode strings";
		constexpr uint8_t bytePosStart = 6;
		auto iter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, bytePosStart);

		ASSERT_EQUAL(iter.bytePos, bytePosStart);

		const uint8_t numCharsToDec = 2;
		uint8_t numBytesToDec = 2;

		iter = iter - numCharsToDec;

		ASSERT_EQUAL(iter.bytePos, bytePosStart - numBytesToDec);
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	DEFINE_TEST(utf8Iter_MinusOperatorWithMultipleCharsIsValid_WithUtf8)
	{
		// NOTE: ∏ is 3 bytes long
		const uint8_t rawStringLiteral[] = u8"Test ∏some unicode strings";
		constexpr uint8_t bytePosStart = 8;
		auto iter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, bytePosStart);

		ASSERT_EQUAL(iter.bytePos, bytePosStart);

		const uint8_t numCharsToDec = 2;
		uint8_t numBytesToDec = 4;

		iter = iter - numCharsToDec;

		ASSERT_EQUAL(iter.bytePos, bytePosStart - numBytesToDec);
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	DEFINE_TEST(utf8Iter_MinusOperatorUnderflowClamps_WithAscii)
	{
		const uint8_t rawStringLiteral[] = u8"Test some unicode strings";
		constexpr uint8_t bytePosStart = 0;
		auto iter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, bytePosStart);

		ASSERT_EQUAL(iter.bytePos, bytePosStart);

		iter = iter - 1;

		ASSERT_EQUAL(iter.bytePos, 0);
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	DEFINE_TEST(utf8Iter_MinusOperatorUnderflowClamps_WithUtf8)
	{
		const uint8_t rawStringLiteral[] = u8"∏Test some unicode strings";
		constexpr uint8_t bytePosStart = 0;
		auto iter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, bytePosStart);

		ASSERT_EQUAL(iter.bytePos, bytePosStart);

		iter = iter - 1;

		ASSERT_EQUAL(iter.bytePos, 0);
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	// += operator tests
	DEFINE_TEST(utf8Iter_PlusEqualsOperatorWithMultipleCharsIsValid_WithAscii)
	{
		const uint8_t rawStringLiteral[] = u8"Test some unicode strings";
		constexpr uint8_t bytePosStart = 4;
		auto controlIter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, bytePosStart);
		auto iter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, bytePosStart);

		ASSERT_EQUAL(iter.bytePos, bytePosStart);

		const uint8_t numCharsToInc = 2;
		// We know this operator works from previous tests so we can use it to compare if the += operator also works
		controlIter = controlIter + numCharsToInc;
		iter += numCharsToInc;

		ASSERT_EQUAL(iter.bytePos, controlIter.bytePos);
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	DEFINE_TEST(utf8Iter_PlusEqualsOperatorWithMultipleCharsIsValid_WithUtf8)
	{
		const uint8_t rawStringLiteral[] = u8"Test ∏some unicode strings";
		constexpr uint8_t bytePosStart = 4;
		auto controlIter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, bytePosStart);
		auto iter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, bytePosStart);

		ASSERT_EQUAL(iter.bytePos, bytePosStart);

		const uint8_t numCharsToInc = 2;
		controlIter = controlIter + numCharsToInc;
		iter += numCharsToInc;

		ASSERT_EQUAL(iter.bytePos, controlIter.bytePos);
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	DEFINE_TEST(utf8Iter_PlusEqualsOperatorOverflowClamps_WithAscii)
	{
		const uint8_t rawStringLiteral[] = u8"Test some unicode strings";
		constexpr uint8_t bytePosStart = sizeof(rawStringLiteral) - 1;
		auto iter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, bytePosStart);

		ASSERT_EQUAL(iter.bytePos, bytePosStart);

		iter += 1;

		ASSERT_EQUAL(iter.bytePos, sizeof(rawStringLiteral) - 1);
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	DEFINE_TEST(utf8Iter_PlusEqualsOperatorOverflowClamps_WithUtf8)
	{
		const uint8_t rawStringLiteral[] = u8"Test ∏some unicode strings";
		constexpr uint8_t bytePosStart = sizeof(rawStringLiteral) - 1;
		auto iter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, bytePosStart);

		ASSERT_EQUAL(iter.bytePos, bytePosStart);

		iter += 1;

		ASSERT_EQUAL(iter.bytePos, sizeof(rawStringLiteral) - 1);
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	// -= operator tests
	DEFINE_TEST(utf8Iter_MinusEqualsOperatorWithMultipleCharsIsValid_WithAscii)
	{
		const uint8_t rawStringLiteral[] = u8"Test some unicode strings";
		constexpr uint8_t bytePosStart = 6;
		auto controlIter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, bytePosStart);
		auto iter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, bytePosStart);

		ASSERT_EQUAL(iter.bytePos, bytePosStart);

		const uint8_t numCharsToDec = 2;
		controlIter = controlIter - numCharsToDec;
		iter -= numCharsToDec;

		ASSERT_EQUAL(iter.bytePos, controlIter.bytePos);
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	DEFINE_TEST(utf8Iter_MinusEqualsOperatorWithMultipleCharsIsValid_WithUtf8)
	{
		// NOTE: ∏ is 3 bytes long
		const uint8_t rawStringLiteral[] = u8"Test ∏some unicode strings";
		constexpr uint8_t bytePosStart = 8;
		auto controlIter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, bytePosStart);
		auto iter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, bytePosStart);

		ASSERT_EQUAL(iter.bytePos, bytePosStart);

		const uint8_t numCharsToDec = 2;

		controlIter = controlIter - numCharsToDec;
		iter -= numCharsToDec;

		ASSERT_EQUAL(iter.bytePos, controlIter.bytePos);
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	DEFINE_TEST(utf8Iter_MinusEqualsOperatorUnderflowClamps_WithAscii)
	{
		const uint8_t rawStringLiteral[] = u8"Test some unicode strings";
		constexpr uint8_t bytePosStart = 0;
		auto iter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, bytePosStart);

		ASSERT_EQUAL(iter.bytePos, bytePosStart);

		iter -= 1;

		ASSERT_EQUAL(iter.bytePos, 0);
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	DEFINE_TEST(utf8Iter_MinusEqualsOperatorUnderflowClamps_WithUtf8)
	{
		const uint8_t rawStringLiteral[] = u8"∏Test some unicode strings";
		constexpr uint8_t bytePosStart = 0;
		auto iter = String::makeIterFromBytePos(rawStringLiteral, sizeof(rawStringLiteral) - 1, bytePosStart);

		ASSERT_EQUAL(iter.bytePos, bytePosStart);

		iter -= 1;

		ASSERT_EQUAL(iter.bytePos, 0);
		ASSERT_EQUAL(iter.numBytes, sizeof(rawStringLiteral) - 1);

		END_TEST;
	}

	void setupCppStringsTestSuite()
	{
		Tests::TestSuite& testSuite = Tests::addTestSuite("cppStrings.hpp");

		// BasicString tests
		ADD_TEST(testSuite, utf8String_C080_ShouldBeBad);
		ADD_TEST(testSuite, utf8String_EDA18CEDBEB4_ShouldBeBad);
		ADD_TEST(testSuite, validUtf8String_ShouldSucceed);
		ADD_TEST(testSuite, validUtf8String_ShouldSucceedWithUnicodeChars);

		// Make Iter tests
		ADD_TEST(testSuite, utf8Iter_MakeIterShouldStartAt0_WithAscii);
		ADD_TEST(testSuite, utf8Iter_MakeIterShouldStartAt0_WithUtf8);
		ADD_TEST(testSuite, utf8Iter_MakeIterShouldStartAtFirstValidBoundary_WithAscii);
		ADD_TEST(testSuite, utf8Iter_MakeIterShouldStartAtFirstValidBoundary_WithUtf8);
		ADD_TEST(testSuite, utf8Iter_MakeIterWithEmptyStringShouldBeFine);
		ADD_TEST(testSuite, utf8Iter_MakeIterWithOverflowShouldClamp);

		// ++ operator tests
		ADD_TEST(testSuite, utf8Iter_IterIncrements1Char_WithAscii);
		ADD_TEST(testSuite, utf8Iter_IterIncrements1Char_WithUtf8);
		ADD_TEST(testSuite, utf8Iter_IterIncrementsClampsWithOverflow_WithAscii);
		ADD_TEST(testSuite, utf8Iter_IterIncrementsClampsWithOverflow_WithUtf8);

		// -- operator tests
		ADD_TEST(testSuite, utf8Iter_IterDecrements1Char_WithAscii);
		ADD_TEST(testSuite, utf8Iter_IterDecrements1Char_WithUtf8);
		ADD_TEST(testSuite, utf8Iter_IterDecrementsClampsWithUnderflow_WithAscii);
		ADD_TEST(testSuite, utf8Iter_IterDecrementsClampsWithUnderflow_WithUtf8);

		// + operator tests
		ADD_TEST(testSuite, utf8Iter_PlusOperatorWithMultipleCharsIsValid_WithAscii);
		ADD_TEST(testSuite, utf8Iter_PlusOperatorWithMultipleCharsIsValid_WithUtf8);
		ADD_TEST(testSuite, utf8Iter_PlusOperatorOverflowClamps_WithAscii);
		ADD_TEST(testSuite, utf8Iter_PlusOperatorOverflowClamps_WithUtf8);

		// - operator tests
		ADD_TEST(testSuite, utf8Iter_MinusOperatorWithMultipleCharsIsValid_WithAscii);
		ADD_TEST(testSuite, utf8Iter_MinusOperatorWithMultipleCharsIsValid_WithUtf8);
		ADD_TEST(testSuite, utf8Iter_MinusOperatorUnderflowClamps_WithAscii);
		ADD_TEST(testSuite, utf8Iter_MinusOperatorUnderflowClamps_WithUtf8);

		// += operator tests
		ADD_TEST(testSuite, utf8Iter_PlusEqualsOperatorWithMultipleCharsIsValid_WithAscii);
		ADD_TEST(testSuite, utf8Iter_PlusEqualsOperatorWithMultipleCharsIsValid_WithUtf8);
		ADD_TEST(testSuite, utf8Iter_PlusEqualsOperatorOverflowClamps_WithAscii);
		ADD_TEST(testSuite, utf8Iter_PlusEqualsOperatorOverflowClamps_WithUtf8);

		// -= operator tests
		ADD_TEST(testSuite, utf8Iter_MinusEqualsOperatorWithMultipleCharsIsValid_WithAscii);
		ADD_TEST(testSuite, utf8Iter_MinusEqualsOperatorWithMultipleCharsIsValid_WithUtf8);
		ADD_TEST(testSuite, utf8Iter_MinusEqualsOperatorUnderflowClamps_WithAscii);
		ADD_TEST(testSuite, utf8Iter_MinusEqualsOperatorUnderflowClamps_WithUtf8);
	}
}

// -------------------- Maybe Test Suite --------------------
namespace MaybeTestSuite
{
	DEFINE_TEST(dummy)
	{
		ASSERT_TRUE(false);
		END_TEST;
	}

	void setupMaybeTestSuite()
	{
		Tests::TestSuite& testSuite = Tests::addTestSuite("cppMaybe.hpp");

		ADD_TEST(testSuite, dummy);
	}
}

// -------------------- Print Test Suite --------------------
namespace PrintTestSuite
{
	#include <Windows.h>
	#include <stdio.h>
	#include <io.h>
	#include <fcntl.h>

	static HANDLE pipeRead = NULL;
	static HANDLE pipeWrite = NULL;
	static char printBuffer[2048 * 10];
	static HANDLE oldStdoutHandle = INVALID_HANDLE_VALUE;
	static int oldCStdout = 0;
	static const char* stdoutFilename = "testOutput.txt";

	DEFINE_BEFORE_EACH(setupTest)
	{
		oldStdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		BOOL pipeCreateRes = CreatePipe(
			&pipeRead,
			&pipeWrite,
			NULL,
			sizeof(printBuffer)
		);

		if (pipeCreateRes == 0)
		{
			throw std::runtime_error("Failed to create stdout redirect pipe.");
		}

		oldCStdout = _dup(1);

		int fd = _open_osfhandle((intptr_t)pipeWrite, _O_APPEND);
		int res = _dup2(fd, 1);
		if (res == -1)
		{
			throw std::runtime_error("Failed to redirect stdout to anonymous pipe.");
		}
		setvbuf(stdout, NULL, _IONBF, 0); //Disable buffering.

		SetStdHandle(STD_OUTPUT_HANDLE, pipeWrite);

		END_BEFORE_EACH;
	}

	DEFINE_AFTER_EACH(teardownTest)
	{
		if (pipeRead != NULL)
		{
			CloseHandle(pipeRead);
		}

		if (pipeWrite != NULL)
		{
			CloseHandle(pipeWrite);
		}

		SetStdHandle(STD_OUTPUT_HANDLE, oldStdoutHandle);

		int res = _dup2(oldCStdout, 1);
		if (res == -1)
		{
			throw std::runtime_error("Failed to redirect stdout to anonymous pipe.");
		}

		END_AFTER_EACH;
	}

	static const char* compareMemory(const uint8_t* expectedOutput, size_t expectedOutputSize)
	{
		DWORD numBytesRead;
		BOOL res = ReadFile(
			pipeRead,
			(void*)&printBuffer[0],
			sizeof(printBuffer),
			&numBytesRead,
			NULL
		);

		ASSERT_NOT_EQUAL(res, 0);
		ASSERT_EQUAL(numBytesRead, expectedOutputSize * 2);

		bool memoryIsAsExpected = g_memory_compareMem(
			(uint8_t*)printBuffer, expectedOutputSize,
			(uint8_t*)expectedOutput, expectedOutputSize
		);
		ASSERT_TRUE(memoryIsAsExpected);

		bool memoryIsEqual = g_memory_compareMem(
			(uint8_t*)printBuffer, expectedOutputSize,
			(uint8_t*)printBuffer + expectedOutputSize, expectedOutputSize);
		ASSERT_TRUE(memoryIsEqual);

		return nullptr;
	}

	static const char* compareMemoryPrintfOnly(const uint8_t* expectedOutput, size_t expectedOutputSize)
	{
		DWORD numBytesRead;
		BOOL res = ReadFile(
			pipeRead,
			(void*)&printBuffer[0],
			sizeof(printBuffer),
			&numBytesRead,
			NULL
		);

		ASSERT_NOT_EQUAL(res, 0);
		ASSERT_EQUAL(numBytesRead, expectedOutputSize);

		bool memoryIsAsExpected = g_memory_compareMem(
			(uint8_t*)printBuffer, expectedOutputSize,
			(uint8_t*)expectedOutput, expectedOutputSize
		);
		ASSERT_TRUE(memoryIsAsExpected);

		return nullptr;
	}

	DEFINE_TEST(hexOutputIsSameAsPrintf)
	{
		using TupleType = std::tuple<std::string, std::string, std::string, uint32_t>;
		const std::vector<TupleType> tests = {
			TupleType{     "0XABCD\n",   "%#6X\n",   "{:#6X}\n", 0xABCD },
			TupleType{     "0xabcd\n",   "%#6x\n",   "{:#6x}\n", 0xABCD },
			TupleType{       "abcd\n",    "%4x\n",    "{:4x}\n", 0xabcd },
			TupleType{ "0x0000ffcc\n", "%#010x\n", "{:#010x}\n", 0xffcc }
		};

		for (size_t i = 0; i < tests.size(); i++)
		{
			auto [expectedOutput, cFormatStr, myFormatStr, number] = tests[i];
			printf(cFormatStr.c_str(), number);
			IO::printf(myFormatStr.c_str(), number);

			const char* res = compareMemory((const uint8_t*)expectedOutput.c_str(), expectedOutput.length());
			if (res)
			{
				return res;
			}
		}

		END_TEST;
	}

	DEFINE_TEST(floatOutputIsSameAsPrintf)
	{
		#pragma warning( push )
		#pragma warning( disable : 4616)
		float one = 1.0f;
		float zero = 0.0f;
		using TupleType = std::tuple<std::string, std::string, std::string, float>;
		const std::vector<TupleType> tests = {
			TupleType{ "3.14\n", "%.2f\n", "{:.2f}\n", 3.145f },
			TupleType{ "3.15\n", "%.2f\n", "{:.2f}\n", 3.146f },
			TupleType{ "0.100000\n", "%f\n", "{}\n", 0.1f },
			TupleType{ "1.000000\n", "%f\n", "{}\n", 1.000000f },
			TupleType{ "inf\n", "%f\n", "{:f}\n", one / zero },
			TupleType{ "INF\n", "%F\n", "{:F}\n", one / zero },
			TupleType{ "-inf\n", "%f\n", "{:f}\n", -one / zero },
			TupleType{ "-INF\n", "%F\n", "{:F}\n", -one / zero },
			TupleType{ "2.1200\n", "%6.4f\n", "{0:6.4f}\n", 2.12f },
			TupleType{ "  2.1200\n", "%8.4f\n", "{ :8.4f}\n", 2.12f },
			TupleType{ "-0.066017\n", "%f\n", "{}\n", -0.066017486155033112f}
		};

		for (size_t i = 0; i < tests.size(); i++)
		{
			auto [expectedOutput, cFormatStr, myFormatStr, number] = tests[i];
			printf(cFormatStr.c_str(), number);
			IO::printf(myFormatStr.c_str(), number);

			const char* res = compareMemory((const uint8_t*)expectedOutput.c_str(), expectedOutput.length());
			if (res)
			{
				return res;
			}
		}

		// Test NAN separately since it does weird stuff
		float a = 0.0f;
		float b = 0.0f;
		IO::printf("{:F}\n", a / b);
		const char* res = compareMemoryPrintfOnly((const uint8_t*)"NAN\n", sizeof("NAN\n") - 1);
		if (res)
		{
			return res;
		}

		// Test exponential degradation (printf doesn't support this, but we do)
		// The number should degrade to exponential format if it doesn't fit in the precision
		IO::printf("{}\n", 1.17E-41f);
		res = compareMemoryPrintfOnly((const uint8_t*)"1.169944e-41\n", sizeof("1.169944e-41\n") - 1);
		if (res)
		{
			return res;
		}

		#pragma warning( pop )

		END_TEST;
	}

	DEFINE_TEST(exponentialFloatOutputIsSameAsPrintf)
	{
		ASSERT_TRUE(false);
		END_TEST;
	}

	DEFINE_TEST(binaryOutputIsCorrect)
	{
		using TupleType = std::tuple<std::string, std::string, uint32_t>;
		const std::vector<TupleType> tests = {
			TupleType{ "0000'0111\n", "{:b}\n", 7 },
			TupleType{ "0000'1000'0000'0111\n", "{:<19b}\n", 2055 },
			TupleType{ "0b0000'0111\n", "{:<#b}\n", 7 },
			TupleType{ "0b0000'1000'0000'0111\n", "{:<#b}\n", 2055 },
			TupleType{ "0B0000'0111\n", "{:<#B}\n", 7 },
			TupleType{ "0B0000'1000'0000'0111\n", "{:<#B}\n", 2055 },
		};

		for (size_t i = 0; i < tests.size(); i++)
		{
			auto [expectedOutput, myFormatStr, number] = tests[i];
			IO::printf(myFormatStr.c_str(), number);

			const char* res = compareMemoryPrintfOnly((const uint8_t*)expectedOutput.c_str(), expectedOutput.length());
			if (res)
			{
				return res;
			}
		}

		END_TEST;
	}

	DEFINE_TEST(leftAlignIsCorrect)
	{
		// Test left-alignment for strings
		{
			using TupleType = std::tuple<std::string, std::string, std::string, const char*>;
			const std::vector<TupleType> tests = {
				TupleType{ "Hello World!    \n", "%-16s\n", "{:<16}\n", "Hello World!" },
				TupleType{ "Hello World!\n", "%-8s\n", "{:<8}\n", "Hello World!" },
			};

			for (size_t i = 0; i < tests.size(); i++)
			{
				auto [expectedOutput, cFormatStr, myFormatStr, number] = tests[i];
				printf(cFormatStr.c_str(), number);
				IO::printf(myFormatStr.c_str(), number);

				const char* res = compareMemory((const uint8_t*)expectedOutput.c_str(), expectedOutput.length());
				if (res)
				{
					return res;
				}
			}
		}

		// Test left-alignment for hex ints (and 0-padded hex ints)
		{
			using TupleType = std::tuple<std::string, std::string, std::string, uint32_t>;
			const std::vector<TupleType> tests = {
				TupleType{ "0XABCD  \n", "%#-8X\n", "{:<#8X}\n", 0xABCD },
				TupleType{ "0XABCD\n", "%#-3X\n", "{:<#3X}\n", 0xABCD },
				TupleType{ "0X00ABCD\n", "%#08X\n", "{:<#08X}\n", 0xABCD },
				TupleType{ "ABCD    \n", "%-8X\n", "{:<8X}\n", 0xABCD },
				TupleType{ "ABCD\n", "%-3X\n", "{:<3X}\n", 0xABCD },
				TupleType{ "0000ABCD\n", "%08X\n", "{:<08X}\n", 0xABCD },
			};

			for (size_t i = 0; i < tests.size(); i++)
			{
				auto [expectedOutput, cFormatStr, myFormatStr, number] = tests[i];
				printf(cFormatStr.c_str(), number);
				IO::printf(myFormatStr.c_str(), number);

				const char* res = compareMemory((const uint8_t*)expectedOutput.c_str(), expectedOutput.length());
				if (res)
				{
					return res;
				}
			}
		}

		// Test left-alignment for floats
		{
			using TupleType = std::tuple<std::string, std::string, std::string, float>;
			const std::vector<TupleType> tests = {
				TupleType{ "1.123  \n", "%-7.3f\n", "{:<7.3f}\n", 1.123f },
				TupleType{ "1.123\n", "%-2.3f\n", "{:<2.3f}\n", 1.123f },
			};

			for (size_t i = 0; i < tests.size(); i++)
			{
				auto [expectedOutput, cFormatStr, myFormatStr, number] = tests[i];
				printf(cFormatStr.c_str(), number);
				IO::printf(myFormatStr.c_str(), number);

				const char* res = compareMemory((const uint8_t*)expectedOutput.c_str(), expectedOutput.length());
				if (res)
				{
					return res;
				}
			}
		}

		// Test left-alignment for binary
		{
			using TupleType = std::tuple<std::string, std::string, uint32_t>;
			const std::vector<TupleType> tests = {
				TupleType{ "0000'0111          \n", "{:<19b}\n", 7 },
				TupleType{ "0000'1000'0000'0111\n", "{:<19b}\n", 2055 },
				TupleType{ "0b0000'0111          \n", "{:<#21b}\n", 7 },
				TupleType{ "0b0000'1000'0000'0111\n", "{:<#21b}\n", 2055 },
			};

			for (size_t i = 0; i < tests.size(); i++)
			{
				auto [expectedOutput, myFormatStr, number] = tests[i];
				IO::printf(myFormatStr.c_str(), number);

				const char* res = compareMemoryPrintfOnly((const uint8_t*)expectedOutput.c_str(), expectedOutput.length());
				if (res)
				{
					return res;
				}
			}
		}

		END_TEST;
	}

	DEFINE_TEST(rightAlignIsCorrect)
	{
		// Test right-alignment for strings
		{
			using TupleType = std::tuple<std::string, std::string, std::string, const char*>;
			const std::vector<TupleType> tests = {
				TupleType{ "    Hello World!\n", "%16s\n", "{:>16}\n", "Hello World!" },
				TupleType{ "Hello World!\n", "%8s\n", "{:>8}\n", "Hello World!" },
			};

			for (size_t i = 0; i < tests.size(); i++)
			{
				auto [expectedOutput, cFormatStr, myFormatStr, number] = tests[i];
				printf(cFormatStr.c_str(), number);
				IO::printf(myFormatStr.c_str(), number);

				const char* res = compareMemory((const uint8_t*)expectedOutput.c_str(), expectedOutput.length());
				if (res)
				{
					return res;
				}
			}
		}

		// Test right-alignment for hex ints (and 0-padded hex ints)
		{
			using TupleType = std::tuple<std::string, std::string, std::string, uint32_t>;
			const std::vector<TupleType> tests = {
				TupleType{ "  0XABCD\n", "%#8X\n", "{:>#8X}\n", 0xABCD },
				TupleType{ "0XABCD\n", "%#3X\n", "{:>#3X}\n", 0xABCD },
				TupleType{ "0X00ABCD\n", "%#08X\n", "{:>#08X}\n", 0xABCD },
				TupleType{ "    ABCD\n", "%8X\n", "{:>8X}\n", 0xABCD },
				TupleType{ "ABCD\n", "%3X\n", "{:>3X}\n", 0xABCD },
				TupleType{ "0000ABCD\n", "%08X\n", "{:>08X}\n", 0xABCD },
			};

			for (size_t i = 0; i < tests.size(); i++)
			{
				auto [expectedOutput, cFormatStr, myFormatStr, number] = tests[i];
				printf(cFormatStr.c_str(), number);
				IO::printf(myFormatStr.c_str(), number);

				const char* res = compareMemory((const uint8_t*)expectedOutput.c_str(), expectedOutput.length());
				if (res)
				{
					return res;
				}
			}
		}

		// Test right-alignment for floats
		{
			using TupleType = std::tuple<std::string, std::string, std::string, float>;
			const std::vector<TupleType> tests = {
				TupleType{ "  1.123\n", "%7.3f\n", "{:>7.3f}\n", 1.123f },
				TupleType{ "1.123\n", "%2.3f\n", "{:>2.3f}\n", 1.123f },
			};

			for (size_t i = 0; i < tests.size(); i++)
			{
				auto [expectedOutput, cFormatStr, myFormatStr, number] = tests[i];
				printf(cFormatStr.c_str(), number);
				IO::printf(myFormatStr.c_str(), number);

				const char* res = compareMemory((const uint8_t*)expectedOutput.c_str(), expectedOutput.length());
				if (res)
				{
					return res;
				}
			}
		}

		// Test right-alignment for binary
		{
			using TupleType = std::tuple<std::string, std::string, uint32_t>;
			const std::vector<TupleType> tests = {
				TupleType{ "          0000'0111\n", "{:>19b}\n", 7 },
				TupleType{ "0000'1000'0000'0111\n", "{:>19b}\n", 2055 },
				TupleType{ "          0b0000'0111\n", "{:>#21b}\n", 7 },
				TupleType{ "0b0000'1000'0000'0111\n", "{:>#21b}\n", 2055 },
			};

			for (size_t i = 0; i < tests.size(); i++)
			{
				auto [expectedOutput, myFormatStr, number] = tests[i];
				IO::printf(myFormatStr.c_str(), number);

				const char* res = compareMemoryPrintfOnly((const uint8_t*)expectedOutput.c_str(), expectedOutput.length());
				if (res)
				{
					return res;
				}
			}
		}

		END_TEST;
	}

	DEFINE_TEST(centerAlignIsCorrect)
	{
		// Test center-alignment for strings
		{
			using TupleType = std::tuple<std::string, std::string, const char*>;
			const std::vector<TupleType> tests = {
				TupleType{ "  Hello World!  \n", "{:^16}\n", "Hello World!" },
				TupleType{ "Hello World!\n", "{:^8}\n", "Hello World!" },
				TupleType{ "   Hello World!  \n", "{:^17}\n", "Hello World!" },
			};

			for (size_t i = 0; i < tests.size(); i++)
			{
				auto [expectedOutput, myFormatStr, number] = tests[i];
				IO::printf(myFormatStr.c_str(), number);

				const char* res = compareMemoryPrintfOnly((const uint8_t*)expectedOutput.c_str(), expectedOutput.length());
				if (res)
				{
					return res;
				}
			}
		}

		// Test center-alignment for hex ints (and 0-padded hex ints)
		{
			using TupleType = std::tuple<std::string, std::string, uint32_t>;
			const std::vector<TupleType> tests = {
				TupleType{ " 0XABCD \n", "{:^#8X}\n", 0xABCD },
				TupleType{ "  0XABCD \n", "{:^#9X}\n", 0xABCD },
				TupleType{ "0XABCD\n", "{:^#3X}\n", 0xABCD },
				TupleType{ "0X00ABCD\n", "{:^#08X}\n", 0xABCD },
				TupleType{ "  ABCD  \n", "{:^8X}\n", 0xABCD },
				TupleType{ "   ABCD  \n", "{:^9X}\n", 0xABCD },
				TupleType{ "ABCD\n", "{:^3X}\n", 0xABCD },
				TupleType{ "0000ABCD\n", "{:^08X}\n", 0xABCD },
			};

			for (size_t i = 0; i < tests.size(); i++)
			{
				auto [expectedOutput, myFormatStr, number] = tests[i];
				IO::printf(myFormatStr.c_str(), number);

				const char* res = compareMemoryPrintfOnly((const uint8_t*)expectedOutput.c_str(), expectedOutput.length());
				if (res)
				{
					return res;
				}
			}
		}

		// Test center-alignment for floats
		{
			using TupleType = std::tuple<std::string, std::string, float>;
			const std::vector<TupleType> tests = {
				TupleType{ " 1.123 \n", "{:^7.3f}\n", 1.123f },
				TupleType{ "  1.123 \n", "{:^8.3f}\n", 1.123f },
				TupleType{ "1.123\n", "{:^2.3f}\n", 1.123f },
			};

			for (size_t i = 0; i < tests.size(); i++)
			{
				auto [expectedOutput, myFormatStr, number] = tests[i];
				IO::printf(myFormatStr.c_str(), number);

				const char* res = compareMemoryPrintfOnly((const uint8_t*)expectedOutput.c_str(), expectedOutput.length());
				if (res)
				{
					return res;
				}
			}
		}

		// Test center-alignment for binary
		{
			using TupleType = std::tuple<std::string, std::string, uint32_t>;
			const std::vector<TupleType> tests = {
				TupleType{ "     0000'0111     \n", "{:^19b}\n", 7 },
				TupleType{ "      0000'0111     \n", "{:^20b}\n", 7 },
				TupleType{ "0000'1000'0000'0111\n", "{:^19b}\n", 2055 },
				TupleType{ "     0b0000'0111     \n", "{:^#21b}\n", 7 },
				TupleType{ "      0b0000'0111     \n", "{:^#22b}\n", 7 },
				TupleType{ "0b0000'1000'0000'0111\n", "{:^#21b}\n", 2055 },
			};

			for (size_t i = 0; i < tests.size(); i++)
			{
				auto [expectedOutput, myFormatStr, number] = tests[i];
				IO::printf(myFormatStr.c_str(), number);

				const char* res = compareMemoryPrintfOnly((const uint8_t*)expectedOutput.c_str(), expectedOutput.length());
				if (res)
				{
					return res;
				}
			}
		}

		END_TEST;
	}

	DEFINE_TEST(signedOutputIsSameAsPrintf)
	{
		// Signed output shouldn't effect hex ints
		{
			using TupleType = std::tuple<std::string, std::string, std::string, uint32_t>;
			const std::vector<TupleType> tests = {
				TupleType{ "  0XABCD\n", "%+#8X\n", "{:>+#8X}\n", 0xABCD },
				TupleType{ "ABCD\n", "%+3X\n", "{:>+3X}\n", 0xABCD },
				TupleType{ "0X00ABCD\n", "%+#08X\n", "{:>+#08X}\n", 0xABCD },
			};

			for (size_t i = 0; i < tests.size(); i++)
			{
				auto [expectedOutput, cFormatStr, myFormatStr, number] = tests[i];
				printf(cFormatStr.c_str(), number);
				IO::printf(myFormatStr.c_str(), number);

				const char* res = compareMemory((const uint8_t*)expectedOutput.c_str(), expectedOutput.length());
				if (res)
				{
					return res;
				}
			}
		}

		// Signed/spaced output test for floats
		{
			using TupleType = std::tuple<std::string, std::string, std::string, float>;
			const std::vector<TupleType> tests = {
				TupleType{ "+1.123\n", "%+.3f\n", "{:+.3f}\n", 1.123f },
				TupleType{ "-1.123\n", "%+.3f\n", "{:+.3f}\n", -1.123f },
				TupleType{ " 1.123\n", "% .3f\n", "{: .3f}\n", 1.123f },
				TupleType{ "-1.123\n", "% .3f\n", "{: .3f}\n", -1.123f },
				TupleType{ "1.123\n", "%.3f\n", "{:-.3f}\n", 1.123f },
				TupleType{ "-1.123\n", "%.3f\n", "{:-.3f}\n", -1.123f },
			};

			for (size_t i = 0; i < tests.size(); i++)
			{
				auto [expectedOutput, cFormatStr, myFormatStr, number] = tests[i];
				printf(cFormatStr.c_str(), number);
				IO::printf(myFormatStr.c_str(), number);

				const char* res = compareMemory((const uint8_t*)expectedOutput.c_str(), expectedOutput.length());
				if (res)
				{
					return res;
				}
			}
		}

		// Signed/spaced output should do nothing for binary
		{
			using TupleType = std::tuple<std::string, std::string, uint32_t>;
			const std::vector<TupleType> tests = {
				TupleType{ "          0000'0111\n", "{:>+19b}\n", 7 },
				TupleType{ "0000'1000'0000'0111\n", "{:>+19b}\n", 2055 },
				TupleType{ "          0b0000'0111\n", "{:>-#21b}\n", 7 },
				TupleType{ "0b0000'1000'0000'0111\n", "{:>-#21b}\n", 2055 },
				TupleType{ "          0b0000'0111\n", "{:> #21b}\n", 7 },
				TupleType{ "0b0000'1000'0000'0111\n", "{:> #21b}\n", 2055 }
			};

			for (size_t i = 0; i < tests.size(); i++)
			{
				auto [expectedOutput, myFormatStr, number] = tests[i];
				IO::printf(myFormatStr.c_str(), number);

				const char* res = compareMemoryPrintfOnly((const uint8_t*)expectedOutput.c_str(), expectedOutput.length());
				if (res)
				{
					return res;
				}
			}
		}

		// Signed/spaced output test for decimal integers
		{
			using TupleType = std::tuple<std::string, std::string, std::string, int32_t>;
			const std::vector<TupleType> tests = {
				TupleType{ "+7\n", "%+d\n", "{:+d}\n", 7 },
				TupleType{ "-7\n", "%+d\n", "{:+d}\n", -7 },
				TupleType{ " 232\n", "% d\n", "{: d}\n", 232 },
				TupleType{ "-232\n", "% d\n", "{: d}\n", -232 },
				TupleType{ "1299\n", "%d\n", "{:-d}\n", 1299 },
				TupleType{ "-1299\n", "%d\n", "{:-d}\n", -1299 },
			};

			for (size_t i = 0; i < tests.size(); i++)
			{
				auto [expectedOutput, cFormatStr, myFormatStr, number] = tests[i];
				printf(cFormatStr.c_str(), number);
				IO::printf(myFormatStr.c_str(), number);

				const char* res = compareMemory((const uint8_t*)expectedOutput.c_str(), expectedOutput.length());
				if (res)
				{
					return res;
				}
			}
		}
		END_TEST;
	}

	DEFINE_TEST(utf8FillCharacterFillsProperly)
	{
		// Different sized UTF8 characters shouldn't effect the fill count
		using TupleType = std::tuple<std::string, std::string, std::string>;
		const std::vector<TupleType> tests = {
			// 1-byte UTF8 codepoint
			TupleType{ "$$Hi!$$\n", "{$:^7}\n", "Hi!"},
			// 2-byte UTF8 codepoint
			TupleType{ u8"\u00A7\u00A7Hi!\u00A7\u00A7\n", u8"{\u00A7:^7}\n", "Hi!"},
			// 3-byte UTF8 codepoint
			TupleType{ u8"\u09A8\u09A8Hi!\u09A8\u09A8\n", u8"{\u09A8:^7}\n", "Hi!"},
			// 4-byte UTF8 codepoint
			TupleType{ u8"\U0001D122\U0001D122Hi!\U0001D122\U0001D122\n", u8"{\U0001D122:^7}\n", "Hi!"},
		};

		for (size_t i = 0; i < tests.size(); i++)
		{
			auto [expectedOutput, myFormatStr, str] = tests[i];
			IO::printf(myFormatStr.c_str(), str);

			const char* res = compareMemoryPrintfOnly((const uint8_t*)expectedOutput.c_str(), expectedOutput.length());
			if (res)
			{
				return res;
			}
		}

		END_TEST;
	}

	void setupPrintTestSuite()
	{
		Tests::TestSuite& testSuite = Tests::addTestSuite("cppPrint.hpp");

		ADD_BEFORE_EACH(testSuite, setupTest);
		ADD_AFTER_EACH(testSuite, teardownTest);

		ADD_TEST(testSuite, hexOutputIsSameAsPrintf);
		ADD_TEST(testSuite, floatOutputIsSameAsPrintf);
		ADD_TEST(testSuite, exponentialFloatOutputIsSameAsPrintf);
		ADD_TEST(testSuite, binaryOutputIsCorrect);
		ADD_TEST(testSuite, leftAlignIsCorrect);
		ADD_TEST(testSuite, rightAlignIsCorrect);
		ADD_TEST(testSuite, centerAlignIsCorrect);
		ADD_TEST(testSuite, signedOutputIsSameAsPrintf);
		ADD_TEST(testSuite, utf8FillCharacterFillsProperly);
	}

}

// -------------------- Thread Pool Test Suite --------------------
namespace ThreadPoolTestSuite
{

	DEFINE_TEST(dummy)
	{
		ASSERT_TRUE(false);
		END_TEST;
	}

	void setupThreadPoolTestSuite()
	{
		Tests::TestSuite& testSuite = Tests::addTestSuite("cppThreadPool.hpp");

		ADD_TEST(testSuite, dummy);
	}

}

// -------------------- Utils Test Suite --------------------
namespace CppUtilsTestSuite
{

	DEFINE_BEFORE_EACH(beforeEach)
	{
		g_logger_info("Before each.");
		END_BEFORE_EACH;
	}

	DEFINE_AFTER_EACH(afterEach)
	{
		g_logger_info("After each.");
		END_AFTER_EACH;
	}

	DEFINE_BEFORE_ALL(beforeAll)
	{
		g_logger_info("Before all");
		END_BEFORE_ALL;
	}

	DEFINE_AFTER_ALL(afterAll)
	{
		g_logger_info("After all");
		END_BEFORE_ALL;
	}

	DEFINE_TEST(dummy)
	{
		g_logger_info("Dummy 1");
		ASSERT_TRUE(false);
		END_TEST;
	}

	DEFINE_TEST(dummy2)
	{
		g_logger_info("Dummy 2");
		ASSERT_TRUE(true);
		END_TEST;
	}

	void setupCppUtilsTestSuite()
	{
		Tests::TestSuite& testSuite = Tests::addTestSuite("cppUtils.hpp");

		ADD_BEFORE_EACH(testSuite, beforeEach);
		ADD_AFTER_EACH(testSuite, afterEach);

		ADD_BEFORE_ALL(testSuite, beforeAll);
		ADD_AFTER_ALL(testSuite, afterAll);

		ADD_TEST(testSuite, dummy);
		ADD_TEST(testSuite, dummy2);
	}

}

using namespace StringTestSuite;
using namespace MaybeTestSuite;
using namespace PrintTestSuite;
using namespace ThreadPoolTestSuite;
using namespace CppUtilsTestSuite;

#include <vector>
#include <string>

struct Foo
{
	Foo(const std::string& str)
		: str(str)
	{
	}

	~Foo()
	{
		g_logger_info("Foo destructor called");
	}

	std::string str;
};

struct ComplexStruct
{
	std::vector<int> blahs;
	Foo foo = Foo("Hello");
};

// I'm purposely leaking memory and don't want to be warned to see if my
// library catches it so we disable warnings about unreferenced vars
#pragma warning( push )
#pragma warning( disable : 4189)
void mainFunc()
{
	g_logger_init();
	g_logger_set_level(g_logger_level_All);
	g_logger_set_log_directory("./logs");
	g_memory_init_padding_zeroed(true, 1024, true);

	constexpr bool runTests = true;
	if (runTests)
	{
		setupCppStringsTestSuite();
		//setupMaybeTestSuite();
		//setupPrintTestSuite();
		//setupThreadPoolTestSuite();
		//setupCppUtilsTestSuite();

		Tests::runTests();
		Tests::free();
	}

	IO::setBackgroundColor(ConsoleColor::BLACK);

	constexpr bool testingCppUtils = false;
	constexpr bool testingCppPrint = false;

	if (testingCppUtils)
	{
		g_logger_info("Some information.");
		g_logger_warning("A warning!");
		g_logger_error("This is an error...");

		//std::array<std::thread, 5> threads;
		//std::array<const char*, threads.size()> threadNames;
		//for (size_t i = 0; i < threads.size(); i++)
		//{
		//	std::string str = std::string("Thread_") + std::to_string(i);
		//	threadNames[i] = (char*)g_memory_allocate(sizeof(char) * (str.length() + 1));
		//	g_memory_copyMem((void*)threadNames[i], (void*)str.c_str(), sizeof(char) * (str.length() + 1));
		//	threads[i] = std::thread(threadLogger, threadNames[i]);
		//}

		//for (size_t i = 0; i < threads.size(); i++)
		//{
		//	threads[i].join();
		//	g_memory_free((void*)threadNames[i]);
		//}

		// Untracked memory allocation, we should be warned.
		void* leakedMemory = g_memory_allocate(sizeof(uint8) * 1025);

		void* someMemory = g_memory_allocate(sizeof(uint8) * 1024);
		g_memory_free(someMemory);

		uint8* memoryCorruptionBufferUnderrun = (uint8*)g_memory_allocate(sizeof(uint8) * 357);
		memoryCorruptionBufferUnderrun[-506] = 'h';
		g_memory_free(memoryCorruptionBufferUnderrun);

		uint8* memoryCorruptionBufferOverrun = (uint8*)g_memory_allocate(sizeof(uint8) * 312);
		memoryCorruptionBufferOverrun[312 + 809] = 'e';
		memoryCorruptionBufferOverrun = (uint8*)g_memory_realloc(memoryCorruptionBufferOverrun, sizeof(uint8) * 543);
		g_memory_free(memoryCorruptionBufferOverrun);

		uint8* reallocWithNullShouldAlloc = (uint8*)g_memory_realloc(NULL, sizeof(uint8) * 50);
		g_memory_free(reallocWithNullShouldAlloc);

		uint8* leakReallocWithNull = (uint8*)g_memory_realloc(NULL, sizeof(uint8) * 27);

		uint8* doubleReallocShouldSucceed = (uint8*)g_memory_allocate(sizeof(uint8) * 24);
		doubleReallocShouldSucceed = (uint8*)g_memory_realloc(doubleReallocShouldSucceed, sizeof(uint8) * 5048);
		doubleReallocShouldSucceed = (uint8*)g_memory_realloc(doubleReallocShouldSucceed, sizeof(uint8) * 5048 * 2);
		g_memory_free(doubleReallocShouldSucceed);

		// Test new and delete operators
		ComplexStruct* newMemoryLeak = g_memory_new ComplexStruct();
		g_logger_info("Foo name: '{}'", newMemoryLeak->foo.str);

		ComplexStruct* newShouldBeFine = g_memory_new ComplexStruct();
		g_memory_delete(newShouldBeFine);

		ComplexStruct* nullFooShouldBeFine = nullptr;
		g_memory_delete(nullFooShouldBeFine);

		g_memory_free(nullptr);
	}

	if (testingCppPrint)
	{
		g_logger_info("Info");
		g_logger_warning("Info");
		g_logger_error("Info");
		g_logger_assert(true, "Info");

		g_logger_info("{}", "Hello World!");

		g_logger_info("Pi: {}", 3.14f);
		g_logger_warning("Warning: {}", "Raw string literal");
		g_logger_error("Error: {}", std::string("C++ string object"));
		g_logger_info("This is a few integers {}, {}, {}, {}, {}, {}", 123, -342342, 0, INT32_MAX, (uint64)INT32_MAX + 1, -7);
		g_logger_info("Testing all sized integer types:\n"
			"  int8_t: {}\n"
			" int16_t: {}\n"
			" int32_t: {}\n"
			" int64_t: {}\n"
			" uint8_t: {}\n"
			"uint16_t: {}\n"
			"uint32_t: {}\n"
			"uint64_t: {}",
			(int8)INT8_MIN,
			(int16)INT16_MIN,
			(int32)INT32_MIN,
			(int64)INT64_MIN,
			(uint8)UINT8_MAX,
			(uint16)UINT16_MAX,
			(uint32)UINT32_MAX,
			(uint64)UINT64_MAX
		);

		g_logger_info("Testing some floats:\n"
			u8"   INFINITE: {\u00b7:13}\n"
			u8"  -INFINITE: {\u00b7:13}\n"
			u8"        NAN: {\u00b7:13}\n"
			u8"       0.32: {\u00b7:13.9f}\n"
			u8" 1.2222239f: {\u00b7:13}\n"
			u8"   1.999999: {\u00b7:13.3f}\n"
			u8"        2.0: {\u00b7:13}\n"
			u8"    2.00001: {\u00b7:13}\n"
			u8"        0.0: {\u00b7:13}\n"
			u8"    1.25e10: {\u00b7:13}\n"
			u8"0.999999999: {\u00b7:13}\n"
			u8"   1.25e-10: {\u00b7:13}",
			(float)INFINITY,
			-1.0f * (float)INFINITY,
			NAN,
			0.32f,
			1.2222239f,
			1.999999f,
			2.0f,
			2.00001f,
			0.0f,
			1.25e10f,
			0.999999999f,
			1.25e-10
		);

		g_logger_info("{}", 1.2222239f);

		g_logger_log("Here's some hex digits:\n"
			"    255: {0:10x}\n"
			"   -128: {0:#x}\n"
			"1194684: {:#X}\n"
			"    123: {:#x}",
			255,
			-128,
			1194684,
			(uint16)123);

		g_logger_info("Here's some binary digits:\n"
			"    255: {0:#b}\n"
			"   -128: {:#B}\n"
			"1194684: {:#B}\n"
			"    123: {:>#b}",
			255,
			-128,
			1194684,
			123);

		g_logger_info("\n"
			u8"\u2554{\u2550:^20}\u2557\n"
			u8"\u2551{ :^20}\u2551\n"
			u8"\u255a{\u2550:^20}\u255d",
			"",
			u8"Hello World!",
			""
		);

		g_logger_info(u8"∏ Test some unicode strings ∏\n"
			u8"     checkmark: \u2713 \n"
			u8"        x-mark: \u2a2f \n"
			u8"       o-slash: \u00D8 \n"
			u8"      small-xi: \u03be \n"
			u8"     small-phi: \u03C6 \n"
			u8"special-quotes: \u201C\u201D\u201E\u201F\u2018\u2019\u201A\u201B \n"
			u8"        arrows: \u2192 \u2190 \u2191 \u2193 \u2194 \u2195 \n"
			u8"        emojis: \U0001F600 \U0001F680 \U0001F4A9");

		// Trying all the colors
		for (int i = (int)ConsoleColor::BLACK; i < (int)ConsoleColor::WHITE; i++)
		{
			if (i <= (int)ConsoleColor::DARKGRAY)
			{
				IO::printf("{:>2}: ", i);
				IO::setColor((ConsoleColor)i, (ConsoleColor)(i + 8));
				IO::printf("I am the color {}", (ConsoleColor)i);
				IO::resetColor();
				IO::printf("\n");
			}

			IO::printf("{:>2}: ", i);
			IO::setForegroundColor((ConsoleColor)i);
			IO::printf("I am the color {}", (ConsoleColor)i);
			IO::resetColor();
			IO::printf("\n");
		}

		g_logger_info("A char: {}", (char)'a');
		g_logger_info("An unsigned and signed long: {} {}", (unsigned long)20, (signed long)-20);
		int aVar = 5;
		g_logger_info(
			"\n A pointer: {}"
			"\nReal value: {:#018X}",
			&aVar,
			(uint64_t)(void*)(&aVar));
	}

	g_memory_dumpMemoryLeaks();

	g_logger_assert(true, "We shouldn't see this.");

	g_memory_deinit();
	g_logger_free();
}

int main()
{
	try
	{
		mainFunc();
	}
	catch (std::exception ex)
	{
		const char* er = ex.what();
		printf("\n\nFailed with exception: %s\n", er);
	}

	return 0;
}
#pragma warning( pop ) 

#endif // GABE_CPP_UTILS_TEST_MAIN