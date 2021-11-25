// ===================================================================================
// Headers
// ===================================================================================
#ifndef GABE_CPP_UTILS_H
#define GABE_CPP_UTILS_H
#include <cstdint>
#include <cstddef>

// ----------------------------------
// Memory Utils
// ----------------------------------
typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

#define g_memory_allocate(numBytes) _g_memory_allocate(__FILE__, __LINE__, numBytes)
#define g_memory_realloc(memory, newSize) _g_memory_realloc(__FILE__, __LINE__, memory, newSize)
#define g_memory_free(memory) _g_memory_free(__FILE__, __LINE__, memory)

void* _g_memory_allocate(const char* filename, int line, size_t numBytes);
void* _g_memory_realloc(const char* filename, int line, void* memory, size_t newSize);
void _g_memory_free(const char* filename, int line, void* memory);

void g_memory_init(bool detectMemoryLeaks);
void g_memory_dumpMemoryLeaks();

int g_memory_compareMem(void* a, void* b, size_t numBytes);
void g_memory_zeroMem(void* memory, size_t numBytes);
void g_memory_copyMem(void* dst, void* src, size_t numBytes);

// ----------------------------------
// Logging Utils
// ----------------------------------
enum g_logger_level
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
#define g_logger_log(format, ...) _g_logger_log(__FILE__, __LINE__, format,##__VA_ARGS__)
#define g_logger_info(format, ...) _g_logger_info(__FILE__, __LINE__, format,##__VA_ARGS__)
#define g_logger_warning(format, ...) _g_logger_warning(__FILE__, __LINE__, format,##__VA_ARGS__)
#define g_logger_error(format, ...) _g_logger_error(__FILE__, __LINE__, format,##__VA_ARGS__)
#define g_logger_assert(condition, format, ...) _g_logger_assert(__FILE__, __LINE__, condition, format,##__VA_ARGS__)
#endif


void _g_logger_log(const char* filename, int line, const char* format, ...);
void _g_logger_info(const char* filename, int line, const char* format, ...);
void _g_logger_warning(const char* filename, int line, const char* format, ...);
void _g_logger_error(const char* filename, int line, const char* format, ...);
void _g_logger_assert(const char* filename, int line, int condition, const char* format, ...);

void g_logger_set_level(g_logger_level level);
g_logger_level g_logger_get_level();

#endif // GABE_CPP_UTILS_H





// ===================================================================================
// Implementation
// ===================================================================================
#ifdef GABE_CPP_UTILS_IMPL
#include <memory>
#include <vector>
#include <mutex>
#include <stdio.h>
#ifndef __linux__
//#include <varargs.h>
#endif
#include <stdarg.h>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cstring>

#ifdef _WIN32
#include <Windows.h>
#include <crtdbg.h>
#endif

// ----------------------------------
// Memory Implementation
// ----------------------------------
struct DebugMemoryAllocation
{
	const char* fileAllocator;
	int fileAllocatorLine;
	int references;
	void* memory;

	bool operator==(const DebugMemoryAllocation& other) const
	{
		return other.memory == this->memory;
	}
};

static std::mutex memoryMtx;
static std::vector<DebugMemoryAllocation> allocations;
static bool trackMemoryAllocations = false;

void g_memory_init(bool detectMemoryLeaks)
{
	trackMemoryAllocations = detectMemoryLeaks;
}

void* _g_memory_allocate(const char* filename, int line, size_t numBytes)
{
	void* memory = std::malloc(numBytes);
	if (trackMemoryAllocations)
	{
		std::lock_guard<std::mutex> lock(memoryMtx);
		// If we are in a debug build, track all memory allocations to see if we free them all as well
		auto iterator = std::find(allocations.begin(), allocations.end(), DebugMemoryAllocation{ filename, line, 0, memory });
		if (iterator == allocations.end())
		{
			allocations.emplace_back(DebugMemoryAllocation{ filename, line, 1, memory });
		}
		else
		{
			if (iterator->references <= 0)
			{
				iterator->references++;
				iterator->fileAllocator = filename;
				iterator->fileAllocatorLine = line;
				iterator->memory = memory;
			}
			else
			{
				g_logger_error("Tried to allocate memory that has already been allocated... This should never be hit. If it is, we have a problem.");
			}
		}
	}
	return memory;
}

void* _g_memory_realloc(const char* filename, int line, void* oldMemory, size_t numBytes)
{
	void* newMemory = std::realloc(oldMemory, numBytes);
	if (trackMemoryAllocations)
	{
		std::lock_guard<std::mutex> lock(memoryMtx);
		// If we are in a debug build, track all memory allocations to see if we free them all as well
		auto newMemoryIter = std::find(allocations.begin(), allocations.end(), DebugMemoryAllocation{ filename, line, 0, newMemory });
		auto oldMemoryIter = std::find(allocations.begin(), allocations.end(), DebugMemoryAllocation{ filename, line, 0, oldMemory });
		if (newMemoryIter != oldMemoryIter)
		{
			// Realloc could not expand the current pointer, so it allocated a new memory block
			if (oldMemoryIter == allocations.end())
			{
				g_logger_error("Tried to realloc invalid memory in '%s' line: %d.", filename, line);
			}
			else
			{
				oldMemoryIter->references--;
			}

			if (newMemoryIter == allocations.end())
			{
				allocations.emplace_back(DebugMemoryAllocation{ filename, line, 1, newMemory });
			}
			else
			{
				if (newMemoryIter->references <= 0)
				{
					newMemoryIter->references++;
					newMemoryIter->fileAllocator = filename;
					newMemoryIter->fileAllocatorLine = line;
					newMemoryIter->memory = newMemory;
				}
				else
				{
					g_logger_error("Tried to allocate memory that has already been allocated... This should never be hit. If it is, we have a problem.");
				}
			}
		}
		// If realloc expanded the memory in-place, then we don't need to do anything because no "new" memory locations were allocated
	}
	return newMemory;
}

