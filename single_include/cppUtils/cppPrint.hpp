/*
 -------- QUICK_START --------
 Do this:
	 #define GABE_CPP_PRINT_IMPL
 before you include this file in *one* C++ file to create the implementation.

 // i.e. it should look like this in *one* source file:
 #define GABE_CPP_PRINT_IMPL
 #include "cppPrint.hpp"

 -------- QUICK_START + cppUtils.hpp --------
 To use this library in conjunction with cppUtils.hpp, make sure any #include<cppUtils/cppUtils.hpp> are formatted like:

 #define USE_GABE_CPP_PRINT
 #include <cppUtils/cppUtils.hpp>
 #undef USE_GABE_CPP_PRINT

 And in your single implementation file, make sure it's formatted like:

 #define GABE_CPP_PRINT_IMPL
 #include <cppUtils/cppPrint.hpp>
 #undef GABE_CPP_PRINT_IMPL

 #define GABE_CPP_UTILS_IMPL
 #include <cppUtils/cppUtils.hpp>

 This will ensure that all calls to g_logger_* will use this print library, allowing you to make print statements
 like `g_logger_info("{:.2f}", 3.1416f)`.



 -------- LICENSE --------

 Open Source, see end of file


 -------- DOCUMENTATION --------

 Format specification: (adapted from https://fmt.dev/latest/syntax.html)

 Any fields marked with [] are optional, whereas fields marked with () are required. If no format is specified,
 for example if you specify something like print("{}"), then it will use all the default options.

   format_spec ::= [fill](":")[align][sign]["#"][width]["." precision][type]
   fill        ::= <any ASCII character other than "{" | "}" | ":">
   align       ::= "<" | ">" | "^"
   sign        ::= "+" | "-" | " "
   width       ::= integer <max UINT16_MAX>
   precision   ::= integer <max UINT16_MAX>
   type        ::= "a" | "A" | "b" | "B" | "c" | "d" | "e" | "E" | "f" | "F" |"g" | "G" |
				   "o" | "p" | "s" | "x" | "X"

		"#": Alternate form. This will display a prefix of 0b for binary, 0x for hex, and 0c for octal.
		":": Required separator
		".": Required separator when specifying precision.
	   fill: Specifies what kind of character to fill any padded space with. Default ' '
	  align: Specifies alignment of content
			 "<" is Left-align
			 ">" is right-align
			 "^" is centered
	   sign: "+" Means always display + or - sign for numeric content. Default "-"
			 " " Means display - for negative numbers, and a padded space for positive numbers
			 "-" Means only display - for negative numbers and do nothing for positive numbers
			 Examples:
			   "+" = "+3.12" "-3.12"
			   " " = " 3.12" "-3.12"
			   "-" = "3.12" "-3.12"
	  width: Minimum width for content to fill. Max width that can be used is UINT16_MAX.
  precision: Number of digits to show after decimal place. Defaults to 6. If the number is truncated
			 it will be rounded to the next natural decimal place, for example 4.9999999 -> 5.000000
			 for a precision of 6. Max precision that can be used is UINT16_MAX.
	   type: This specifies the output format for any integral data you want to print. There are variations
			 of the types based on whether the data is floating point, integral,  or a pointer.

			 Integer flags:
			 -------------
			 default: 'd'
			 'b': Binary format. Outputs the number in base 2 with an apostrophe every 4 bits for clarity (e.g
				  1111'1010). Using the "#" will add the prefix "0b".
			 'B': Same as above, except the prefix will use "0B".
			 'c': Character format. Will output the number as an ASCII character.
			 'd': Decimal integer. Outputs the number in base 10. This is the default.
			 'o': Octal format. Outputs the number in base 8. Using the "#" will add the prefix "0c".
			 'O': Same as above, except the prefix is "0C".
			 'x': Hex format. Outputs the number in base 16 using lower-case letters 'a'-'f'. Using the
				  "#" will add the prefix "0x".
			 'X': Same as above, except it will use upper-case letters 'A'-'F', and using "#" will add the prefix "0X".

			 Integer flags:
			 -------------
			 default: 'f' with a precision of 6
			 'a': Hexadecimal floating point format. Prints the number in base 16 with lower-case letters 'a'-'f'.
				  Using "#" will prefix "0x". Uses "p" to denote the exponent.
			 'A': Same as above, except uses upper-case letters 'A'-'F' and using "#" will add the prefix "0X" and
				  "P" will denote the exponent.
			 'e': Exponent notation. Prints the number in scientific notation using the letter 'e' to denote the exponent.
			 'E': Same as above, except uses the letter 'E'.
			 'f': Fixed point. Displays the number as a fixed point number.
			 'F': Fixed point. Except nan is converted to NAN, and inf is converted to INF.

			 Pointer flags:
			 -------------
			 default: 'p'
			 'p': Pointer format. This is the default type for pointers. Prints the pointer address in hexadecimal notation
				  using 4 bytes, or 8 bytes on a 32 bit or 64 bit computer respectively.

 -------- DLL STUFF --------

 Because of the nature of this library and the amount of templated functions, I would not recommend using this library
 as a DLL or shared library. Instead, statically linking this library is recommended by compiling the code directly
 in your codebase.

 If really must use this library as part of a DLL, I have created a macro:

	GABE_CPP_PRINT_API

 that does nothing, but prefaces each forward declaration. Simply redefine this
 to your preferred __declspec(dllimport) or __declspec(dllexport) before including this
 file anywhere and it should just work out. No guarantees are made for ABI stability.

*/
#ifndef GABE_CPP_PRINT_H
#define GABE_CPP_PRINT_H

