// ===================================================================================
// Test main
// This is just a testing workspace for me while developing this
// do not define GABE_CPP_UTILS_TEST_MAIN unless you are developing 
// this single_include file.
// ===================================================================================
#ifdef GABE_CPP_UTILS_TEST_MAIN
#include <cppUtils/cppUtils.hpp>

void main()
{
	g_logger_init();
	g_logger_set_level(g_logger_level::All);
	g_logger_set_log_directory("C:/dev/C++/CppUtils/logs");
	g_logger_info("Some information.");
	g_logger_warning("A warning!");
	g_logger_error("This is an error...");

	g_memory_init(true, 1024);

	// Untracked memory allocation, we should be warned.
	g_memory_allocate(sizeof(char) * 1025);

	void* someMemory = g_memory_allocate(sizeof(char) * 1024);
	g_memory_free(someMemory);

	uint8* memoryCorruptionBufferUnderrun = (uint8*)g_memory_allocate(sizeof(char) * 357);
	memoryCorruptionBufferUnderrun[-506] = 'h';
	g_memory_free(memoryCorruptionBufferUnderrun);

	uint8* memoryCorruptionBufferOverrun = (uint8*)g_memory_allocate(sizeof(char) * 312);
	memoryCorruptionBufferOverrun[312 + 809] = 'a';
	memoryCorruptionBufferOverrun = (uint8*)g_memory_realloc(memoryCorruptionBufferOverrun, sizeof(char) * 543);
	g_memory_free(memoryCorruptionBufferOverrun);

	g_memory_dumpMemoryLeaks();

	g_logger_assert(true, "We shouldn't see this.");
	g_logger_assert(false, "Bad assertion, should fail and break the program.");
}

#endif // GABE_CPP_UTILS_TEST_MAIN