void _g_memory_free(const char* filename, int line, void* memory)
{
	if (trackMemoryAllocations)
	{
		std::lock_guard<std::mutex> lock(memoryMtx);
		auto iterator = std::find(allocations.begin(), allocations.end(), DebugMemoryAllocation{ filename, line, 0, memory });
		if (iterator == allocations.end())
		{
			g_logger_error("Tried to free invalid memory that was never allocated at '%s' line: %d", filename, line);
		}
		else if (iterator->references <= 0)
		{
			g_logger_error("Tried to free memory that has already been freed.");
			g_logger_error("Code that attempted to free: '%s' line: %d", filename, line);
			g_logger_error("Code that allocated the memory block: '%s' line: %d", iterator->fileAllocator, iterator->fileAllocatorLine);
		}
		else
		{
			iterator->references--;
		}
	}

	// When debug is turned off we literally just free the memory, so it will throw a segfault if a
	// faulty release build was published
	std::free(memory);
}

void g_memory_dumpMemoryLeaks()
{
	std::lock_guard<std::mutex> lock(memoryMtx);
	for (const auto& alloc : allocations)
	{
		if (alloc.references > 0)
		{
			g_logger_warning("Memory leak detected. Allocated from: '%s' line: %d", alloc.fileAllocator, alloc.fileAllocatorLine);
		}
	}
}

int g_memory_compareMem(void* a, void* b, size_t numBytes)
{
	return std::memcmp(a, b, numBytes);
}

void g_memory_zeroMem(void* memory, size_t numBytes)
{
	std::memset(memory, 0, numBytes);
}

void g_memory_copyMem(void* dst, void* src, size_t numBytes)
{
	std::memcpy(dst, src, numBytes);
}


// ----------------------------------
// Logging Implementation
// ----------------------------------
static std::mutex logMutex;

static g_logger_level log_level = g_logger_level::All;

void g_logger_set_level(g_logger_level level)
{
	log_level = level;
}

g_logger_level g_logger_get_level()
{
	return log_level;
}

#ifdef _WIN32

void _g_logger_log(const char* filename, int line, const char* format, ...)
{
	if (g_logger_get_level() <= g_logger_level::Log)
	{
		std::lock_guard<std::mutex> lock(logMutex);
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
		std::lock_guard<std::mutex> lock(logMutex);
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
		std::lock_guard<std::mutex> lock(logMutex);
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
		std::lock_guard<std::mutex> lock(logMutex);
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
			std::lock_guard<std::mutex> lock(logMutex);
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

#elif defined(__linux__) // end LOGGING_IMPL_WIN32
// begin LOGGING_IMPL_LINUX

#include <csignal>

namespace ColorCode
{
	const char* KNRM = "\x1B[0m";
	const char* KRED = "\x1B[31m";
	const char* KGRN = "\x1B[32m";
	const char* KYEL = "\x1B[33m";
	const char* KBLU = "\x1B[34m";
	const char* KMAG = "\x1B[35m";
	const char* KCYN = "\x1B[36m";
	const char* KWHT = "\x1B[37m";
}

void _g_logger_log(const char* filename, int line, const char* format, ...)
{
	if (g_logger_get_level() <= g_logger_level::Log)
	{
		std::lock_guard<std::mutex> lock(logMutex);
		printf("%s%s (line %d) Log: \n", ColorCode::KBLU, filename, line);

		std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		char buf[20] = { 0 };
		std::strftime(buf, sizeof(buf), "%Y-%m-%d %I:%M:%S", std::localtime(&now));
		printf("%s[%s]: ", ColorCode::KNRM, buf);

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
		printf("%s%s (line %d) Info: \n", ColorCode::KGRN, filename, line);

		std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		char buf[20] = { 0 };
		std::strftime(buf, sizeof(buf), "%Y-%m-%d %I:%M:%S", std::localtime(&now));
		printf("%s[%s]: ", ColorCode::KNRM, buf);

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
		printf("%s%s (line %d) Warning: \n", ColorCode::KYEL, filename, line);

		std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		char buf[20] = { 0 };
		std::strftime(buf, sizeof(buf), "%Y-%m-%d %I:%M:%S", std::localtime(&now));
		printf("%s[%s]: ", ColorCode::KNRM, buf);

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
		printf("%s%s (line %d) Error: \n", ColorCode::KRED, filename, line);

		std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		char buf[20] = { 0 };
		std::strftime(buf, sizeof(buf), "%Y-%m-%d %I:%M:%S", std::localtime(&now));
		printf("%s[%s]: ", ColorCode::KNRM, buf);

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
			printf("%s%s (line %d) Assertion Failure: \n", ColorCode::KRED, filename, line);

			std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			char buf[20] = { 0 };
			std::strftime(buf, sizeof(buf), "%Y-%m-%d %I:%M:%S", std::localtime(&now));
			printf("%s[%s]: ", ColorCode::KNRM, buf);

			va_list args;
			va_start(args, format);
			vprintf(format, args);
			va_end(args);

			printf("\n");
			std::raise(SIGINT);
			exit(-1);
		}
	}
}

#else // end LOGGING_IMPL_LINUX
// Begin DEFAULT_LOGGING_IMPL

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
#endif // DEFAULT_LOGGING_IMPL
#endif // CPP_UTILS_IMPL
