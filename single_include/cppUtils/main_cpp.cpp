// ===================================================================================
// Test main
// This is just a testing workspace for me while developing this
// do not define GABE_CPP_UTILS_TEST_MAIN unless you are developing 
// this single_include file.
// ===================================================================================
#ifdef GABE_CPP_UTILS_TEST_MAIN

#define GABE_CPP_UTILS_IMPL
#include <cppUtils/cppPrint.hpp>
#include <cppUtils/cppUtils.hpp>
#include <cppUtils/cppTests.hpp>
#include <cppUtils/cppStrings.hpp>

using namespace CppUtils;

#include <cppUtils/cppMaybe.hpp>

// -------------------- Tests --------------------
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

void setupCppStringsTestSuite()
{
	Tests::TestSuite& testSuite = Tests::addTestSuite("cppStrings.hpp");

	ADD_TEST(testSuite, utf8String_C080_ShouldBeBad);
	ADD_TEST(testSuite, utf8String_EDA18CEDBEB4_ShouldBeBad);
	ADD_TEST(testSuite, validUtf8String_ShouldSucceed);
	ADD_TEST(testSuite, validUtf8String_ShouldSucceedWithUnicodeChars);
}

// I'm purposely leaking memory and don't want to be warned to see if my
// library catches it so we disable warnings about unreferenced vars
#pragma warning( push )
#pragma warning( disable : 4189)
void mainFunc()
{
	g_logger_init();
	g_logger_set_level(g_logger_level_All);
	g_logger_set_log_directory("C:/dev/C++/CppUtils/logs");
	g_memory_init_padding(true, 1024);


	setupCppStringsTestSuite();

	Tests::runTests();
	Tests::free();

	IO::setBackgroundColor(ConsoleColor::BLACK);

	constexpr bool testingCppUtils = false;
	constexpr bool testingCppPrint = true;

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
		memoryCorruptionBufferOverrun[312 + 809] = 'a';
		memoryCorruptionBufferOverrun = (uint8*)g_memory_realloc(memoryCorruptionBufferOverrun, sizeof(uint8) * 543);
		g_memory_free(memoryCorruptionBufferOverrun);

		uint8* reallocWithNullShouldAlloc = (uint8*)g_memory_realloc(NULL, sizeof(uint8) * 50);
		g_memory_free(reallocWithNullShouldAlloc);

		uint8* leakReallocWithNull = (uint8*)g_memory_realloc(NULL, sizeof(uint8) * 27);

		uint8* doubleReallocShouldSucceed = (uint8*)g_memory_allocate(sizeof(uint8) * 24);
		doubleReallocShouldSucceed = (uint8*)g_memory_realloc(doubleReallocShouldSucceed, sizeof(uint8) * 5048);
		doubleReallocShouldSucceed = (uint8*)g_memory_realloc(doubleReallocShouldSucceed, sizeof(uint8) * 5048 * 2);
		g_memory_free(doubleReallocShouldSucceed);
	}

	if (testingCppPrint)
	{
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
			"   INFINITE: {*:13}\n"
			"  -INFINITE: {*:13}\n"
			"        NAN: {*:13}\n"
			"       0.32: {*:13.9f}\n"
			" 1.2222239f: {*:13}\n"
			"   1.999999: {*:13.3f}\n"
			"        2.0: {*:13}\n"
			"    2.00001: {*:13}\n"
			"        0.0: {*:13}\n"
			"    1.25e10: {*:13}\n"
			"0.999999999: {*:13}\n"
			"   1.25e-10: {*:13}",
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
			u8"        arrows: \u2192 \u2190 \u2191 \u2193 \u2194 \u2195");
	}

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
		printf("\n\nFailed with exception: %s\n", ex.what());
	}

	return 0;
}
#pragma warning( pop ) 

#endif // GABE_CPP_UTILS_TEST_MAIN