#include <string>
#include <cppUtils/cppMaybe.hpp>

// Override this define to add DLL support on your platform
#ifndef GABE_CPP_PRINT_API 
#define GABE_CPP_PRINT_API
#endif

#ifndef USE_GABE_CPP_PRINT
#define USE_GABE_CPP_PRINT
#endif

struct g_DumbString;

enum class g_io_stream_mods : uint32_t
{
	None = 0,
	PrecisionSet = 1 << 0,
	CapitalModifier = 1 << 1,
	AltFormat = 1 << 2,
};

enum class g_io_stream_paramType : uint16_t
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

enum class g_io_stream_align : uint8_t
{
	Left,
	Right,
	Center
};

enum class g_io_stream_sign : uint8_t
{
	Positive,
	Negative,
	Space
};

struct g_io_stream
{
	uint16_t precision;
	uint16_t width;
	uint32_t fillCharacter;
	/*u32*/ g_io_stream_mods mods;
	/*u16*/ g_io_stream_paramType type;
	/* u8*/ g_io_stream_align alignment;
	/* u8*/ g_io_stream_sign sign;

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
g_io_stream& operator<<(g_io_stream& io, double const& number);
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
	// NOTE: N - 1 is because we don't want to count the null byte that ends the C-String
	_g_io_print_string_formatted((const char*)s, N - 1, "", 0, io);
	return io;
}
template<>
g_io_stream& operator<<(g_io_stream& io, std::string const& str);
template<typename T, typename E>
g_io_stream& operator<<(g_io_stream& io, g_Maybe<T, E> const& maybeVal)
{
	if (maybeVal.hasValue())
	{
		io << maybeVal.value();
	}
	else
	{
		io << "<nullopt, error=" << maybeVal.error() << ">";
	}
	return io;
}

GABE_CPP_PRINT_API void _g_io_printf_internal(const char* s, size_t length);

GABE_CPP_PRINT_API void g_io_printf(const char* s);

