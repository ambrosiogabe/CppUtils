#ifndef GABE_LOGGER
#define GABE_LOGGER

const enum g_logger_level
{
	All = 0,
	Log = 1,
	Info = 2,
	Warning = 3,
	Error = 4,
	Assert = 5,
	None = 6,
};

#if !(defined(__GNUC__) || defined(__GNUG__))
#define g_logger_log(format, ...) _g_logger_log(__FILE__, __LINE__, format, __VA_ARGS__)
#define g_logger_info(format, ...) _g_logger_info(__FILE__, __LINE__, format, __VA_ARGS__)
#define g_logger_warning(format, ...) _g_logger_warning(__FILE__, __LINE__, format, __VA_ARGS__)
#define g_logger_error(format, ...) _g_logger_error(__FILE__, __LINE__, format, __VA_ARGS__)
#define g_logger_assert(condition, format, ...) _g_logger_assert(__FILE__, __LINE__, condition, format, __VA_ARGS__)
#else
#define Log(format, ZeroOrMoreArgs...) _Log(__FILE__, __LINE__, format, ZeroOrMoreArgs)
#define Info(format, ZeroOrMoreArgs...) _Info(__FILE__, __LINE__, format, ZeroOrMoreArgs)
#define Warning(format, ZeroOrMoreArgs...) _Warning(__FILE__, __LINE__, format, ZeroOrMoreArgs)
#define Error(format, ZeroOrMoreArgs...) _Error(__FILE__, __LINE__, format, ZeroOrMoreArgs)
#define Assert(condition, format, ...) _Assert(__FILE__, __LINE__, condition, format, ZeroOrMoreArgs)
#endif


void _g_logger_log(const char* filename, int line, const char* format, ...);
void _g_logger_info(const char* filename, int line, const char* format, ...);
void _g_logger_warning(const char* filename, int line, const char* format, ...);
void _g_logger_error(const char* filename, int line, const char* format, ...);
void _g_logger_assert(const char* filename, int line, int condition, const char* format, ...);

void g_logger_set_level(g_logger_level level);
g_logger_level g_logger_get_level();

#endif