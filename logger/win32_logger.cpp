#ifdef GABE_LOGGER_IMPL
#ifdef _WIN32
#include <stdio.h>
#include <varargs.h>
#include <stdarg.h>
#include <stdlib.h>
#include <chrono>

#include <Windows.h>
#include <crtdbg.h>

#include "logger.h"

void _g_logger_log(const char* filename, int line, const char* format, ...)
{
	if (g_logger_get_level() <= g_logger_level::Log)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN);
		printf("%s (line %d) Log: \n", filename, line);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0F);
		
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
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
		printf("%s (line %d) Info: \n", filename, line);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0F);
		
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
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_RED);
		printf("%s (line %d) Warning: \n", filename, line);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0F);

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
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		printf("%s (line %d) Error: \n", filename, line);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0F);
		
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
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
			printf("%s (line %d) Assertion Failure: \n", filename, line);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0F);
			
			std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			char buf[20] = { 0 };
			std::strftime(buf, sizeof(buf), "%Y-%m-%d %I:%M:%S", std::localtime(&now));
			printf("[%s]: ", buf);

			va_list args;
			va_start(args, format);
			vprintf(format, args);
			va_end(args);

			printf("\n");
			_CrtDbgBreak();
			exit(-1);
		}
	}
}
#endif
#endif