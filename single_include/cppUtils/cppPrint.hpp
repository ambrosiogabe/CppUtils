/*
 Do this:
	 #define GABE_CPP_PRINT_IMPL
 before you include this file in *one* C++ file to create the implementation.

 // i.e. it should look like this in *one* source file:
 #define GABE_CPP_PRINT_IMPL
 #include "cppPrint.hpp"



 -------- LICENSE --------

 Open Source, see end of file



 -------- DOCUMENTATION --------

 TODO: Add me


 -------- DLL STUFF --------

 If you want to use this library as part of a DLL, I have created a macro:

	GABE_CPP_PRINT_API

 that does nothing, but prefaces each forward declaration. Simply redefine this
 to your preferred __declspec(dllimport) or __declspec(dllexport) before including this
 file anywhere and it should just work out.
*/
#ifndef GABE_CPP_PRINT_H
#define GABE_CPP_PRINT_H

#include <string>

// Override this define to add DLL support on your platform
#ifndef GABE_CPP_PRINT_API 
#define GABE_CPP_PRINT_API
#endif

#ifndef USE_GABE_CPP_PRINT
#define USE_GABE_CPP_PRINT
#endif

struct g_DumbString;

struct g_cppPrint_io
{
};

extern g_cppPrint_io g_cppPrint_stdout;

template<typename T>
g_cppPrint_io& operator<<(g_cppPrint_io& io, T const& t);

// Helper for raw string literals and char[N] arrays
template<std::size_t N>
using g_cppPrint_sizedCharArray = char[N];

// Specializations for common types
template<>
g_cppPrint_io& operator<<(g_cppPrint_io& io, float const& number);
template<>
g_cppPrint_io& operator<<(g_cppPrint_io& io, int8_t const& integer);
template<>
g_cppPrint_io& operator<<(g_cppPrint_io& io, int16_t const& integer);
template<>
g_cppPrint_io& operator<<(g_cppPrint_io& io, int32_t const& integer);
template<>
g_cppPrint_io& operator<<(g_cppPrint_io& io, int64_t const& integer);
template<>
g_cppPrint_io& operator<<(g_cppPrint_io& io, uint8_t const& integer);
template<>
g_cppPrint_io& operator<<(g_cppPrint_io& io, uint16_t const& integer);
template<>
g_cppPrint_io& operator<<(g_cppPrint_io& io, uint32_t const& integer);
template<>
g_cppPrint_io& operator<<(g_cppPrint_io& io, uint64_t const& integer);
template<>
g_cppPrint_io& operator<<(g_cppPrint_io& io, g_DumbString const& dumbString);
template<>
g_cppPrint_io& operator<<(g_cppPrint_io& io, const char* const& s);
template<>
g_cppPrint_io& operator<<(g_cppPrint_io& io, char* const& s);
template<std::size_t N>
g_cppPrint_io& operator<<(g_cppPrint_io& io, g_cppPrint_sizedCharArray<N> const& s)
{
	_g_io_printf_internal((const char*)s, N);
	return io;
}
template<>
g_cppPrint_io& operator<<(g_cppPrint_io& io, std::string const& str);

GABE_CPP_PRINT_API void _g_io_printf_internal(const char* s, size_t length);

GABE_CPP_PRINT_API void g_io_printf(const char* s);

template<typename T, typename...Args>
GABE_CPP_PRINT_API void _g_io_printf_internal(const char* s, size_t length, const T& value, const Args&... args)
{
	size_t strLength = 0;
	size_t fmtStart = SIZE_MAX;
	for (size_t i = 0; i < length; i++)
	{
		if (s[i] == '{' && fmtStart == SIZE_MAX)
		{
			if (i < length - 1 && s[i + 1] != '{')
			{
				_g_io_printf_internal(s, strLength);
				fmtStart = i;
			}
			else
			{
				_g_io_printf_internal("{", 1);
				strLength = 0;
				i++;
			}
			continue;
		}

		if (fmtStart != SIZE_MAX)
		{
			// Closing the format string
			if (s[i] == '}')
			{
				g_cppPrint_stdout << value;
				// NOTE: Recursively call the variadic template argument, with unpacked variables
				//       The base case, where there is no args... left is handled in the function
				//       above
				if (length >= i + 1)
				{
					return _g_io_printf_internal((s + i + 1), length - i - 1, args...);
				}
				return;
			}
		}
		else
		{
			strLength++;
		}
	}

	// TODO: Do we really want to throw an error for extra arguments provided to printf
	//       Do performance profiling and see if it causes any significant impact
	throw std::runtime_error("Extra arguments provided to g_io_printf");
}

template<typename T, typename... Args>
GABE_CPP_PRINT_API void g_io_printf(const char* s, const T& value, const Args&... args)
{
	size_t strLength = strlen(s);
	_g_io_printf_internal(s, strLength, value, args...);
}

#endif // GABE_CPP_PRINT_H

// ------------------------ Implementation ------------------------
#ifdef GABE_CPP_PRINT_IMPL

#include <cppUtils/cppStrings.hpp>
#include <string>

// -------------------- Common --------------------
g_cppPrint_io g_cppPrint_stdout = {};

template<>
g_cppPrint_io& operator<<(g_cppPrint_io& io, float const&)
{
	g_io_printf("TODO: Implement float <<");
	return io;
}

