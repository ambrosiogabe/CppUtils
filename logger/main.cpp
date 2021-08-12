#ifndef SINGLE_INCLUDE_TEST
#include "logger.h"
#include "logger.h"

int main()
{
	g_logger_set_level(g_logger_level::All);

	g_logger_log("This is just a log.");
	g_logger_info("This is some info.");
	g_logger_assert(true, "This assertion should pass.");
	g_logger_warning("Oh no! This is a warning.");
	g_logger_error("An error has occured.");

	g_logger_assert(false, "And this one should fail.");

	return 0;
}
#else 
#define GABE_LOGGER_IMPL
#include "logger/logger.h"
#undef GABE_LOGGER_IMPL
#include "logger/logger.h"

int main()
{
	g_logger_set_level(g_logger_level::All);

	g_logger_log("This is just a log.");
	g_logger_info("This is some info.");
	g_logger_assert(true, "This assertion should pass.");
	g_logger_warning("Oh no! This is a warning.");
	g_logger_error("An error has occured.");

	g_logger_assert(false, "And this one should fail.");

	return 0;
}
#endif