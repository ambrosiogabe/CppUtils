// ===================================================================================
// Test main
// This is just a testing workspace for me while developing this
// do not define GABE_CPP_UTILS_TEST_MAIN unless you are developing 
// this single_include file.
// ===================================================================================
#ifdef GABE_CPP_UTILS_TEST_MAIN
#include <cppUtils/cppUtils.hpp>

int main()
{
	g_logger_set_level(g_logger_level::All);
    g_logger_log("Some very basic information.");
	g_logger_info("Some information.");
	g_logger_warning("A warning!");
	g_logger_error("This is an error...");

	g_memory_init(true);

	// Untracked memory allocation, we should be warned.
	g_memory_allocate(sizeof(char) * 1024);

	void* someMemory = g_memory_allocate(sizeof(char) * 1024);
	g_memory_free(someMemory);

	g_memory_dumpMemoryLeaks();

	g_logger_assert(true, "We shouldn't see this.");
	g_logger_assert(false, "Bad assertion, should fail and break the program.");
    g_logger_info("You shouldn't see me.");

    return 0;
}

#endif // GABE_CPP_UTILS_TEST_MAIN