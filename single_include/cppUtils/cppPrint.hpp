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

enum class g_io_stream_mods
{
	            None = 0,
	    PrecisionSet = 1 << 0,
	 CapitalModifier = 1 << 1,
};

enum class g_io_stream_paramType
{
	None,
	Binary,
	Character,
	Decimal,
	Octal,
	Hexadecimal,
	FloatHexadecimal,
	ExponentNotation,
	FixedPoint,
	GeneralFormat,
	Pointer,
};

struct g_io_stream
{
	int precision;
	g_io_stream_mods mods;
	g_io_stream_paramType type;

	void parseModifiers(const char* modifiersStr, size_t length);
	void resetModifiers();
};

extern g_io_stream g_io_stream_stdout;

template<typename T>
g_io_stream& operator<<(g_io_stream& io, T const& t);

// Helper for raw string literals and char[N] arrays
template<std::size_t N>
using g_io_sizedCharArray = char[N];

// Specializations for common types
template<>
g_io_stream& operator<<(g_io_stream& io, float const& number);
template<>
g_io_stream& operator<<(g_io_stream& io, int8_t const& integer);
template<>
g_io_stream& operator<<(g_io_stream& io, int16_t const& integer);
template<>
g_io_stream& operator<<(g_io_stream& io, int32_t const& integer);
template<>
g_io_stream& operator<<(g_io_stream& io, int64_t const& integer);
template<>
g_io_stream& operator<<(g_io_stream& io, uint8_t const& integer);
template<>
g_io_stream& operator<<(g_io_stream& io, uint16_t const& integer);
template<>
g_io_stream& operator<<(g_io_stream& io, uint32_t const& integer);
template<>
g_io_stream& operator<<(g_io_stream& io, uint64_t const& integer);
template<>
g_io_stream& operator<<(g_io_stream& io, g_DumbString const& dumbString);
template<>
g_io_stream& operator<<(g_io_stream& io, const char* const& s);
template<>
g_io_stream& operator<<(g_io_stream& io, char* const& s);
template<std::size_t N>
g_io_stream& operator<<(g_io_stream& io, g_io_sizedCharArray<N> const& s)
{
	_g_io_printf_internal((const char*)s, N);
	return io;
}
template<>
g_io_stream& operator<<(g_io_stream& io, std::string const& str);

GABE_CPP_PRINT_API void _g_io_printf_internal(const char* s, size_t length);

GABE_CPP_PRINT_API void g_io_printf(const char* s);

