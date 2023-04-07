// ===================================================================================
// Test main
// This is just a testing workspace for me while developing this
// do not define GABE_CPP_UTILS_TEST_MAIN unless you are developing 
// this single_include file.
// ===================================================================================
#ifdef GABE_CPP_UTILS_TEST_MAIN
#include <cppUtils/cppUtils.hpp>

// I'm purposely leaking memory and don't want to be warned to see if my
// library catches it so we disable warnings about unreferenced vars
#pragma warning( push )
#pragma warning( disable : 4189)
void main(void)
{
	g_logger_init();
	g_logger_set_level(g_logger_level_All);
	g_logger_set_log_directory("C:/dev/C++/CppUtils/logs");

	g_logger_info("Some information.");
	g_logger_warning("A warning!");
	g_logger_error("This is an error...");

	g_memory_init_padding(true, 1024);

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
	// This shouldn't cause a buffer overrun if we make it smaller
	reallocWithNullShouldAlloc[49] = 'a';
	reallocWithNullShouldAlloc = (uint8*)g_memory_realloc(reallocWithNullShouldAlloc, sizeof(uint8) * 30);
	g_memory_free(reallocWithNullShouldAlloc);

	uint8* leakReallocWithNull = (uint8*)g_memory_realloc(NULL, sizeof(uint8) * 27);

	int a = 10;
	int b = 10;
	g_logger_assert(g_memory_compareMem(&a, sizeof(int), &b, sizeof(int)) == TRUE, "Bad memcmp function.");

	g_memory_dumpMemoryLeaks();

	g_logger_assert(true, "We shouldn't see this.");
	g_logger_assert(false, "Bad assertion, should fail and break the program.");

	g_memory_deinit();
	g_logger_free();
}
#pragma warning( pop ) 

#endif // GABE_CPP_UTILS_TEST_MAIN