template<typename T, typename...Args>
GABE_CPP_PRINT_API void _g_io_printf_internal(const char* s, size_t length, const T& value, const Args&... args)
{
	size_t stringStart = 0;
	size_t numBytes = 0;
	size_t fmtStart = SIZE_MAX;
	for (size_t i = 0; i < length; i++)
	{
		if (s[i] == '{' && fmtStart == SIZE_MAX)
		{
			if (i < length - 1 && s[i + 1] == '{')
			{
				_g_io_printf_internal(s + stringStart, numBytes);
				_g_io_printf_internal("{", 1);
				numBytes = 0;
				i++;
				stringStart = i + 1;
			}
			else
			{
				_g_io_printf_internal(s + stringStart, numBytes);
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
			numBytes++;
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
#include <stdexcept>

// -------------------- Common --------------------

// ------ Internal variables ------
g_io_stream g_io_stream_stdout = {
	0,
	0,
	' ',
	g_io_stream_mods::None,
	g_io_stream_paramType::None,
	g_io_stream_align::Right,
	g_io_stream_sign::Positive
};

// NOTE: Big enough to hold 64 bits (for binary representation) plus an apostrophe every 4 bits (for clarity in reading)
static constexpr size_t maxIntegerBufferSize = 81;

// ------ Internal functions ------
static void _g_io_print_string_formatted(const char* content, size_t contentLength, const char* prefix, size_t prefixLength, const g_io_stream& io);
static const char* _g_io_get_int_prefix(const g_io_stream& io);
static size_t _g_io_get_int_prefix_size(const g_io_stream& io);
static const char* _g_io_get_float_prefix(const g_io_stream& io);
static size_t _g_io_get_float_prefix_size(const g_io_stream& io);
static inline bool g_io_isWhitespace(uint32_t c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }
static inline bool g_io_isDigit(uint32_t c) { return c >= '0' && c <= '9'; }
static inline int32_t g_io_toNumber(uint32_t c) { return (int32_t)(c - '0'); }

static int32_t g_io_parseNextInteger(const char* str, size_t length, size_t* numCharactersParsed)
{
	size_t cursor = 0;

	// Keep parsing until we hit length or a non-integer-digit
	while (cursor < length && g_io_isDigit(str[cursor]))
	{
		cursor++;
	}

	*numCharactersParsed = cursor;
	if (cursor == 0)
	{
		// No digits parsed, this failed to parse a number so early exit to avoid
		// buffer overflows in the next function
		return 0;
	}

	int32_t multiplier = 1;
	int32_t result = 0;
	for (int i = (int)(cursor - 1); i >= 0; i--)
	{
		// TODO: Do something to handle overflow here
		result += (multiplier * g_io_toNumber(str[i]));
		multiplier *= 10;
	}

	return result;
}

void g_io_stream::parseModifiers(const char* modifiersStr, size_t length)
{
	// format_spec ::= [fill](":")[align][sign]["#"][width]["." precision][type]
	auto maybeParseInfo = g_parser_makeParser(modifiersStr, length);;
	if (!maybeParseInfo.hasValue())
	{
		throw std::runtime_error("Invalid UTF8 string passed to printf.");
	}

	g_ParseInfo& parseInfo = maybeParseInfo.mut_value();

	// Parse [fill](":")
	// Return early if end of string is reached with no specifiers
	uint8_t numBytesParsed;
	{
		uint32_t c = g_parser_peek(parseInfo, &numBytesParsed);
		if (c != ':' && c != '\0')
		{
			// Instead of copying the unicode codepoint, we'll copy the raw bytes
			// since they'll get decoded later in the cycle
			this->fillCharacter = 0;
			for (size_t i = 0; i < numBytesParsed; i++)
			{
				this->fillCharacter = this->fillCharacter |
					(parseInfo.utf8String[parseInfo.cursor + i] << ((numBytesParsed - i - 1) * 8));
			}
			parseInfo.cursor += numBytesParsed;
		}

		c = g_parser_peek(parseInfo, &numBytesParsed);
		if (c == ':')
		{
			// Consume ':' and move on to next parsing stage
			parseInfo.cursor += numBytesParsed;
		}
		else if (c == '\0')
		{
			// No format specified use default
			return;
		}
		else
		{
			throw std::runtime_error("Malformed printf format specifier. Missing required ':'.");
		}
	}

	// Parse [align]
	{
		uint32_t c = g_parser_peek(parseInfo, &numBytesParsed);
		switch (c)
		{
		case '<':
			this->alignment = g_io_stream_align::Left;
			parseInfo.cursor += numBytesParsed;
			break;
		case '>':
			this->alignment = g_io_stream_align::Right;
			parseInfo.cursor += numBytesParsed;
			break;
		case '^':
			this->alignment = g_io_stream_align::Center;
			parseInfo.cursor += numBytesParsed;
			break;
		default:
			break;
		}
	}

	// Parse [sign]
	{
		uint32_t c = g_parser_peek(parseInfo, &numBytesParsed);
		switch (c)
		{
		case '+':
			this->sign = g_io_stream_sign::Positive;
			parseInfo.cursor += numBytesParsed;
			break;
		case '-':
			this->sign = g_io_stream_sign::Negative;
			parseInfo.cursor += numBytesParsed;
			break;
		case ' ':
			this->sign = g_io_stream_sign::Space;
			parseInfo.cursor += numBytesParsed;
			break;
		default:
			break;
		}
	}

	// Parse ["#"] alt form shebang
	{
		if (g_parser_peek(parseInfo, &numBytesParsed) == '#')
		{
			this->mods = (g_io_stream_mods)((uint32_t)this->mods | (uint32_t)g_io_stream_mods::AltFormat);
			parseInfo.cursor += numBytesParsed;
		}
	}

	// Parse [width]
	{
		uint32_t c = g_parser_peek(parseInfo, &numBytesParsed);
		if (g_io_isDigit(c))
		{
			size_t numBytesParsedForNum;
			uint32_t parsedNumber = g_io_parseNextInteger(modifiersStr + parseInfo.cursor, length - parseInfo.cursor, &numBytesParsedForNum);
			if (parsedNumber >= UINT16_MAX)
			{
				throw std::runtime_error("Invalid format specification. Width can only be specified up to UINT16_MAX digits.");
			}
			this->width = (uint16_t)parsedNumber;
			parseInfo.cursor += numBytesParsedForNum;
		}
	}

	// Parse ["." precision]
	{
		if (g_parser_peek(parseInfo, &numBytesParsed) == '.')
		{
			this->mods = (g_io_stream_mods)((uint32_t)this->mods | (uint32_t)g_io_stream_mods::PrecisionSet);
			parseInfo.cursor += numBytesParsed;

			// Parse precision digits
			if (!g_io_isDigit(g_parser_peek(parseInfo, &numBytesParsed)))
			{
				throw std::runtime_error("Invalid format specification. \".\" must be followed by an integer to specify a precision width.");
			}

			size_t numBytesParsedForNum;
			int32_t parsedNumber = g_io_parseNextInteger(modifiersStr + parseInfo.cursor, length - parseInfo.cursor, &numBytesParsedForNum);
			if (parsedNumber >= UINT16_MAX)
			{
				throw std::runtime_error("Invalid format specification. Precision can only be up to UINT16_MAX digits in [\".\" precision] of format specifier.");
			}
			this->precision = (uint16_t)parsedNumber;
			parseInfo.cursor += numBytesParsedForNum;
		}
	}

	// Parse type
	{
		uint32_t c = g_parser_peek(parseInfo, &numBytesParsed);
		parseInfo.cursor += numBytesParsed;
		bool typeParsed = true;
		switch (c)
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
			typeParsed = false;
			break;
		}

		// Check if the modifier is capitalized
		if (typeParsed && c >= 'A' && c <= 'Z')
		{
			this->mods = (g_io_stream_mods)((uint32_t)this->mods | (uint32_t)g_io_stream_mods::CapitalModifier);
		}
	}

	// Make sure we finished parsing the whole thing, otherwise error out
	if (parseInfo.cursor < length)
	{
		throw std::runtime_error("Invalid format specifier. Expected end of format specifier \"}\" after [\"type\"], but string continued.");
	}
}

void g_io_stream::resetModifiers()
{
	this->alignment = g_io_stream_align::Right;
	this->fillCharacter = ' ';
	this->mods = g_io_stream_mods::None;
	this->precision = 0;
	this->sign = g_io_stream_sign::Positive;
	this->type = g_io_stream_paramType::None;
	this->width = 0;
}

template<typename T>
static size_t integerToString(char* const buffer, size_t bufferSize, T integer)
{
	// Handle buffer overflow while calculating the number of digits
	T posNumber = integer < 0 ? integer * -1 : integer;
	if (posNumber == std::numeric_limits<T>().min())
	{
		posNumber = std::numeric_limits<T>().max();
	}

	size_t numDigits = integer != 0
		? (size_t)floor(log10((double)posNumber)) + 1 + (integer < 0 ? 1 : 0)
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

static char halfByteToHex(uint8_t halfByte, char baseA)
{
	const char* hexMap = "0123456789abcdef";
	char hexDigit = hexMap[halfByte];
	if (hexDigit >= 'a' && hexDigit <= 'f')
	{
		hexDigit = (hexDigit - 'a') + baseA;
	}

	return hexDigit;
}

static size_t integerToHexString(char* const buffer, size_t bufferSize, void* integer, size_t integerSize, bool useCapitalChars)
{
	char baseA = useCapitalChars ? 'A' : 'a';

	char* bufferPtr = buffer;
	char* bufferEnd = (char*)buffer + bufferSize;

	bool mostSignificantBitHit = false;

	// Convert binary number to hex
	uint8_t* bytePtr = (uint8_t*)(integer)+(integerSize - 1);
	while (bytePtr >= (uint8_t*)integer && bufferPtr <= bufferEnd)
	{
		// Skip leading 0's
		if (!mostSignificantBitHit)
		{
			if (*bytePtr == 0)
			{
				bytePtr--;
				continue;
			}
			mostSignificantBitHit = true;
		}

		// TODO: Maybe we should have compile-time macros that switch this up
		//       depending on what the target platform architecture is
		// Assume machine is little-endian and print big-endian style
		*(bufferPtr++) = halfByteToHex((*bytePtr >> 4) & 0xF, baseA);

		if (bufferPtr <= bufferEnd)
		{
			*(bufferPtr++) = halfByteToHex(*bytePtr & 0xF, baseA);
		}

		bytePtr--;
	}

	return (bufferPtr - buffer);
}

static size_t dataToBinaryString(char* const buffer, size_t bufferSize, void* data, size_t dataSize)
{
	char* bufferPtr = buffer;
	char* bufferEnd = (char*)buffer + bufferSize;

	bool mostSignificantBitHit = false;

	// Convert binary number to hex
	uint8_t* bytePtr = (uint8_t*)(data)+(dataSize - 1);
	while (bytePtr >= (uint8_t*)data && bufferPtr <= bufferEnd)
	{
		// Skip leading 0's
		if (!mostSignificantBitHit)
		{
			if (*bytePtr == 0)
			{
				bytePtr--;
				continue;
			}
			mostSignificantBitHit = true;
		}

		// TODO: Maybe we should have compile-time macros that switch this up
		//       depending on what the target platform architecture is
		// Assume machine is little-endian and print big-endian style
		for (int i = 7; i >= 0; i--)
		{
			if (bufferPtr <= bufferEnd)
			{
				*(bufferPtr++) = ((*bytePtr >> i) & 0x1) == 1 ? '1' : '0';
			}

			if (bufferPtr <= bufferEnd && i == 4)
			{
				// Add apostrophes every 4 bits for clarity in reading
				*(bufferPtr++) = '\'';
			}
		}

		if (bufferPtr <= bufferEnd && bytePtr > data)
		{
			// Add apostrophes every 4 bits for clarity in reading
			*(bufferPtr++) = '\'';
		}

		bytePtr--;
	}

	return (bufferPtr - buffer);
}

template<typename T>
static size_t integerToString(char* const buffer, size_t bufferSize, T integer, g_io_stream& io)
{
	switch (io.type)
	{
	case g_io_stream_paramType::None: // Default to decimal display
		// TODO: Should we throw a runtime error here or does it really matter?
	case g_io_stream_paramType::FixedPoint: // Treat any floating point modifiers as the default state as well
	case g_io_stream_paramType::Decimal:
		return integerToString(buffer, bufferSize, integer);
	case g_io_stream_paramType::Hexadecimal:
		return integerToHexString(buffer, bufferSize, &integer, sizeof(T),
			(uint32_t)io.mods & (uint32_t)g_io_stream_mods::CapitalModifier);
	case g_io_stream_paramType::Binary:
		return dataToBinaryString(buffer, bufferSize, &integer, sizeof(T));
	}

	throw std::runtime_error("Unsupported io stream modifier in integerToString used.");
}

static size_t realNumberToString(double const& number, char* const buffer, size_t bufferSize, const g_io_stream& io, int expCutoff)
{
	switch (io.type)
	{
		// Default to fixed point
	case g_io_stream_paramType::None:
	case g_io_stream_paramType::FixedPoint:
		break;
	case g_io_stream_paramType::ExponentNotation:
		expCutoff = 0;
		break;
	case g_io_stream_paramType::GeneralFormat:
		// TODO: Arbitrary precision here
		break;
	default:
		throw std::runtime_error("Unsupported io stream modifier in realNumberToString used.");
	}

	int numDigitsAfterDecimal = io.precision;

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
	else if (number == 0.0)
	{
		// TODO: Print the appropriate number of 0's after mantissa for formatted strings
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

	bool isNegative = number < 0.0;
	double n = isNegative ? -1.0 * (double)number : (double)number;

	// Calculate magnitude
	int magnitude = (int)log10(n);

	bool useExp = magnitude > expCutoff || magnitude < -expCutoff;
	bool expSignIsPositive = magnitude > 0;
	int expNumber = 0;
	if (useExp)
	{
		double weight = pow(10.0, (double)magnitude);
		n = n / weight;
		if (n < 1.0 && n > -1.0)
		{
			n *= 10;
			magnitude--;
		}
		expNumber = magnitude;
		magnitude = 0;
	}

	if (isNegative)
	{
		*(bufferPtr++) = '-';
	}

	if (numDigitsAfterDecimal == 0)
	{
		numDigitsAfterDecimal = 6;
	}

	int parsedNumDigitsAfterDecimal = 0;
	bool passedDecimal = false;
	while (parsedNumDigitsAfterDecimal <= numDigitsAfterDecimal && buffer != bufferEnd)
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
		}
		magnitude--;
	}

	// Round if needed
	if (passedDecimal && g_io_isDigit(*(bufferPtr - 1)) && g_io_toNumber(*(bufferPtr - 1)) >= 5)
	{
		// Round all digits up by 1 until we can't
		int carry = 1;
		for (int i = (int)(bufferPtr - buffer - 2); i >= 0; i--)
		{
			if (buffer[i] == '.')
			{
				continue;
			}

			int oldNumber = g_io_toNumber(buffer[i]);
			int newNumber = oldNumber + carry;
			bool shouldBreak = oldNumber < 5;
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

	// After rounding, remove 1 digit of precision, since we added an extra digit of precision
	// just for rounding purposes
	bufferPtr--;

	if (useExp && buffer != bufferEnd)
	{
		*(bufferPtr++) = 'e';
		*(bufferPtr++) = expSignIsPositive ? '+' : '-';
		int numToPrint = expNumber < 0 ? expNumber * -1 : expNumber;
		bufferPtr += integerToString(bufferPtr, bufferEnd - bufferPtr, numToPrint);
	}

	return (bufferPtr - buffer);
}

// Adapted from https://stackoverflow.com/a/7097567
template<>
g_io_stream& operator<<(g_io_stream& io, float const& number)
{
	constexpr size_t bufferSize = 128;
	char buffer[bufferSize];
	size_t length = realNumberToString((double)number, buffer, bufferSize, io, 5);
	_g_io_print_string_formatted(buffer, length, _g_io_get_float_prefix(io), _g_io_get_float_prefix_size(io), io);
	return io;
}

template<>
g_io_stream& operator<<(g_io_stream& io, double const& number)
{
	constexpr size_t bufferSize = 128;
	char buffer[bufferSize];
	size_t length = realNumberToString(number, buffer, bufferSize, io, 5);
	_g_io_print_string_formatted(buffer, length, _g_io_get_float_prefix(io), _g_io_get_float_prefix_size(io), io);
	return io;
}

template<>
g_io_stream& operator<<(g_io_stream& io, int8_t const& integer)
{
	char buffer[maxIntegerBufferSize];
	size_t length = integerToString(buffer, maxIntegerBufferSize, integer, io);
	_g_io_print_string_formatted(buffer, length, _g_io_get_int_prefix(io), _g_io_get_int_prefix_size(io), io);
	return io;
}

template<>
g_io_stream& operator<<(g_io_stream& io, int16_t const& integer)
{
	char buffer[maxIntegerBufferSize];
	size_t length = integerToString(buffer, maxIntegerBufferSize, integer, io);
	_g_io_print_string_formatted(buffer, length, _g_io_get_int_prefix(io), _g_io_get_int_prefix_size(io), io);
	return io;
}

template<>
g_io_stream& operator<<(g_io_stream& io, int32_t const& integer)
{
	char buffer[maxIntegerBufferSize];
	size_t length = integerToString(buffer, maxIntegerBufferSize, integer, io);
	_g_io_print_string_formatted(buffer, length, _g_io_get_int_prefix(io), _g_io_get_int_prefix_size(io), io);
	return io;
}

template<>
g_io_stream& operator<<(g_io_stream& io, int64_t const& integer)
{
	char buffer[maxIntegerBufferSize];
	size_t length = integerToString(buffer, maxIntegerBufferSize, integer, io);
	_g_io_print_string_formatted(buffer, length, _g_io_get_int_prefix(io), _g_io_get_int_prefix_size(io), io);
	return io;
}

template<>
g_io_stream& operator<<(g_io_stream& io, uint8_t const& integer)
{
	char buffer[maxIntegerBufferSize];
	size_t length = integerToString(buffer, maxIntegerBufferSize, integer, io);
	_g_io_print_string_formatted(buffer, length, _g_io_get_int_prefix(io), _g_io_get_int_prefix_size(io), io);
	return io;
}

template<>
g_io_stream& operator<<(g_io_stream& io, uint16_t const& integer)
{
	char buffer[maxIntegerBufferSize];
	size_t length = integerToString(buffer, maxIntegerBufferSize, integer, io);
	_g_io_print_string_formatted(buffer, length, _g_io_get_int_prefix(io), _g_io_get_int_prefix_size(io), io);
	return io;
}

template<>
g_io_stream& operator<<(g_io_stream& io, uint32_t const& integer)
{
	char buffer[maxIntegerBufferSize];
	size_t length = integerToString(buffer, maxIntegerBufferSize, integer, io);
	_g_io_print_string_formatted(buffer, length, _g_io_get_int_prefix(io), _g_io_get_int_prefix_size(io), io);
	return io;
}

template<>
g_io_stream& operator<<(g_io_stream& io, uint64_t const& integer)
{
	char buffer[maxIntegerBufferSize];
	size_t length = integerToString(buffer, maxIntegerBufferSize, integer, io);
	_g_io_print_string_formatted(buffer, length, _g_io_get_int_prefix(io), _g_io_get_int_prefix_size(io), io);
	return io;
}

template<>
g_io_stream& operator<<(g_io_stream& io, g_DumbString const& dumbString)
{
	_g_io_print_string_formatted((const char*)dumbString.data, dumbString.numBytes, "", 0, io);
	return io;
}

template<>
g_io_stream& operator<<(g_io_stream& io, const char* const& s)
{
	_g_io_print_string_formatted((const char*)s, std::strlen((const char*)s), "", 0, io);
	return io;
}

template<>
g_io_stream& operator<<(g_io_stream& io, char* const& s)
{
	_g_io_print_string_formatted((const char*)s, std::strlen((const char*)s), "", 0, io);
	return io;
}

template<>
g_io_stream& operator<<(g_io_stream& io, std::string const& str)
{
	_g_io_print_string_formatted((const char*)str.c_str(), str.size(), "", 0, io);
	return io;
}

// ------ Internal functions ------
static void _g_io_print_string_formatted(const char* content, size_t contentLength, const char* prefix, size_t prefixLength, const g_io_stream& io)
{
	if (prefixLength > 0)
	{
		_g_io_printf_internal(prefix, prefixLength);
	}

	auto numCharsInContent = g_dumbString_utf8Length(content, contentLength);
	if (!numCharsInContent.hasValue())
	{
		throw std::runtime_error("Tried to print invalid UTF8 string in printf.");
	}
	size_t totalContentLength = prefixLength + numCharsInContent.value();
	uint32_t leftPadding = 0;
	uint32_t rightPadding = 0;
	if (io.width != 0)
	{
		switch (io.alignment)
		{
		case g_io_stream_align::Left:
		{
			if (io.width > totalContentLength)
			{
				rightPadding = (uint32_t)(io.width - totalContentLength);
			}
		}
		break;
		case g_io_stream_align::Right:
		{
			if (io.width > totalContentLength)
			{
				leftPadding = (uint32_t)(io.width - totalContentLength);
			}
		}
		break;
		case g_io_stream_align::Center:
		{
			if (io.width > totalContentLength)
			{
				leftPadding = (uint32_t)((io.width - totalContentLength) / 2);
				// Since integer division can truncate, rightPadding will just
				// take whatever's left
				rightPadding = (uint32_t)(io.width - totalContentLength - leftPadding);
			}
		}
		break;
		}
	}

	// Only allocate scratch memory if needed
	uint8_t numBytesInChar = 1;
	if (io.fillCharacter >= 0xFF'FF'FF)
	{
		numBytesInChar = 4;
	}
	else if (io.fillCharacter >= 0xFF'FF)
	{
		numBytesInChar = 3;
	}
	else if (io.fillCharacter >= 0xFF)
	{
		numBytesInChar = 2;
	}

	constexpr size_t smallStringBufferSize = 32;
	char smallStringBuffer[smallStringBufferSize];
	char* scratchMemory = (leftPadding + rightPadding) * numBytesInChar > smallStringBufferSize
		? (char*)malloc(sizeof(char) * (leftPadding + rightPadding) * numBytesInChar)
		: smallStringBuffer;
	if (leftPadding > 0)
	{
		for (size_t i = 0; i < leftPadding; i++)
		{
			for (size_t j = 0; j < numBytesInChar; j++)
			{
				scratchMemory[(i * numBytesInChar) + j] = (uint8_t)((io.fillCharacter >> ((numBytesInChar - j - 1) * 8)) & 0xFF);
			}
		}

		_g_io_printf_internal(scratchMemory, leftPadding * numBytesInChar);
	}

	_g_io_printf_internal(content, contentLength);

	if (rightPadding > 0)
	{
		for (size_t i = leftPadding; i < rightPadding; i++)
		{
			for (size_t j = 0; j < numBytesInChar; j++)
			{
				scratchMemory[i] = (uint8_t)((io.fillCharacter >> ((numBytesInChar - j - 1) * 8)) & 0xFF);
			}
		}

		_g_io_printf_internal(scratchMemory, rightPadding * numBytesInChar);
	}

	if ((leftPadding + rightPadding) * numBytesInChar > smallStringBufferSize)
	{
		free(scratchMemory);
	}
}

static const char* _g_io_get_int_prefix(const g_io_stream& io)
{
	bool isCaps = (uint32_t)io.mods & (uint32_t)g_io_stream_mods::CapitalModifier;
	switch (io.type)
	{
	case g_io_stream_paramType::Binary:
	{
		if (isCaps)
		{
			return "0B";
		}
		return "0b";
	}
	case g_io_stream_paramType::Octal:
	{
		if (isCaps)
		{
			return "0C";
		}
		return "0c";
	}
	case g_io_stream_paramType::Hexadecimal:
	{
		if (isCaps)
		{
			return "0X";
		}
		return "0x";
	}
	default:
		return "";
	}
}

static size_t _g_io_get_int_prefix_size(const g_io_stream& io)
{
	switch (io.type)
	{
	case g_io_stream_paramType::Binary:
		return sizeof("0b") - 1;
	case g_io_stream_paramType::Octal:
		return sizeof("0c") - 1;
	case g_io_stream_paramType::Hexadecimal:
		return sizeof("0x") - 1;
	default:
		return sizeof("") - 1;
	}
}

static const char* _g_io_get_float_prefix(const g_io_stream& io)
{
	bool isCaps = (uint32_t)io.mods & (uint32_t)g_io_stream_mods::CapitalModifier;
	switch (io.type)
	{
	case g_io_stream_paramType::FloatHexadecimal:
	{
		if (isCaps)
		{
			return "0X";
		}
		return "0x";
	}
	default:
		return "";
	}
}

static size_t _g_io_get_float_prefix_size(const g_io_stream& io)
{
	switch (io.type)
	{
	case g_io_stream_paramType::FloatHexadecimal:
		return sizeof("0x") - 1;
	default:
		return sizeof("") - 1;
	}
}

// -------------------- Platform --------------------
#ifdef _WIN32

#include <Windows.h>

static HANDLE stdoutHandle = NULL;
static bool writingDirectlyToConsole = false;
static constexpr int fontExistCode = 3;

int CALLBACK win32FontExistsCallback(const LOGFONTW*, const TEXTMETRICW*, DWORD, LPARAM)
{
	return fontExistCode;
}

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

		// Are we printing to the console or redirecting through a pipe?
		DWORD _mode;
		BOOL consoleModeRes = GetConsoleMode(stdoutHandle, &_mode);
		writingDirectlyToConsole = consoleModeRes != 0;

		if (writingDirectlyToConsole)
		{
			DWORD nfont = 0;
			COORD fontSize = GetConsoleFontSize(stdoutHandle, nfont);

			CONSOLE_FONT_INFOEX cfi;
			cfi.cbSize = sizeof(cfi);
			cfi.nFont = nfont;
			cfi.dwFontSize.X = fontSize.X;
			cfi.dwFontSize.Y = fontSize.Y;
			cfi.FontWeight = 400;
			cfi.FontFamily = TMPF_TRUETYPE;
			cfi.FontWeight = FW_NORMAL;

			// Try a few fonts in order of the best font for UTF8 to the worst font to use
			// if all tries fail, then the console will use the default console font which
			// will probably fail for unicode stuff.
			constexpr size_t maxFallbackFonts = 5;
			size_t fallbackIndex = 0;
			constexpr wchar_t* fontsToTry[maxFallbackFonts] = {
				L"Cascadia Mono",
				L"Cascadia Code",
				L"Consolas",
				L"NSimSun"
			};

			HDC hdc = GetDC(NULL);
			LOGFONTW logFont;
			memset(&logFont, 0, sizeof(LOGFONT));
			wcscpy_s(logFont.lfFaceName, fontsToTry[fallbackIndex++]);
			bool fontExists = EnumFontFamiliesExW(hdc, &logFont, win32FontExistsCallback, NULL, NULL) == fontExistCode;

			while (!fontExists && fallbackIndex < maxFallbackFonts)
			{
				wcscpy_s(logFont.lfFaceName, fontsToTry[fallbackIndex++]);
				fontExists = EnumFontFamiliesExW(hdc, &logFont, win32FontExistsCallback, NULL, NULL) == fontExistCode;
			}

			if (fontExists)
			{
				wcscpy_s(cfi.FaceName, fontsToTry[fallbackIndex - 1]);
				BOOL res = SetCurrentConsoleFontEx(stdoutHandle, FALSE, &cfi);
				if (!res)
				{
					throw std::runtime_error("Failed to find suitable font for console.");
				}
			}
		}

		// TODO: If we're not writing directly to console then check if the file we are writing to is
		//       new. If it is new, then write the BOM prefix for the file to specify this is UTF-8.
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
		DWORD numCharsInString = (DWORD)length;
		DWORD numCharsWritten;
		if (!writingDirectlyToConsole)
		{
			WriteFile(stdoutHandle, s, (DWORD)length, &numCharsWritten, NULL);
		}
		else
		{
			numCharsInString = MultiByteToWideChar(CP_UTF8, NULL, s, (int)length, NULL, 0);
			WCHAR* utf16String = (WCHAR*)malloc(sizeof(WCHAR) * numCharsInString);
			MultiByteToWideChar(CP_UTF8, NULL, s, (int)length, utf16String, numCharsInString);
			WriteConsoleW(stdoutHandle, utf16String, numCharsInString, &numCharsWritten, NULL);
			free(utf16String);
		}

		if (numCharsWritten != numCharsInString)
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