#if defined(GABE_LOGGER_UNDEFINED_IMPL) || defined(GABE_LOGGER_IMPL)
#include "logger.h"

static g_logger_level log_level = g_logger_level::All;

void g_logger_set_level(g_logger_level level)
{
	log_level = level;
}

g_logger_level g_logger_get_level()
{
	return log_level;
}

#endif