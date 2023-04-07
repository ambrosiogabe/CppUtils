/*
 Do this:
	 #define GABE_CPP_UTILS_IMPL
 before you include this file in *one* C++ file to create the implementation.

 // i.e. it should look like this in *one* source file:
 #define GABE_CPP_UTILS_IMPL
 #include "cppUtils.hpp"



 -------- LICENSE --------

 Open Source, see end of file



 -------- DOCUMENTATION --------

 This library is a single header library, inspired by the stb libraries. It
 includes several utilities to do simple things like logging and
 memory tracking/heap corruption detection.Here is a list of the functions
 and some simple documentation For DLL info, look at the end of the documentation.



 -------- MEMORY TRACKER/HEAP CORRUPTION DETECTOR --------

 -------- BASIC USAGE --------

 int main()
 {
	g_memory_init_padding(true, 12);

	// Run my app

	g_memory_dumpMemoryLeaks(); // Get any memory leaks
	g_memory_deinit(); // Release resources like mutexes
 }

 -------- FUNCTIONS --------

 g_memory_init(bool detectMemoryLeaks);
 g_memory_init_padding(bool detectMemoryLeaks, uint16 bufferPadding)
  - detectMemoryLeaks   -- Set this to true to detect memory corruption/leaks. When this is set to
						   false, this is simply a wrapper around std::malloc, std::realloc, etc.
  - bufferPadding       -- This is how many extra bytes will be allocated at the start and end of
						   each memory allocation. I advise that you set this to a very low
						   number, like 1-2 bytes, if you plan on releasing your app with this turned on.

 g_memory_dumpMemoryLeaks();
 g_memory_deinit();

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



 -------- LOGGER --------

 -------- BASIC USAGE --------

 NOTE: If you have cppPrint.hpp in your project as well, as long as you include cppPrint.hpp *before* including
	   cppUtils.hpp, you can (and must) use the cppPrint syntax in all logging like:

		   printf("Some string {}", "Some Format");

	   instead of the traditional printf format syntax. The rest of the instructions should remain the same.

 All logging functions follow the format of printf, where you call a function
 like:

	 printf("Some string %s", "Some Format");

 by calling it with a string, followed by a list of formatting options.

	g_logger_info("Some string %s", "Some Format");

 You can set the log level and a specific log directory. Make sure to free
 the logger before the application exits. A typical use in an application may
 look like this:

 int main()
 {
	g_logger_init();
	g_logger_set_level(g_logger_level_All);
	g_logger_set_log_directory("C:\\dev\\myapp\\logs");

	// Run the application and log as needed

	g_logger_free();
 }

 -------- FUNCTIONS --------

	g_logger_init();
	g_logger_free();

	g_logger_setLogDirectory(const char* file)
	g_logger_set_level(g_logger_level level)

	g_logger_log(const char* format, ...args)
	g_logger_info(const char* format, ...args)
	g_logger_warning(const char* format, ...args)
	g_logger_error(const char* format, ...args)
	g_logger_assert(bool condition, const char* failureFormat, ...args)

 -------- FUNCTION DESCRIPTIONS --------

 These are all the logging options listed from least severe, to most severe:

	g_logger_log(const char* format, ...args)
	g_logger_info(const char* format, ...args)
	g_logger_warning(const char* format, ...args)
	g_logger_error(const char* format, ...args)

 In windows, on dev assertions will break into the debugger on visual studio. In production it
 crashes with a window printing where to find the log file (if logging to a file is enabled)
 and the assertion failure message.

	g_logger_assert(bool condition, const char* failureMessage, ...format)

 Set up logging to a file as well the standard output.

	g_logger_setLogDirectory(const char* file)

 Use this to restrict the messages to a particular level. This will log anything
 at the preferred level or higher:

	g_logger_set_level(g_logger_level level)


 -------- DLL STUFF --------

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

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

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

	GABE_CPP_UTILS_API void g_memory_init(bool detectMemoryLeaks);
	GABE_CPP_UTILS_API void g_memory_init_padding(bool detectMemoryLeaks, uint16 bufferPadding);
	GABE_CPP_UTILS_API void g_memory_deinit(void);
	GABE_CPP_UTILS_API void g_memory_dumpMemoryLeaks(void);

	GABE_CPP_UTILS_API bool g_memory_compareMem(void* a, size_t aLength, void* b, size_t bLength);
	GABE_CPP_UTILS_API void g_memory_zeroMem(void* memory, size_t numBytes);
	GABE_CPP_UTILS_API void g_memory_copyMem(void* dst, void* src, size_t numBytes);

	// ----------------------------------
	// Logging Utils
	// ----------------------------------
	typedef enum g_logger_level
	{
		g_logger_level_All = 0,
		g_logger_level_Log = 1,
		g_logger_level_Info = 2,
		g_logger_level_Warning = 3,
		g_logger_level_Error = 4,
		g_logger_level_Assert = 5,
		g_logger_level_None = 6,
	} g_logger_level;

#ifndef USE_GABE_CPP_PRINT
#ifdef _WIN32
#define VA_ARGS(...) , ##__VA_ARGS__
#define g_logger_log(format, ...) _g_logger_cStdCommonPrint(__FILE__, __LINE__, g_logger_level_Log, FOREGROUND_BLUE | FOREGROUND_GREEN, format VA_ARGS(__VA_ARGS__))
#define g_logger_info(format, ...) _g_logger_cStdCommonPrint(__FILE__, __LINE__, g_logger_level_Info, FOREGROUND_GREEN, format VA_ARGS(__VA_ARGS__))
#define g_logger_warning(format, ...) _g_logger_cStdCommonPrint(__FILE__, __LINE__, g_logger_level_Warning, FOREGROUND_GREEN | FOREGROUND_RED, format VA_ARGS(__VA_ARGS__))
#define g_logger_error(format, ...) _g_logger_cStdCommonPrint(__FILE__, __LINE__, g_logger_level_Error, FOREGROUND_RED, format VA_ARGS(__VA_ARGS__))
#define g_logger_assert(condition, format, ...) _g_logger_assert(__FILE__, __LINE__, condition, format, __VA_ARGS__)

	typedef unsigned short WORD;
	GABE_CPP_UTILS_API void _g_logger_cStdCommonPrint(const char* filename, int line, g_logger_level level, WORD color, const char* format, ...);
#elif defined(unix) || defined(__unix) || defined(__unix__)
#define g_logger_log(format, ...) _g_logger_cStdCommonPrint(__FILE__, __LINE__, g_logger_level_Log, ColorCode::KBLU, format,##__VA_ARGS__)
#define g_logger_info(format, ...) _g_logger_cStdCommonPrint(__FILE__, __LINE__, g_logger_level_Info, ColorCode::KGRN, format,##__VA_ARGS__)
#define g_logger_warning(format, ...) _g_logger_cStdCommonPrint(__FILE__, __LINE__, , g_logger_level_Warning, ColorCode::KYEL, format,##__VA_ARGS__)
#define g_logger_error(format, ...) _g_logger_cStdCommonPrint(__FILE__, __LINE__, , g_logger_level_Error, ColorCode::KRED, format,##__VA_ARGS__)
#define g_logger_assert(condition, format, ...) _g_logger_assert(__FILE__, __LINE__, condition, format,##__VA_ARGS__)

	GABE_CPP_UTILS_API void _g_logger_cStdCommonPrint(const char* filename, int line, g_logger_level level, const char* color, const char* format, ...);
#else 
#error "Unsupported platform for logging."
#endif
#endif // USE_GABE_CPP_PRINT

	GABE_CPP_UTILS_API void _g_logger_assert(const char* filename, int line, int condition, const char* format, ...);

	GABE_CPP_UTILS_API void g_logger_set_level(g_logger_level level);
	GABE_CPP_UTILS_API g_logger_level g_logger_get_level(void);

	GABE_CPP_UTILS_API void g_logger_init(void);
	GABE_CPP_UTILS_API void g_logger_free(void);

	GABE_CPP_UTILS_API void g_logger_set_log_directory(const char* directory);

	// ----------------------------------
	// Thread safety utils
	// ----------------------------------

	// These "mutexes" are really Win32 critical sections
	// You should use a different library if you need interprocess
	// mutexes and not single-multithread-process mutexes

	GABE_CPP_UTILS_API void* g_thread_createMutex(void);
	GABE_CPP_UTILS_API void g_thread_lockMutex(void* mtx);
	GABE_CPP_UTILS_API void g_thread_releaseMutex(void* mtx);
	GABE_CPP_UTILS_API void g_thread_freeMutex(void* mtx);

#ifdef __cplusplus
}
#endif

// Template version of logging for using this library with GABE_CPP_PRINT library
#ifdef USE_GABE_CPP_PRINT
#include <cppUtils/cppPrint.hpp>

#ifdef _WIN32

typedef unsigned short WORD;
GABE_CPP_UTILS_API void _g_logger_printPreamble(const char* filename, int line, char* buf, size_t bufSize, WORD color);
GABE_CPP_UTILS_API void _g_logger_printPostamble(const char* filename, int line, char* buf, size_t bufSize);

template<typename...Args>
GABE_CPP_UTILS_API void _g_logger_gabeCommonPrint(const char* filename, int line, g_logger_level level, WORD color, const char* format, Args... args)
{
	if (g_logger_get_level() <= level)
	{
		char buf[20] = { 0 };
		_g_logger_printPreamble(filename, line, buf, sizeof(buf), color);
		g_io_printf(format, args...);
		_g_logger_printPostamble(filename, line, buf, sizeof(buf));
	}
}

GABE_CPP_UTILS_API void _g_logger_assertGabePreamble(const char* filename, int line, char* buf, size_t bufSize);
GABE_CPP_UTILS_API void _g_logger_assertGabePostamble(const char* filename, int line, char* buf, size_t bufSize);

template<typename...Args>
GABE_CPP_UTILS_API void _g_logger_gabeAssert(const char* filename, int line, bool condition, const char* format, Args... args)
{
	if (g_logger_get_level() <= g_logger_level_Assert)
	{
		if (!condition)
		{
			char buf[20] = { 0 };
			_g_logger_assertGabePreamble(filename, line, buf, sizeof(buf));
			g_io_printf(format, args...);
			_g_logger_assertGabePostamble(filename, line, buf, sizeof(buf));
		}
	}
}

#define VA_ARGS(...) , ##__VA_ARGS__
#define g_logger_log(format, ...) _g_logger_gabeCommonPrint(__FILE__, __LINE__, g_logger_level_Log, FOREGROUND_BLUE | FOREGROUND_GREEN, format VA_ARGS(__VA_ARGS__))
#define g_logger_info(format, ...) _g_logger_gabeCommonPrint(__FILE__, __LINE__, g_logger_level_Info, FOREGROUND_GREEN, format VA_ARGS(__VA_ARGS__))
#define g_logger_warning(format, ...) _g_logger_gabeCommonPrint(__FILE__, __LINE__, g_logger_level_Warning, FOREGROUND_GREEN | FOREGROUND_RED, format VA_ARGS(__VA_ARGS__))
#define g_logger_error(format, ...) _g_logger_gabeCommonPrint(__FILE__, __LINE__, g_logger_level_Error, FOREGROUND_RED, format VA_ARGS(__VA_ARGS__))
#define g_logger_assert(condition, format, ...) _g_logger_gabeAssert(__FILE__, __LINE__, condition, format, __VA_ARGS__)

#endif // _WIN32
#endif // USE_GABE_CPP_PRINT

#endif // GABE_CPP_UTILS_H





// ===================================================================================
// Implementation
// ===================================================================================
#ifdef GABE_CPP_UTILS_IMPL
#include <memory.h>

// Common includes
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
// Ignore Win32 warnings
#pragma warning( push )
#pragma warning( disable : 5105)
#include <Windows.h>
#include <crtdbg.h>
// For mutex on Windows
#include <synchapi.h>
#pragma warning( pop )
#endif

#ifdef __cplusplus
#define GABE_CPP_UTILS_EXTERN extern "C"
#else 
#define GABE_CPP_UTILS_EXTERN
#endif 

// Forward declarations
static void* g_thread_createMutexUntracked(void);
static void g_thread_freeMutexUntracked(void* mutex);

// ----------------------------------
// C Memory Implementation
// ----------------------------------
typedef struct gma_DebugMemoryAllocation
{
	const char* fileAllocator;
	int fileAllocatorLine;
	int references;
	size_t memorySize;
	void* memory;
} gma_DebugMemoryAllocation;

typedef struct gma_DebugMemoryAllocationList
{
	gma_DebugMemoryAllocation* data;
	size_t length;
	size_t maxCapacity;
} gma_DebugMemoryAllocationList;

static bool gma_DebugMemoryAllocation_equals(const gma_DebugMemoryAllocation* a, const gma_DebugMemoryAllocation* b)
{
	return a->memory == b->memory;
}

static gma_DebugMemoryAllocation* gma_DebugMemoryAllocation_find(gma_DebugMemoryAllocationList* list, const gma_DebugMemoryAllocation* value)
{
	for (size_t i = 0; i < list->length; i++)
	{
		if (gma_DebugMemoryAllocation_equals(list->data + i, value))
		{
			return list->data + i;
		}
	}

	return NULL;
}

static void gma_DebugMemoryAllocation_push(gma_DebugMemoryAllocationList* list, const gma_DebugMemoryAllocation* element)
{
	// TODO: Do heavy testing here to make sure this is rock-solid
	if (list->length + 1 > list->maxCapacity)
	{
		// Grow our list
		list->maxCapacity *= 2;
		gma_DebugMemoryAllocation* newPtr = (gma_DebugMemoryAllocation*)realloc(list->data, sizeof(gma_DebugMemoryAllocation) * list->maxCapacity);
		g_logger_assert(list->data != NULL, "Realloc failed, out of memory.");
		list->data = newPtr;
	}

	list->data[list->length] = *element;
	list->length += 1;
}

static void gma_DebugMemoryAllocationList_init(gma_DebugMemoryAllocationList* list)
{
	list->length = 0;
	list->maxCapacity = 8;
	list->data = (gma_DebugMemoryAllocation*)malloc(sizeof(gma_DebugMemoryAllocation) * list->maxCapacity);
}

static void* memoryMtx = NULL;
static gma_DebugMemoryAllocationList allocations;
static bool trackMemoryAllocations = false;
#define specialMemoryFlagsSize 8
static const uint8 specialMemoryFlags[specialMemoryFlagsSize] = { (uint8)'h', (uint8)'e', (uint8)'Y', (uint8)'G' , (uint8)'a', (uint8)'b', (uint8)'e', (uint8)'!' };
static uint16 bufferPadding = 5;

void g_memory_init(bool detectMemoryErrors)
{
	g_memory_init_padding(detectMemoryErrors, 0);
}

void g_memory_init_padding(bool detectMemoryErrors, uint16 inBufferPadding)
{
	trackMemoryAllocations = detectMemoryErrors;
	bufferPadding = inBufferPadding;
	gma_DebugMemoryAllocationList_init(&allocations);
	memoryMtx = g_thread_createMutexUntracked();
}

void g_memory_deinit(void)
{
	if (memoryMtx)
	{
		g_thread_freeMutexUntracked(memoryMtx);
	}
}

static inline void setMemoryPaddingPost(uint8* memoryBase, size_t numBytes)
{
	uint8* memoryBytes = ((uint8*)memoryBase) + numBytes - bufferPadding;
	for (uint16 i = 0; i < bufferPadding; i++)
	{
		memoryBytes[i] = specialMemoryFlags[i % specialMemoryFlagsSize];
	}
}

static inline void setMemoryPaddingPre(uint8* memoryBase)
{
	uint8* memoryBytes = (uint8*)memoryBase;
	for (uint16 i = 0; i < bufferPadding; i++)
	{
		memoryBytes[i] = specialMemoryFlags[i % specialMemoryFlagsSize];
	}
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
		// TODO: Might be cool to inject malloc like vulkan to allow custom allocators
		void* memory = malloc(numBytes);
		if (memory)
		{
			setMemoryPaddingPre((uint8*)memory);
			setMemoryPaddingPost((uint8*)memory, numBytes);
		}

		g_thread_lockMutex(memoryMtx);
		// If we are in a debug build, track all memory allocations to see if we free them all as well
		gma_DebugMemoryAllocation tmp = {
			filename,
			line,
			1,
			numBytes,
			memory
		};
		gma_DebugMemoryAllocation* iterator = gma_DebugMemoryAllocation_find(&allocations, &tmp);
		if (iterator == NULL)
		{
			gma_DebugMemoryAllocation_push(&allocations, &tmp);
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

		g_thread_releaseMutex(memoryMtx);
		return (void*)((uint8*)memory + bufferPadding);
	}

	// If we aren't tracking memory, just return malloc
	return malloc(numBytes);
}

void* _g_memory_realloc(const char* filename, int line, void* oldMemory, size_t numBytes)
{
	if (trackMemoryAllocations)
	{
		// In memory error tracking mode, I'll add sentinel values to the beginning and
		// end of the block of memory to ensure it doesn't have any errors on free

		// If ptr is NULL, the behavior is the same as calling malloc(new_size).
		if (oldMemory == NULL)
		{
			return _g_memory_allocate(filename, line, numBytes);
		}

		// If numBytes is 0 then that's undefined behavior
		if (numBytes == 0)
		{
			g_logger_warning("Realloc called with newSize of 0 bytes. This is undefined behavior.\n\trealloc(ptr, 0) is undefined.");
		}

		g_thread_lockMutex(memoryMtx);

		oldMemory = (void*)((uint8*)oldMemory - bufferPadding);
		gma_DebugMemoryAllocation tmp = {
			filename,
			line,
			0,
			numBytes,
			oldMemory
		};
		gma_DebugMemoryAllocation* oldMemoryIter = gma_DebugMemoryAllocation_find(&allocations, &tmp);
		numBytes += bufferPadding * 2 * sizeof(uint8);
		void* newMemory = realloc(oldMemory, numBytes);

		if (oldMemoryIter != NULL)
		{
			oldMemoryIter->references--;
			if (oldMemoryIter->references > 0)
			{
				g_logger_error("Tried to reallocate memory that has already been allocated... This should never be hit. If it is, we have a problem.");
			}
		}
		else
		{
			g_logger_error("This should never be hit. Realloc was called with memory that wasn't allocated by this library.");
		}

		// Clear the padding bits after the new allocation just in case the new allocation
		// is smaller. This way a valid memory write doesn't get misinterpreted as a buffer
		// overrun
		setMemoryPaddingPost((uint8*)newMemory, numBytes);

		// If we are in a debug build, track all memory allocations to see if we free them all as well
		gma_DebugMemoryAllocation newTmp = {
			filename,
			line,
			0,
			numBytes,
			newMemory
		};
		gma_DebugMemoryAllocation* newMemoryIter = gma_DebugMemoryAllocation_find(&allocations, &newTmp);
		if (newMemoryIter == NULL)
		{
			gma_DebugMemoryAllocation newAlloc = {
				filename,
				line,
				1,
				numBytes,
				newMemory
			};
			gma_DebugMemoryAllocation_push(&allocations, &newAlloc);
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

		g_thread_releaseMutex(memoryMtx);

		// If realloc expanded the memory in-place, then we don't need to do anything because no "new" memory locations were allocated
		// and no "new" memory references were created
		return (void*)((uint8*)newMemory + bufferPadding);
	}

	// If we're not tracking allocations, just return realloc
	return realloc(oldMemory, numBytes);;
}

void _g_memory_free(const char* filename, int line, void* memory)
{
	if (trackMemoryAllocations)
	{
		memory = (void*)((uint8*)memory - bufferPadding);

		g_thread_lockMutex(memoryMtx);

		gma_DebugMemoryAllocation tmp = {
			filename,
			line,
			0,
			0,
			memory
		};
		gma_DebugMemoryAllocation* iterator = gma_DebugMemoryAllocation_find(&allocations, &tmp);
		if (iterator == NULL)
		{
#ifndef USE_GABE_CPP_PRINT
			g_logger_error("Tried to free invalid memory that was never allocated at '%s' line: %d", filename, line);
#else
			g_logger_error("Tried to free invalid memory that was never allocated at '{}' line: {}", filename, line);
#endif
	}
		else if (iterator->references <= 0)
		{
#ifndef USE_GABE_CPP_PRINT
			g_logger_error("Tried to free memory that has already been freed.");
			g_logger_error("Code that attempted to free: '%s' line: %d", filename, line);
			g_logger_error("Code that allocated the memory block: '%s' line: %d", iterator->fileAllocator, iterator->fileAllocatorLine);
#else
			g_logger_error("Tried to free memory that has already been freed.");
			g_logger_error("Code that attempted to free: '{}' line: {}", filename, line);
			g_logger_error("Code that allocated the memory block: '{}' line: {}", iterator->fileAllocator, iterator->fileAllocatorLine);
#endif
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
					if (memoryBytes[i] != specialMemoryFlags[i % specialMemoryFlagsSize])
					{
#ifndef USE_GABE_CPP_PRINT
						g_logger_warning("Heap corruption detected. Buffer underrun in memory allocated from: '%s' line: %d", iterator->fileAllocator, iterator->fileAllocatorLine);
#else 
						g_logger_warning("Heap corruption detected. Buffer underrun in memory allocated from: '{}' line: {}", iterator->fileAllocator, iterator->fileAllocatorLine);
#endif
						break;
				}
			}

				memoryBytes = (uint8*)memory + iterator->memorySize - bufferPadding;
				for (int i = 0; i < bufferPadding; i++)
				{
					if (memoryBytes[i] != specialMemoryFlags[i % specialMemoryFlagsSize])
					{
#ifndef USE_GABE_CPP_PRINT
						g_logger_warning("Heap corruption detected. Buffer overrun in memory allocated from: '%s' line: %d", iterator->fileAllocator, iterator->fileAllocatorLine);
#else 
						g_logger_warning("Heap corruption detected. Buffer overrun in memory allocated from: '{}' line: {}", iterator->fileAllocator, iterator->fileAllocatorLine);
#endif
						break;
				}
		}
			}
		}

		g_thread_releaseMutex(memoryMtx);
	}

	// When debug is turned off we literally just free the memory, so it will throw a segfault if a
	// faulty release build was published
	free(memory);
}

void g_memory_dumpMemoryLeaks(void)
{
	g_thread_lockMutex(memoryMtx);

	for (size_t i = 0; i < allocations.length; i++)
	{
		gma_DebugMemoryAllocation* alloc = allocations.data + i;
		if (alloc->references > 0)
		{
#ifndef USE_GABE_CPP_PRINT
			g_logger_warning("Memory leak detected. Leaked '%zu' bytes allocated from: '%s' line: %d", alloc->memorySize - (bufferPadding * 2), alloc->fileAllocator, alloc->fileAllocatorLine);
#else
			g_logger_warning("Memory leak detected. Leaked '{}' bytes allocated from: '{}' line: {}", alloc->memorySize - (bufferPadding * 2), alloc->fileAllocator, alloc->fileAllocatorLine);
#endif
	}
}

	g_thread_releaseMutex(memoryMtx);
}

bool g_memory_compareMem(void* a, size_t aLength, void* b, size_t bLength)
{
	if (aLength != bLength) return FALSE;
	return (memcmp(a, b, bLength) == 0);
}

void g_memory_zeroMem(void* memory, size_t numBytes)
{
	memset(memory, 0, numBytes);
}

void g_memory_copyMem(void* dst, void* src, size_t numBytes)
{
	memcpy(dst, src, numBytes);
}


// ----------------------------------
// Logging Implementation Common C11
// ----------------------------------

// Initialize these variables just in case init isn't called for some reason
static g_logger_level log_level = g_logger_level_All;

// If this is not nullptr, logging to file is enabled
static FILE* logFile = NULL;
static char* logFilePath = NULL;
static void* logMutex = NULL;

void g_logger_set_level(g_logger_level level)
{
	log_level = level;
}

g_logger_level g_logger_get_level(void)
{
	return log_level;
}

void g_logger_init(void)
{
	logFile = NULL;
	logFilePath = NULL;
	log_level = g_logger_level_All;
	logMutex = g_thread_createMutexUntracked();
}

void g_logger_free(void)
{
	if (logFile)
	{
		fclose(logFile);
		logFile = NULL;
	}

	if (logFilePath)
	{
		free(logFilePath);
		logFilePath = NULL;
	}

	if (logMutex)
	{
		g_thread_freeMutexUntracked(logMutex);
		logMutex = NULL;
	}
}

void g_logger_set_log_directory(const char* directory)
{
	// TODO: Better error checking
	// Max path on windows, should be large enough for linux, if not we should error out here
#define maxPath 261
	char timebuf[maxPath] = { 0 };

	time_t now;
	time(&now);
	struct tm localTime;
	localtime_s(&localTime, &now);
	strftime(timebuf, sizeof(timebuf), "/log_%Y-%m-%d_%I_%M_%S.txt", &localTime);

	size_t filenameLength = strlen(timebuf);
	size_t dirStringLength = strlen(directory);
	// Minus 1 here for the null character
	if (dirStringLength >= (maxPath - filenameLength - 1))
	{
		printf("Directory name to long. Max length 260 gets exceeded with log filename. Not setting log directory to: %s\n", directory);
		return;
	}
	// +1 for null byte
	logFilePath = (char*)malloc(sizeof(char) * (filenameLength + dirStringLength + 1));
	if (!logFilePath)
	{
		printf("Failed to allocate memory for the log file path. Out of memory. Returning early.");
		return;
	}

	memcpy(logFilePath, directory, sizeof(char) * dirStringLength);
	memcpy(logFilePath + (sizeof(char) * dirStringLength), timebuf, sizeof(char) * filenameLength);
	logFilePath[dirStringLength + filenameLength] = '\0';

	fopen_s(&logFile, logFilePath, "wb");
	if (!logFile)
	{
		printf("Failed to open file '%s' to log to. Please make sure the log directory exists, otherwise this will fail.", directory);
		free(logFilePath);
		logFilePath = NULL;
	}

#undef maxPath
}


// ----------------------------------------
// Logging Implementation OS specific C++
// ----------------------------------------
#ifdef _WIN32
#ifdef USE_GABE_CPP_PRINT
void _g_logger_printPreamble(const char* filename, int line, char* buf, size_t bufSize, WORD color)
{
	g_thread_lockMutex(logMutex);

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
	g_io_printf("{} (line {}) Log: \n", filename, line);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0F);

	time_t now;
	time(&now);
	struct tm localTime;
	localtime_s(&localTime, &now);
	strftime(buf, bufSize, "%Y-%m-%d %I:%M:%S", &localTime);
	g_io_printf("[{}]: ", buf);
}

void _g_logger_printPostamble(const char* filename, int line, char* buf, size_t)
{
	g_io_printf("\n");

	if (logFile)
	{
		fprintf(logFile, "%s (line %d) Log: \n", filename, line);
		fprintf(logFile, "[%s]: ", buf);

		// TODO: Implement me for printing to files
		//va_list fargs;
		//va_start(fargs, format);
		//vfprintf(logFile, format, fargs);
		//va_end(fargs);

		fprintf(logFile, "\n");
	}

	g_thread_releaseMutex(logMutex);
}

void _g_logger_assertGabePreamble(const char* filename, int line, char* buf, size_t bufSize)
{
	g_thread_lockMutex(logMutex);

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
	g_io_printf("{} (line {}) Assertion Failure: \n", filename, line);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0F);

	time_t now;
	time(&now);
	struct tm localTime;
	localtime_s(&localTime, &now);
	strftime(buf, bufSize, "%Y-%m-%d %I:%M:%S", &localTime);
	g_io_printf("[{}]: ", buf);
}

void _g_logger_assertGabePostamble(const char* filename, int line, char* buf, size_t)
{
	g_io_printf("\n");

	if (logFile)
	{
		fprintf(logFile, "%s (line %d) Assertion Failure: \n", filename, line);
		fprintf(logFile, "[%s]: ", buf);

		//va_list fargs;
		//va_start(fargs, format);
		//vfprintf(logFile, format, fargs);
		//va_end(fargs);

		fprintf(logFile, "\n");
	}

	_CrtDbgBreak();

	// TODO: Find out how to do this
	//MessageBoxA(
	//	NULL,
	//	fullErrorMessageBuffer,
	//	"Critical Assertion Failure",
	//	MB_ICONEXCLAMATION | MB_OK
	//);

	g_thread_releaseMutex(logMutex);
	g_logger_free();
	exit(-1);
}
#else // end USE_GABE_CPP_PRINT

void _g_logger_cStdCommonPrint(const char* filename, int line, g_logger_level level, WORD color, const char* format, ...)
{
	if (g_logger_get_level() <= level)
	{
		g_thread_lockMutex(logMutex);

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
		printf("%s (line %d) Info: \n", filename, line);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0F);

		time_t now;
		time(&now);
		char buf[20] = { 0 };
		struct tm localTime;
		localtime_s(&localTime, &now);
		strftime(buf, sizeof(buf), "%Y-%m-%d %I:%M:%S", &localTime);
		printf("[%s]: ", buf);

		va_list args;
		va_start(args, format);
		vprintf(format, args);
		va_end(args);

		printf("\n");

		if (logFile)
		{
			fprintf(logFile, "%s (line %d) Info: \n", filename, line);
			fprintf(logFile, "[%s]: ", buf);

			va_list fargs;
			va_start(fargs, format);
			vfprintf(logFile, format, fargs);
			va_end(fargs);

			fprintf(logFile, "\n");
		}

		g_thread_releaseMutex(logMutex);
	}
}
#endif // not defined USE_GABE_CPP

void _g_logger_assert(const char* filename, int line, int condition, const char* format, ...)
{
	if (g_logger_get_level() <= g_logger_level_Assert)
	{
		if (!condition)
		{
			g_thread_lockMutex(logMutex);

#define fullErrorMessageBufferSize 4096
			char fullErrorMessageBuffer[fullErrorMessageBufferSize];

			size_t offset = 0;
			sprintf_s(
				fullErrorMessageBuffer,
				fullErrorMessageBufferSize,
				"Critical Assertion Failure\r\n\r\n%s (line %d) Assertion Failure: \r\n",
				filename, line
			);
			offset = strlen(fullErrorMessageBuffer);

			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
			printf("%s (line %d) Assertion Failure: \n", filename, line);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0F);

			time_t now;
			time(&now);
			char buf[20] = { 0 };
			struct tm localTime;
			localtime_s(&localTime, &now);
			strftime(buf, sizeof(buf), "%Y-%m-%d %I:%M:%S", &localTime);
			printf("[%s]: ", buf);
			sprintf_s(
				fullErrorMessageBuffer + offset,
				fullErrorMessageBufferSize - offset,
				"[%s]",
				buf
			);
			offset = strlen(fullErrorMessageBuffer);

			va_list args;
			va_start(args, format);
			vprintf(format, args);
			va_end(args);

#define bufferCount 512
			char buffer[bufferCount];
			va_start(args, format);
			vsnprintf(buffer, bufferCount, format, args);
			va_end(args);

			sprintf_s(
				fullErrorMessageBuffer + offset,
				fullErrorMessageBufferSize - offset,
				"%s\r\n",
				buffer
			);
			offset = strlen(fullErrorMessageBuffer);
			printf("\n");

			if (logFile)
			{
				fprintf(logFile, "%s (line %d) Assertion Failure: \n", filename, line);
				fprintf(logFile, "[%s]: ", buf);

				va_list fargs;
				va_start(fargs, format);
				vfprintf(logFile, format, fargs);
				va_end(fargs);

				fprintf(logFile, "\n");

				sprintf_s(
					fullErrorMessageBuffer + offset,
					fullErrorMessageBufferSize - offset,
					"\r\n\r\nFull log file located at %s",
					logFilePath
				);
			}

			_CrtDbgBreak();

			MessageBoxA(
				NULL,
				fullErrorMessageBuffer,
				"Critical Assertion Failure",
				MB_ICONEXCLAMATION | MB_OK
			);

			g_thread_releaseMutex(logMutex);
			g_logger_free();
			exit(-1);
		}
	}

#undef bufferCount
#undef fullErrorMessageBufferSize
}

#elif defined(unix) || defined(__unix) || defined(__unix__) // end LOGGING_IMPL_WIN32
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

void _g_logger_cStdCommonPrint(const char* filename, int line, g_logger_level level, const char* color, const char* format, ...)
{
	if (g_logger_get_level() <= level)
	{
		g_thread_lockMutex(logMutex);

		printf("%s%s (line %d) Log: \n", color, filename, line);

		std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		char buf[20] = { 0 };
		std::strftime(buf, sizeof(buf), "%Y-%m-%d %I:%M:%S", std::localtime(&now));
		printf("%s[%s]: ", ColorCode::KNRM, buf);

		va_list args;
		va_start(args, format);
		vprintf(format, args);
		va_end(args);

		printf("\n");

		if (logFile)
		{
			fprintf(logFile, "%s (line %d) Log: \n", filename, line);
			fprintf(logFile, "[%s]: ", buf);

			va_list fargs;
			va_start(fargs, format);
			vfprintf(logFile, format, fargs);
			va_end(fargs);

			fprintf(logFile, "\n");
		}

		g_thread_releaseMutex(logMutex);
	}
}

void _g_logger_assert(const char* filename, int line, int condition, const char* format, ...)
{
	if (g_logger_get_level() <= g_logger_level_Assert)
	{
		if (!condition)
		{
			g_thread_lockMutex(logMutex);

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

			if (logFile)
			{
				fprintf(logFile, "%s (line %d) Log: \n", filename, line);
				fprintf(logFile, "[%s]: ", buf);

				va_list fargs;
				va_start(fargs, format);
				vfprintf(logFile, format, fargs);
				va_end(fargs);

				fprintf(logFile, "\n");
			}

			std::raise(SIGINT);
			g_logger_free();
			g_thread_releaseMutex(logMutex);

			exit(-1);
		}
	}
}

#else
#error "Unsupported platform for logging implementation."
#endif // end LOGGING_IMPL_LINUX

// ----------------------------------
// Thread safety utils
// ----------------------------------
#ifdef _WIN32 

GABE_CPP_UTILS_API void* g_thread_createMutex(void)
{
	CRITICAL_SECTION* criticalSection = (CRITICAL_SECTION*)g_memory_allocate(sizeof(CRITICAL_SECTION));
	InitializeCriticalSection(criticalSection);

	return (void*)criticalSection;
}

GABE_CPP_UTILS_API void g_thread_lockMutex(void* mtx)
{
	CRITICAL_SECTION* critical = (CRITICAL_SECTION*)mtx;
	EnterCriticalSection(critical);
}

GABE_CPP_UTILS_API void g_thread_releaseMutex(void* mtx)
{
	CRITICAL_SECTION* critical = (CRITICAL_SECTION*)mtx;
	LeaveCriticalSection(critical);
}

GABE_CPP_UTILS_API void g_thread_freeMutex(void* mtx)
{
	if (mtx)
	{
		CRITICAL_SECTION* critical = (CRITICAL_SECTION*)mtx;
		DeleteCriticalSection(critical);
		g_memory_free(mtx);
	}
}

static void* g_thread_createMutexUntracked(void)
{
	CRITICAL_SECTION* criticalSection = (CRITICAL_SECTION*)malloc(sizeof(CRITICAL_SECTION));
	if (criticalSection)
	{
		InitializeCriticalSection(criticalSection);
	}

	return (void*)criticalSection;
}

static void g_thread_freeMutexUntracked(void* mtx)
{
	if (mtx)
	{
		CRITICAL_SECTION* critical = (CRITICAL_SECTION*)mtx;
		DeleteCriticalSection(critical);
		free(mtx);
	}
}

#elif defined(__linux__) // End ThreadImpl _WIN32
// Begin ThreadImpl Linux

GABE_CPP_UTILS_API void* g_thread_createMutex() n
{
	g_logger_assert(false, "TODO: Implement me.");
}

GABE_CPP_UTILS_API void g_thread_lockMutex(void* mtx)
{
	g_logger_assert(false, "TODO: Implement me.");
}

GABE_CPP_UTILS_API void g_thread_releaseMutex(void* mtx)
{
	g_logger_assert(false, "TODO: Implement me.");
}

GABE_CPP_UTILS_API void g_thread_freeMutex(void* mtx)
{
	g_logger_assert(false, "TODO: Implement me.");
}

#endif // End ThreadImpl Linux
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
