#ifdef GABE_LOGGER_IMPL
#ifndef _WIN32
#include <stdio.h>
#include <varargs.h>
#include <stdarg.h>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <mutex>

#include "logger.h"

static std::mutex logMutex;

void _g_logger_log(const char* filename, int line, const char* format, ...)
{
	if (g_logger_get_level() <= g_logger_level::Log)
	{
		std::lock_guard<std::mutex> lock(logMutex);
		printf("%s (line %d) Log: \n", filename, line);
		
		std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		char buf[20] = { 0 };
		std::strftime(buf, sizeof(buf), "%Y-%m-%d %I:%M:%S", std::localtime(&now));
		printf("[%s]: ", buf);

		va_list args;
		va_start(args, format);
		vprintf(format, args);
		va_end(args);

		printf("\n");
	}
}

void _g_logger_info(const char* filename, int line, const char* format, ...)
{
	if (g_logger_get_level() <= g_logger_level::Info)
	{
		std::lock_guard<std::mutex> lock(logMutex);
		printf("%s (line %d) Info: \n", filename, line);
		
		std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		char buf[20] = { 0 };
		std::strftime(buf, sizeof(buf), "%Y-%m-%d %I:%M:%S", std::localtime(&now));
		printf("[%s]: ", buf);

		va_list args;
		va_start(args, format);
		vprintf(format, args);
		va_end(args);

		printf("\n");
	}
}

void _g_logger_warning(const char* filename, int line, const char* format, ...)
{
	if (g_logger_get_level() <= g_logger_level::Warning)
	{
		std::lock_guard<std::mutex> lock(logMutex);
		printf("%s (line %d) Warning: \n", filename, line);
		
		std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		char buf[20] = { 0 };
		std::strftime(buf, sizeof(buf), "%Y-%m-%d %I:%M:%S", std::localtime(&now));
		printf("[%s]: ", buf);

		va_list args;
		va_start(args, format);
		vprintf(format, args);
		va_end(args);

		printf("\n");
	}
}

void _g_logger_error(const char* filename, int line, const char* format, ...)
{
	if (g_logger_get_level() <= g_logger_level::Error)
	{
		std::lock_guard<std::mutex> lock(logMutex);
		printf("%s (line %d) Error: \n", filename, line);
		
		std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		char buf[20] = { 0 };
		std::strftime(buf, sizeof(buf), "%Y-%m-%d %I:%M:%S", std::localtime(&now));
		printf("[%s]: ", buf);

		va_list args;
		va_start(args, format);
		vprintf(format, args);
		va_end(args);

		printf("\n");
	}
}

void _g_logger_assert(const char* filename, int line, int condition, const char* format, ...)
{
	if (g_logger_get_level() <= g_logger_level::Assert)
	{
		if (!condition)
		{
			std::lock_guard<std::mutex> lock(logMutex);
			printf("%s (line %d) Assertion Failure: \n", filename, line);
			
			std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			char buf[20] = { 0 };
			std::strftime(buf, sizeof(buf), "%Y-%m-%d %I:%M:%S", std::localtime(&now));
			printf("[%s]: ", buf);

			va_list args;
			va_start(args, format);
			vprintf(format, args);
			va_end(args);

			printf("\n");
			exit(-1);
		}
	}
}
#endif
#endif