template<typename T>
static void printInteger(char* buffer, size_t bufferSize, T integer)
{
	char* bufferPtr = &buffer[0] + bufferSize - 1;
	T n = integer;
	do
	{
		int valMod10 = n % 10;
		*bufferPtr = valMod10 >= 0 
			? (char)(valMod10 + '0')
			: (char)((-1 * valMod10) + '0');
		n = n / 10;
		bufferPtr--;
	} while (n != 0 && bufferPtr >= buffer);

	if (integer < 0 && bufferPtr >= buffer)
	{
		*bufferPtr = '-';
		bufferPtr--;
	}

	_g_io_printf_internal(bufferPtr + 1, (buffer + bufferSize) - bufferPtr - 1);
}

template<>
g_cppPrint_io& operator<<(g_cppPrint_io& io, int8_t const& integer)
{
	constexpr size_t bufferSize = 4;
	char buffer[bufferSize];
	printInteger(buffer, bufferSize, integer);
	return io;
}

template<>
g_cppPrint_io& operator<<(g_cppPrint_io& io, int16_t const& integer)
{
	constexpr size_t bufferSize = 6;
	char buffer[bufferSize];
	printInteger(buffer, bufferSize, integer);
	return io;
}

template<>
g_cppPrint_io& operator<<(g_cppPrint_io& io, int32_t const& integer)
{
	constexpr size_t bufferSize = 11;
	char buffer[bufferSize];
	printInteger(buffer, bufferSize, integer);
	return io;
}

template<>
g_cppPrint_io& operator<<(g_cppPrint_io& io, int64_t const& integer)
{
	constexpr size_t bufferSize = 20;
	char buffer[bufferSize];
	printInteger(buffer, bufferSize, integer);
	return io;
}

template<>
g_cppPrint_io& operator<<(g_cppPrint_io& io, uint8_t const& integer)
{
	constexpr size_t bufferSize = 3;
	char buffer[bufferSize];
	printInteger(buffer, bufferSize, integer);
	return io;
}

template<>
g_cppPrint_io& operator<<(g_cppPrint_io& io, uint16_t const& integer)
{
	constexpr size_t bufferSize = 5;
	char buffer[bufferSize];
	printInteger(buffer, bufferSize, integer);
	return io;
}

template<>
g_cppPrint_io& operator<<(g_cppPrint_io& io, uint32_t const& integer)
{
	constexpr size_t bufferSize = 10;
	char buffer[bufferSize];
	printInteger(buffer, bufferSize, integer);
	return io;
}

template<>
g_cppPrint_io& operator<<(g_cppPrint_io& io, uint64_t const& integer)
{
	UINT64_MAX;
	constexpr size_t bufferSize = 20;
	char buffer[bufferSize];
	printInteger(buffer, bufferSize, integer);
	return io;
}

template<>
g_cppPrint_io& operator<<(g_cppPrint_io& io, g_DumbString const& dumbString)
{
	_g_io_printf_internal((const char*)dumbString.str, dumbString.numBytes);
	return io;
}

template<>
g_cppPrint_io& operator<<(g_cppPrint_io& io, const char* const& s)
{
	_g_io_printf_internal((const char*)s, std::strlen((const char*)s));
	return io;
}

template<>
g_cppPrint_io& operator<<(g_cppPrint_io& io, char* const& s)
{
	_g_io_printf_internal((const char*)s, std::strlen((const char*)s));
	return io;
}

template<>
g_cppPrint_io& operator<<(g_cppPrint_io& io, std::string const& str)
{
	_g_io_printf_internal((const char*)str.c_str(), str.size());
	return io;
}

// -------------------- Platform --------------------
#ifdef _WIN32

#include <Windows.h>
#include <stdexcept>

static HANDLE stdoutHandle = NULL;

static void initializeStdoutIfNecessary()
{
	if (stdoutHandle == NULL)
	{
		stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		if (stdoutHandle == INVALID_HANDLE_VALUE)
		{
			throw std::runtime_error(("Cannot acquire a STD_OUTPUT_HANDLE. Last error: " + std::to_string(GetLastError())).c_str());
		}
		else if (stdoutHandle == NULL)
		{
			// Running on a device with no dedicated stdout, just pipe the output nowhere in this case
			throw std::runtime_error("Cannot acquire a STD_OUTPUT_HANDLE. Current device does not support stdout.");
		}
	}
}

void g_io_printf(const char* s)
{
	_g_io_printf_internal(s, std::strlen(s));
}

void _g_io_printf_internal(const char* s, size_t length)
{
	initializeStdoutIfNecessary();
	if (length <= MAXDWORD)
	{
		DWORD numBytesWritten;
		WriteFile(stdoutHandle, s, (DWORD)length, &numBytesWritten, NULL);
		if (numBytesWritten != length)
		{
			throw std::runtime_error("Failed to write to stdout. OOM or something idk.");
		}
	}
	else
	{
		std::string errorMessage = "String length is invalid '" + std::to_string(length) + "'. Must be >= 0 && <= " + std::to_string(MAXDWORD);
		throw std::runtime_error(errorMessage.c_str());
	}
}

#else // end _WIN32

void g_io_printf(const char* s)
{
	std::cout << s;
}

void _g_io_printf_internal(const char* s, size_t length)
{
	if (length >= 0 && length <= INT32_MAX)
	{
		printf("%.*s", (int)length, s);
	}
	else
	{
		std::string errorMessage = "String length is invalid '" + std::to_string(length) + "'. Must be >= 0 && <= " + std::to_string(INT32_MAX);
		throw new std::runtime_error(errorMessage.c_str());
	}
}

#endif // end PLATFORM_IMPLS

#endif // end GABE_CPP_PRINT_IMPL