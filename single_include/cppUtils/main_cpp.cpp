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

std::ostream& operator<<(std::ostream& io, const Vec2& vec)
{
	io << "{ " << vec.x << ", " << vec.y << " }";
	return io;
}

// I'm purposely leaking memory and don't want to be warned to see if my
// library catches it so we disable warnings about unreferenced vars
#pragma warning( push )
#pragma warning( disable : 4189)
int main()
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

		g_logger_info("Vec2: {}", vec2);
	}

	g_memory_dumpMemoryLeaks();

	g_logger_assert(true, "We shouldn't see this.");

	g_memory_deinit();
	g_logger_free();

	return 0;
}
#pragma warning( pop ) 

#endif // GABE_CPP_UTILS_TEST_MAIN