template<typename T, typename...Args>
GABE_CPP_PRINT_API void _g_io_printf_internal(const char* s, size_t length, const T& value, const Args&... args)
{
	size_t stringStart = 0;
	size_t strLength = 0;
	size_t fmtStart = SIZE_MAX;
	for (size_t i = 0; i < length; i++)
	{
		if (s[i] == '{' && fmtStart == SIZE_MAX)
		{
			if (i < length - 1 && s[i + 1] == '{')
			{
				_g_io_printf_internal(s + stringStart, strLength);
				_g_io_printf_internal("{", 1);
				strLength = 0;
				i++;
				stringStart = i + 1;
			}
			else
			{
				_g_io_printf_internal(s + stringStart, strLength);
				fmtStart = i + 1;
			}
			continue;
		}

		if (fmtStart != SIZE_MAX)
		{
			// Closing the format string
			if (s[i] == '}')
			{
				g_io_stream_stdout.parseModifiers(s + fmtStart, i - fmtStart);
				g_io_stream_stdout << value;
				g_io_stream_stdout.resetModifiers();
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

	if (fmtStart == SIZE_MAX)
	{
		// TODO: Do we really want to throw an error for extra arguments provided to printf
		//       Do performance profiling and see if it causes any significant impact
		throw std::runtime_error("Extra arguments provided to g_io_printf");
	}
	else
	{
		throw std::runtime_error("Malformed printf statement. Unclosed bracket pair '{', '}'. No matching '}' found.");
	}
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

// -------------------- Common --------------------
g_io_stream g_io_stream_stdout = {
	0,
	g_io_stream_mods::None
};

static inline bool g_io_isWhitespace(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }
static inline bool g_io_isDigit(char c) { return c >= '0' && c <= '9'; }
static inline int32_t g_io_toNumber(char c) { return (int32_t)(c - '0'); }

static int32_t g_io_parseNextInteger(const char* str, size_t length, size_t* numCharactersParsed)
{
	size_t cursor = 0;

	// Keep parsing until we hit length or a non-integer-digit
	while (cursor < length && g_io_isDigit(str[cursor]))
	{
		cursor++;
	}

	*numCharactersParsed = cursor;

	int32_t multiplier = 1;
	int32_t result = 0;
	for (size_t i = (cursor - 1); cursor > 0; cursor--)
	{
		// TODO: Do something to handle overflow here
		result += (multiplier * g_io_toNumber(str[i]));
		multiplier *= 10;
	}

	return result;
}

void g_io_stream::parseModifiers(const char* modifiersStr, size_t length)
{
	size_t cursor = 0;

	// Parse '.' precision
	if (modifiersStr[cursor] == '.')
	{
		this->mods = (g_io_stream_mods)((uint32_t)this->mods | (uint32_t)g_io_stream_mods::PrecisionSet);
		cursor++;
	}

	// Parse precision digits
	if (g_io_isDigit(modifiersStr[cursor]))
	{
		size_t numCharsParsed;
		this->precision = g_io_parseNextInteger(modifiersStr + cursor, length - cursor, &numCharsParsed);
		cursor += numCharsParsed;
	}

	// Parse type
	switch (modifiersStr[cursor])
	{
	case 'b':
	case 'B':
		this->type = g_io_stream_paramType::Binary;
		break;
	case 'c':
		this->type = g_io_stream_paramType::Character;
		break;
	case 'd':
		this->type = g_io_stream_paramType::Decimal;
		break;
	case 'o':
		this->type = g_io_stream_paramType::Octal;
		break;
	case 'x':
	case 'X':
		this->type = g_io_stream_paramType::Hexadecimal;
		break;
	case 'a':
	case 'A':
		this->type = g_io_stream_paramType::FloatHexadecimal;
		break;
	case 'e':
	case 'E':
		this->type = g_io_stream_paramType::ExponentNotation;
		break;
	case 'f':
	case 'F':
		this->type = g_io_stream_paramType::FixedPoint;
		break;
	case 'g':
	case 'G':
		this->type = g_io_stream_paramType::GeneralFormat;
		break;
	case 'p':
		this->type = g_io_stream_paramType::Pointer;
		break;
	default:
		break;
	}

	// Check if the modifier is capitalized
	switch (modifiersStr[cursor])
	{
	case 'F':
	case 'A':
	case 'E':
	case 'G':
	case 'B':
	case 'X':
		this->mods = (g_io_stream_mods)((uint32_t)this->mods | (uint32_t)g_io_stream_mods::CapitalModifier);
		break;
	default:
		break;
	}
}

void g_io_stream::resetModifiers()
{
	this->precision = 0;
	this->mods = g_io_stream_mods::None;
}

template<typename T>
static size_t integerToString(char* buffer, size_t bufferSize, T integer)
{
	size_t numDigits = integer != 0
		? (size_t)floor(log10((double)(integer < 0 ? -1 * integer : integer)) + 1.0) + (integer < 0 ? 1 : 0)
		: 1;
	if (numDigits > bufferSize)
	{
		numDigits = bufferSize;
	}

	char* bufferPtr = (char*)buffer + ((numDigits - 1) * sizeof(char));
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

	return numDigits;
}

template<typename T>
static size_t realNumberToString(T const& number, char* const buffer, size_t bufferSize, int numDigitsAfterDecimal, int expCutoff)
{
	static_assert(std::is_floating_point<T>(), "printRealNumber only works with floating point digits.");

	// Handle special cases
	if (isinf(number))
	{
		char* bufferPtr = buffer;
		if (number > 0)
		{
			if (bufferSize >= sizeof("inf"))
			{
				*bufferPtr++ = 'i';
				*bufferPtr++ = 'n';
				*bufferPtr++ = 'f';
				return sizeof("inf") - 1;
			}

			return 0;
		}

		if (bufferSize >= sizeof("-inf"))
		{
			*bufferPtr++ = '-';
			*bufferPtr++ = 'i';
			*bufferPtr++ = 'n';
			*bufferPtr++ = 'f';
			return sizeof("-inf") - 1;
		}

		return 0;
	}
	else if (isnan(number))
	{
		if (bufferSize >= sizeof("nan"))
		{
			char* bufferPtr = buffer;
			*bufferPtr++ = 'n';
			*bufferPtr++ = 'a';
			*bufferPtr++ = 'n';
			return sizeof("nan") - 1;
		}

		return 0;
	}
	else if (number == (T)0.0f)
	{
		if (bufferSize >= sizeof("0.0"))
		{
			char* bufferPtr = buffer;
			*bufferPtr++ = '0';
			*bufferPtr++ = '.';
			*bufferPtr++ = '0';
			return sizeof("0.0") - 1;
		}

		return 0;
	}

	char* bufferPtr = &buffer[0];
	char* bufferEnd = bufferPtr + bufferSize;

	bool isNegative = number < (T)0.0f;
	double n = isNegative ? -1.0 * (double)number : (double)number;

	// Calculate magnitude
	int magnitude = (int)log10(n);

	bool useExp = magnitude > expCutoff;
	int expNumber = 0;
	if (useExp)
	{
		double weight = pow(10.0, (double)magnitude);
		n = n / weight;
		expNumber = magnitude;
		magnitude = 0;
	}

	if (isNegative)
	{
		*(bufferPtr++) = '-';
	}

	bool usePrecision = false;
	float precision = 0.000001f;
	if (numDigitsAfterDecimal == 0)
	{
		usePrecision = true;
	}

	int parsedNumDigitsAfterDecimal = 0;
	bool passedDecimal = false;
	while (((!usePrecision && parsedNumDigitsAfterDecimal < numDigitsAfterDecimal) || (usePrecision && n >= 0.0 + precision))
		&& buffer != bufferEnd)
	{
		double weight = pow(10.0, (double)magnitude);
		int digit = (int)floor(n / weight);
		n -= (digit * weight);
		
		*(bufferPtr++) = (char)('0' + digit);
		if (passedDecimal)
		{
			parsedNumDigitsAfterDecimal++;
		}

		if (magnitude == 0)
		{
			*(bufferPtr++) = '.';
			passedDecimal = true;
			if (usePrecision && buffer != bufferEnd && n < 0.0 + (double)precision)
			{
				// Add trailing 0 if it would otherwise end in a decimal.
				// So, change 2. -> 2.0
				*(bufferPtr++) = '0';
			}
		}
		magnitude--;
	}

	if (passedDecimal && g_io_isDigit(*(bufferPtr - 1)) && g_io_toNumber(*(bufferPtr - 1)) >= 5)
	{
		// Round all digits up by 1 until we can't
		int carry = 1;
		for (size_t i = bufferPtr - buffer - 1; i > 0; i--)
		{
			if (buffer[i] == '.')
			{
				continue;
			}

			int newNumber = g_io_toNumber(buffer[i]) + carry;
			bool shouldBreak = newNumber < 5;
			if (newNumber >= 10) 
			{
				carry = newNumber - 9;
				newNumber = newNumber % 10;
			}

			buffer[i] = '0' + (char)newNumber;

			if (shouldBreak)
			{
				break;
			}
		}
	}

	if (useExp && buffer != bufferEnd)
	{
		*(bufferPtr++) = 'e';
		bufferPtr += integerToString(bufferPtr, bufferEnd - bufferPtr, expNumber);
	}

	return (bufferPtr - buffer);
}

// Adapted from https://stackoverflow.com/a/7097567
template<>
g_io_stream& operator<<(g_io_stream& io, float const& number)
{
	constexpr size_t bufferSize = 256;
	char buffer[bufferSize];
	size_t length = realNumberToString(number, buffer, bufferSize, io.precision, 5);
	_g_io_printf_internal(buffer, length);
	return io;
}

template<>
g_io_stream& operator<<(g_io_stream& io, int8_t const& integer)
{
	constexpr size_t bufferSize = 4;
	char buffer[bufferSize];
	size_t length = integerToString(buffer, bufferSize, integer);
	_g_io_printf_internal(buffer, length);
	return io;
}

template<>
g_io_stream& operator<<(g_io_stream& io, int16_t const& integer)
{
	constexpr size_t bufferSize = 6;
	char buffer[bufferSize];
	size_t length = integerToString(buffer, bufferSize, integer);
	_g_io_printf_internal(buffer, length);
	return io;
}

template<>
g_io_stream& operator<<(g_io_stream& io, int32_t const& integer)
{
	constexpr size_t bufferSize = 11;
	char buffer[bufferSize];
	size_t length = integerToString(buffer, bufferSize, integer);
	_g_io_printf_internal(buffer, length);
	return io;
}

template<>
g_io_stream& operator<<(g_io_stream& io, int64_t const& integer)
{
	constexpr size_t bufferSize = 20;
	char buffer[bufferSize];
	size_t length = integerToString(buffer, bufferSize, integer);
	_g_io_printf_internal(buffer, length);
	return io;
}

template<>
g_io_stream& operator<<(g_io_stream& io, uint8_t const& integer)
{
	constexpr size_t bufferSize = 3;
	char buffer[bufferSize];
	size_t length = integerToString(buffer, bufferSize, integer);
	_g_io_printf_internal(buffer, length);
	return io;
}

template<>
g_io_stream& operator<<(g_io_stream& io, uint16_t const& integer)
{
	constexpr size_t bufferSize = 5;
	char buffer[bufferSize];
	size_t length = integerToString(buffer, bufferSize, integer);
	_g_io_printf_internal(buffer, length);
	return io;
}

template<>
g_io_stream& operator<<(g_io_stream& io, uint32_t const& integer)
{
	constexpr size_t bufferSize = 10;
	char buffer[bufferSize];
	size_t length = integerToString(buffer, bufferSize, integer);
	_g_io_printf_internal(buffer, length);
	return io;
}

template<>
g_io_stream& operator<<(g_io_stream& io, uint64_t const& integer)
{
	UINT64_MAX;
	constexpr size_t bufferSize = 20;
	char buffer[bufferSize];
	size_t length = integerToString(buffer, bufferSize, integer);
	_g_io_printf_internal(buffer, length);
	return io;
}

template<>
g_io_stream& operator<<(g_io_stream& io, g_DumbString const& dumbString)
{
	_g_io_printf_internal((const char*)dumbString.str, dumbString.numBytes);
	return io;
}

template<>
g_io_stream& operator<<(g_io_stream& io, const char* const& s)
{
	_g_io_printf_internal((const char*)s, std::strlen((const char*)s));
	return io;
}

template<>
g_io_stream& operator<<(g_io_stream& io, char* const& s)
{
	_g_io_printf_internal((const char*)s, std::strlen((const char*)s));
	return io;
}

template<>
g_io_stream& operator<<(g_io_stream& io, std::string const& str)
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