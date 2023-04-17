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

#include <cppUtils/cppUtils.hpp>

#ifdef GABE_CPP_UTILS_IMPL
#undef GABE_CPP_UTILS_IMPL
#endif

#define GABE_CPP_STRINGS_IMPL
#include <cppUtils/cppStrings.hpp>

#include <array>
#include <thread>
#include <string>
#include <iostream>

void threadLogger(const char* threadName)
{
	for (int i = 0; i < 10; i++)
	{
		//g_logger_info("%s[i]: %d", threadName, i);
		g_logger_info("{}[i]: {}", threadName, i);
	}
}

struct Vec2
{
	float x;
	float y;
};

struct UnknownStruct
{
	float foo;
	int bar;
	const char* string;
};

g_io_stream& operator<<(g_io_stream& io, const UnknownStruct& foo)
{
	io << "<Foo: " << foo.foo << ", bar: " << foo.bar << ", string: " << foo.string << ">";
	return io;
}

g_io_stream& operator<<(g_io_stream& io, const Vec2& vec)
{
	io << "<X:" << vec.x << ", Y:" << vec.y << ">";
	return io;
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

	constexpr bool testingCppUtils = false;
	constexpr bool testingCppPrint = true;

	if (testingCppUtils)
	{
		g_logger_info("Some information.");
		g_logger_warning("A warning!");
		g_logger_error("This is an error...");

		std::array<std::thread, 5> threads;
		std::array<const char*, threads.size()> threadNames;
		for (size_t i = 0; i < threads.size(); i++)
		{
			std::string str = std::string("Thread_") + std::to_string(i);
			threadNames[i] = (char*)g_memory_allocate(sizeof(char) * (str.length() + 1));
			g_memory_copyMem((void*)threadNames[i], (void*)str.c_str(), sizeof(char) * (str.length() + 1));
			threads[i] = std::thread(threadLogger, threadNames[i]);
		}

		for (size_t i = 0; i < threads.size(); i++)
		{
			threads[i].join();
			g_memory_free((void*)threadNames[i]);
		}

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

		g_DumbString string = g_dumbString_new("Hello World!");
		g_logger_info("{}, {}", string, 2.3f);
		g_dumbString_free(string);

		Vec2 vec2 = Vec2{ 0.3f, 2.1f };

		g_logger_log("Hello World! My Vec2: {}", vec2);
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

		g_logger_info("Vec2: {{ hello {{ there {{ {:.2f} continue on printing", vec2);

		UnknownStruct unknown = {
			2.75f,
			-280,
			"Hello sailor!"
		};
		g_logger_info("Hey this is unknown: {:.2f}", unknown);

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
			"+{-:^20}+\n"
			"|{ :^20}|\n"
			"+{-:^20}+",
			"",
			"Hello World!",
			""
		);
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