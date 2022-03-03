/*
 Do this:
	 #define GABE_CPP_UTILS_IMPL
 before you include this file in *one* C++ file to create the implementation.

 // i.e. it should look like this in *one* source file:
 #define GABE_CPP_UTILS_IMPL
 #include "cppUtils.hpp"



 LICENSE

 Open Source, see end of file



 DOCUMENTATION

 This library is a single header library, inspired by the stb libraries. It
 includes several utilities to do simple things like logging and 
 memory tracking/heap corruption detection.Here is a list of the functions 
 and some simple documentation For DLL info, look at the end of the documentation.



 MEMORY TRACKER/HEAP CORRUPTION DETECTOR

 BASIC USAGE

 int main() 
 {
	g_memory_init(true, 12);
	// Run my app
	g_memory_dumpMemoryLeaks();
 }

 FUNCTIONS

 g_memory_init(bool detectMemoryLeaks, uint16 bufferPadding = 5);
  - detectMemoryLeaks   -- Set this to true to detect memory corruption/leaks. When this is set to
						   false, this is simply a wrapper around std::malloc, std::realloc, etc.
  - bufferPadding       -- This is how many extra bytes will be allocated at the start and end of
						   each memory allocation. I advise that you set this to a very low
						   number, like 1-2 bytes, if you plan on releasing your app with this turned on.

 g_memory_dumpMemoryLeaks();

 NOTE: Only memory allocated using this function will be tracked
	g_memory_allocate(size_t numBytes)

 NOTE: Call this on any memory allocated with the above function to properly track it
	g_memory_free(void* memory)

 NOTE: Only call this on memory that was allocated using the above function
	g_memory_realloc(void* memory, size_t newSize)

 Miscellaneous memory functions:
	g_memory_compareMem(void* a, void* b, size_t numBytes)
	g_memory_zeroMem(void* memory, size_t numBytes);
	g_memory_copyMem(void* dst, void* src, size_t numBytes);



 LOGGER

 BASIC USAGE

 All logging functions follow the format of printf, where you call a function
 like:

	 printf("Some string %s", "Some Format");

 by calling it with a string, followed by a list of formatting options.

 FUNCTIONS

 These are all the logging options listed from least severe, to most severe:

	g_logger_log(const char* message, ...format)
	g_logger_info(const char* message, ...format)
	g_logger_warning(const char* message, ...format)
	g_logger_error(const char* message, ...format)
	g_logger_assert(bool condition, const char* failureMessage, ...format)

 Use this to restrict the messages to a particular level. This will log anything
 at the preferred level or higher in severity:

	g_logger_set_level(g_logger_level level)


 DLL STUFF

 If you want to use this library as part of a DLL, I have created a macro:

	GABE_CPP_UTILS_API

 that does nothing, but prefaces each forward declaration. Simply redefine this
 to your preferred __declspec(dllimport) or __declspec(dllexport) before including this
 file anywhere and it should just work out.
*/

// ===================================================================================
// Headers
// ===================================================================================
#ifndef GABE_CPP_UTILS_H
#define GABE_CPP_UTILS_H

// Override this define to add DLL support on your platform
#ifndef GABE_CPP_UTILS_API 
#define GABE_CPP_UTILS_API 
#endif

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

GABE_CPP_UTILS_API void* _g_memory_allocate(const char* filename, int line, size_t numBytes);
GABE_CPP_UTILS_API void* _g_memory_realloc(const char* filename, int line, void* memory, size_t newSize);
GABE_CPP_UTILS_API void _g_memory_free(const char* filename, int line, void* memory);

GABE_CPP_UTILS_API void g_memory_init(bool detectMemoryLeaks, uint16 bufferPadding = 5);
GABE_CPP_UTILS_API void g_memory_dumpMemoryLeaks();

GABE_CPP_UTILS_API int g_memory_compareMem(void* a, void* b, size_t numBytes);
GABE_CPP_UTILS_API void g_memory_zeroMem(void* memory, size_t numBytes);
GABE_CPP_UTILS_API void g_memory_copyMem(void* dst, void* src, size_t numBytes);

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


GABE_CPP_UTILS_API void _g_logger_log(const char* filename, int line, const char* format, ...);
GABE_CPP_UTILS_API void _g_logger_info(const char* filename, int line, const char* format, ...);
GABE_CPP_UTILS_API void _g_logger_warning(const char* filename, int line, const char* format, ...);
GABE_CPP_UTILS_API void _g_logger_error(const char* filename, int line, const char* format, ...);
GABE_CPP_UTILS_API void _g_logger_assert(const char* filename, int line, int condition, const char* format, ...);

GABE_CPP_UTILS_API void g_logger_set_level(g_logger_level level);
GABE_CPP_UTILS_API g_logger_level g_logger_get_level();

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
#include <array>
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
	size_t memorySize;
	void* memory;

	bool operator==(const DebugMemoryAllocation& other) const
	{
		return other.memory == this->memory;
	}
};

static std::mutex memoryMtx;
static std::vector<DebugMemoryAllocation> allocations;
static bool trackMemoryAllocations = false;
static const std::array<uint8, 8> specialMemoryFlags = { (uint8)'h', (uint8)'e', (uint8)'Y', (uint8)'G' , (uint8)'a', (uint8)'b', (uint8)'e', (uint8)'!' };
static uint16 bufferPadding = 5;

void g_memory_init(bool detectMemoryErrors, uint16 inBufferPadding)
{
	trackMemoryAllocations = detectMemoryErrors;
	bufferPadding = inBufferPadding;
}

void* _g_memory_allocate(const char* filename, int line, size_t numBytes)
{
	if (trackMemoryAllocations)
	{
		// In memory error tracking mode, I'll add sentinel values to the beginning and
		// end of the block of memory to ensure it doesn't have any errors on free
		numBytes += (bufferPadding * 2) * sizeof(uint8);

		// In debug mode we allocate 10 extra bytes, 5 before the block and 5 after. We can use these to detect
		// Buffer overruns or underruns
		void* memory = std::malloc(numBytes);
		if (memory)
		{
			uint8* memoryBytes = (uint8*)memory;
			for (int i = 0; i < bufferPadding; i++)
			{
				memoryBytes[i] = specialMemoryFlags[i % specialMemoryFlags.size()];
			}

			memoryBytes = ((uint8*)memory) + numBytes - bufferPadding;
			for (int i = 0; i < bufferPadding; i++)
			{
				memoryBytes[i] = specialMemoryFlags[i % specialMemoryFlags.size()];
			}
		}

		std::lock_guard<std::mutex> lock(memoryMtx);
		// If we are in a debug build, track all memory allocations to see if we free them all as well
		auto iterator = std::find(allocations.begin(), allocations.end(), DebugMemoryAllocation{ filename, line, 0, numBytes, memory });
		if (iterator == allocations.end())
		{
			allocations.emplace_back(DebugMemoryAllocation{ filename, line, 1, numBytes, memory });
		}
		else
		{
			if (iterator->references <= 0)
			{
				iterator->references++;
				iterator->fileAllocator = filename;
				iterator->memorySize = numBytes;
				iterator->fileAllocatorLine = line;
				iterator->memory = memory;
			}
			else
			{
				g_logger_error("Tried to allocate memory that has already been allocated... This should never be hit. If it is, we have a problem.");
			}
		}

		return (void*)((uint8*)memory + bufferPadding);
	}

	// If we aren't tracking memory, just return malloc
	return std::malloc(numBytes);
}

void* _g_memory_realloc(const char* filename, int line, void* oldMemory, size_t numBytes)
{
	if (trackMemoryAllocations)
	{
		// In memory error tracking mode, I'll add sentinel values to the beginning and
		// end of the block of memory to ensure it doesn't have any errors on free
		oldMemory = (void*)((uint8*)oldMemory - bufferPadding);
		auto oldMemoryIter = std::find(allocations.begin(), allocations.end(), DebugMemoryAllocation{ filename, line, 0, numBytes, oldMemory });
		if (oldMemoryIter == allocations.end())
		{
			g_logger_error("Tried to reallocate a block of memory that was not allocated by this library.");
			return nullptr;
		}
		void* sentinelCopy = std::malloc(sizeof(uint8) * bufferPadding);
		std::memcpy(sentinelCopy, (void*)((uint8*)oldMemory + oldMemoryIter->memorySize - bufferPadding), sizeof(uint8) * bufferPadding);

		numBytes += bufferPadding * 2 * sizeof(uint8);
		void* newMemory = std::realloc(oldMemory, numBytes);
		if (newMemory)
		{
			// Copy the old sentinel values that were at the end to the end of the new memory block
			oldMemoryIter->memorySize = numBytes;
			std::memcpy((uint8*)newMemory + numBytes - bufferPadding, sentinelCopy, bufferPadding * sizeof(uint8));
			std::free(sentinelCopy);
		}

		std::lock_guard<std::mutex> lock(memoryMtx);
		// If we are in a debug build, track all memory allocations to see if we free them all as well
		auto newMemoryIter = std::find(allocations.begin(), allocations.end(), DebugMemoryAllocation{ filename, line, 0, numBytes, newMemory });
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
				allocations.emplace_back(DebugMemoryAllocation{ filename, line, 1, numBytes, newMemory });
			}
			else
			{
				if (newMemoryIter->references <= 0)
				{
					newMemoryIter->references++;
					newMemoryIter->fileAllocator = filename;
					newMemoryIter->memorySize = numBytes;
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
		return (void*)((uint8*)newMemory + bufferPadding);
	}

	// If we're not tracking allocations, just return realloc
	return std::realloc(oldMemory, numBytes);;
}

void _g_memory_free(const char* filename, int line, void* memory)
{
	if (trackMemoryAllocations)
	{
		memory = (void*)((uint8*)memory - bufferPadding);
		std::lock_guard<std::mutex> lock(memoryMtx);
		auto iterator = std::find(allocations.begin(), allocations.end(), DebugMemoryAllocation{ filename, line, 0, 0, memory });
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

			if (iterator->references == 0)
			{
				// Check to see if our special flags were changed. If they were, we have heap corruption!
				uint8* memoryBytes = (uint8*)memory;
				for (int i = 0; i < bufferPadding; i++)
				{
					if (memoryBytes[i] != specialMemoryFlags[i % specialMemoryFlags.size()])
					{
						g_logger_warning("Heap corruption detected. Buffer underrun in memory allocated from: '%s' line: %d", iterator->fileAllocator, iterator->fileAllocatorLine);
						break;
					}
				}

				memoryBytes = (uint8*)memory + iterator->memorySize - bufferPadding;
				for (int i = 0; i < bufferPadding; i++)
				{
					if (memoryBytes[i] != specialMemoryFlags[i % specialMemoryFlags.size()])
					{
						g_logger_warning("Heap corruption detected. Buffer overrun in memory allocated from: '%s' line: %d", iterator->fileAllocator, iterator->fileAllocatorLine);
						break;
					}
				}
			}
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
			g_logger_warning("Memory leak detected. Leaked '%zu' bytes allocated from: '%s' line: %d", alloc.memorySize - 10, alloc.fileAllocator, alloc.fileAllocatorLine);
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

/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2022 Gabe Ambrosio
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/
