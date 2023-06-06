/*
 -------- QUICK_START --------
 This is to be used in conjunction with the other source files, to use it do the following code snippet
 in *one* C++ file to create the implementation.

 // i.e. it should look like this in *one* source file:
 // The order of the includes is important, keep it in this order and you'll be fine
 #define GABE_CPP_PRINT_IMPL
 #include <cppUtils/cppPrint.hpp>
 #undef GABE_CPP_PRINT_IMPL
 
 #define GABE_CPP_UTILS_IMPL
 #include <cppUtils/cppUtils.hpp>
 #undef GABE_CPP_UTILS_IMPL
 
 #define GABE_CPP_TESTS_IMPL
 #include <cppUtils/cppTests.hpp>
 #undef GABE_CPP_TESTS_IMPL
 
 #define GABE_CPP_STRING_IMPL
 #include <cppUtils/cppStrings.hpp>
 #undef GABE_CPP_STRING_IMPL

 Then you can include it anywhere else you please.

 This works nicely with cppUtils.hpp. All calls to g_logger_* will use this print library,
 allowing you to make print statements like `g_logger_info("{:.2f}", 3.1416f)`.



 -------- LICENSE --------

 Open Source


 -------- DOCUMENTATION --------

 Format specification: (adapted from https://fmt.dev/latest/syntax.html)

 Any fields marked with [] are optional, whereas fields marked with () are required. If no format is specified,
 for example if you specify something like print("{}"), then it will use all the default options.

   format_spec ::= [fill](":")[align][sign]["#"]("0")[width]["." precision][type]
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
			 NOTE: Preceding the width field with a 0 will add 0's to the padding for hexadecimal/binary/octal
			       digits after the prefix. It will also add the 0's after the sign for numbers.
			 NOTE: Preceding the width with a "0" means the fill character will have no effect.
			 NOTE: Preceding the width with a "0" forces the alignment to be right-aligned.
			       "0X000ABCD" for {:#09X} vs "   0XABCD" for {:#9X}

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
#include <filesystem>
#include <stdexcept>
#include <cppUtils/cppMaybe.hpp>

// Override this define to add DLL support on your platform
#ifndef GABE_CPP_PRINT_API 
#define GABE_CPP_PRINT_API
#endif

#ifndef USE_GABE_CPP_PRINT
#define USE_GABE_CPP_PRINT
#endif

namespace CppUtils { struct Stream; }

template<typename T>
CppUtils::Stream& operator<<(CppUtils::Stream& io, T const& t);

namespace CppUtils
{

struct BasicString;

// Structures/enumerations live inside CppUtils scope
enum class StreamMods : uint32_t
{
	None = 0,
	PrecisionSet = 1 << 0,
	CapitalModifier = 1 << 1,
	AltFormat = 1 << 2,
	UseZeroPadding = 1 << 3,
};

enum class StreamParamType : uint16_t
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

enum class StreamAlign : uint8_t
{
	Left,
	Right,
	Center
};

enum class StreamSign : uint8_t
{
	Positive,
	Negative,
	Space
};

struct Stream
{
	uint16_t precision;
	uint16_t width;
	uint32_t fillCharacter;
	/*u32*/ StreamMods mods;
	/*u16*/ StreamParamType type;
	/* u8*/ StreamAlign alignment;
	/* u8*/ StreamSign sign;

	void parseModifiers(const char* modifiersStr, size_t length);
	void resetModifiers();
};

enum ConsoleColor
{
	BLACK,
	DARKBLUE,
	DARKGREEN,
	DARKCYAN,
	DARKRED,
	DARKMAGENTA,
	DARKYELLOW,
	DARKGRAY,
	GRAY,
	BLUE,
	GREEN,
	CYAN,
	RED,
	MAGENTA,
	YELLOW,
	WHITE,
};

namespace IO
{

extern Stream stdoutStream;

GABE_CPP_PRINT_API void _printfInternal(const char* s, size_t length);
GABE_CPP_PRINT_API void printFormattedString(const char* content, size_t contentLength, const char* prefix, size_t prefixLength, const Stream& io, bool isPositive = false);

GABE_CPP_PRINT_API void printf(const char* s);

GABE_CPP_PRINT_API void setColor(ConsoleColor background, ConsoleColor foreground);
GABE_CPP_PRINT_API void setBackgroundColor(ConsoleColor background);
GABE_CPP_PRINT_API void setForegroundColor(ConsoleColor foreground);
GABE_CPP_PRINT_API void resetColor();

template<typename T, typename...Args>
GABE_CPP_PRINT_API void _printfInternal(const char* s, size_t length, const T& value, const Args&... args)
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
				_printfInternal(s + stringStart, numBytes);
				_printfInternal("{", 1);
				numBytes = 0;
				i++;
				stringStart = i + 1;
			}
			else
			{
				_printfInternal(s + stringStart, numBytes);
				fmtStart = i + 1;
			}
			continue;
		}

		if (fmtStart != SIZE_MAX)
		{
			// Closing the format string
			if (s[i] == '}')
			{
				stdoutStream.parseModifiers(s + fmtStart, i - fmtStart);
				stdoutStream << value;
				stdoutStream.resetModifiers();
				// NOTE: Recursively call the variadic template argument, with unpacked variables
				//       The base case, where there is no args... left is handled in the function
				//       above
				if (length >= i + 1)
				{
					return _printfInternal((s + i + 1), length - i - 1, args...);
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
		throw std::runtime_error("Extra arguments provided to printf");
	}
	else
	{
		throw std::runtime_error("Malformed printf statement. Unclosed bracket pair '{', '}'. No matching '}' found.");
	}
}

template<typename T, typename... Args>
GABE_CPP_PRINT_API void printf(const char* s, const T& value, const Args&... args)
{
	size_t strLength = strlen(s);
	_printfInternal(s, strLength, value, args...);
}

}
} // End CppUtils::io

// Helper for raw string literals and char[N] arrays
template<std::size_t N>
using SizedCharArray = char[N];

// Specializations for common types
template<typename T>
CppUtils::Stream& operator<<(CppUtils::Stream& io, T* const& c)
{
	CppUtils::IO::printf("{:#018p}", (uint64_t)(void*)(c));
	return io;
}
template<>
CppUtils::Stream& operator<<(CppUtils::Stream& io, float const& number);
template<>
CppUtils::Stream& operator<<(CppUtils::Stream& io, double const& number);
template<>
CppUtils::Stream& operator<<(CppUtils::Stream& io, int8_t const& integer);
template<>
CppUtils::Stream& operator<<(CppUtils::Stream& io, int16_t const& integer);
template<>
CppUtils::Stream& operator<<(CppUtils::Stream& io, int32_t const& integer);
template<>
CppUtils::Stream& operator<<(CppUtils::Stream& io, long const& integer);
template<>
CppUtils::Stream& operator<<(CppUtils::Stream& io, int64_t const& integer);
template<>
CppUtils::Stream& operator<<(CppUtils::Stream& io, uint8_t const& integer);
template<>
CppUtils::Stream& operator<<(CppUtils::Stream& io, uint16_t const& integer);
template<>
CppUtils::Stream& operator<<(CppUtils::Stream& io, uint32_t const& integer);
template<>
CppUtils::Stream& operator<<(CppUtils::Stream& io, unsigned long const& integer);
template<>
CppUtils::Stream& operator<<(CppUtils::Stream& io, uint64_t const& integer);
template<>
CppUtils::Stream& operator<<(CppUtils::Stream& io, CppUtils::BasicString const& dumbString);
template<>
CppUtils::Stream& operator<<(CppUtils::Stream& io, const char* const& s);
template<>
CppUtils::Stream& operator<<(CppUtils::Stream& io, unsigned char* const& s);
template<>
CppUtils::Stream& operator<<(CppUtils::Stream& io, char* const& s);
template<std::size_t N>
CppUtils::Stream& operator<<(CppUtils::Stream& io, SizedCharArray<N> const& s)
{
	// NOTE: N - 1 is because we don't want to count the null byte that ends the C-String
	CppUtils::IO::printFormattedString((const char*)s, N - 1, "", 0, io);
	return io;
}
template<>
CppUtils::Stream& operator<<(CppUtils::Stream& io, std::string const& str);
template<>
CppUtils::Stream& operator<<(CppUtils::Stream& io, std::filesystem::path const& str);
template<typename T, typename E>
CppUtils::Stream& operator<<(CppUtils::Stream& io, CppUtils::Maybe<T, E> const& maybeVal)
{
	if (maybeVal.hasValue())
	{
		io << maybeVal.value();
	}
	else
	{
		io << "<Maybe=Error, '" << maybeVal.error() << "'>";
	}
	return io;
}
template<>
CppUtils::Stream& operator<<(CppUtils::Stream& io, CppUtils::ConsoleColor const& color);
template<>
CppUtils::Stream& operator<<(CppUtils::Stream& io, char const& c);

#endif // GABE_CPP_PRINT_H

// ------------------------ Implementation ------------------------
#ifdef GABE_CPP_PRINT_IMPL
#include <cppUtils/cppStrings.hpp>

namespace CppUtils
{
namespace IO
{
// -------------------- Common --------------------

// ------ Internal variables ------
Stream stdoutStream = {
	0,
	0,
	' ',
	StreamMods::None,
	StreamParamType::None,
	StreamAlign::Right,
	StreamSign::Positive
};

// NOTE: Big enough to hold 64 bits (for binary representation) plus an apostrophe every 4 bits (for clarity in reading)
static constexpr size_t maxIntegerBufferSize = 81;

// ------ Internal functions ------
static const char* getIntPrefix(const Stream& io);
static size_t getIntPrefixSize(const Stream& io);
static const char* getFloatPrefix(const Stream& io);
static size_t getFloatPrefixSize(const Stream& io);
static inline bool isWhitespace(uint32_t c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }
static inline bool isDigit(uint32_t c) { return c >= '0' && c <= '9'; }
static inline int32_t toNumber(uint32_t c) { return (int32_t)(c - '0'); }
static int32_t parseNextInteger(const char* str, size_t length, size_t* numCharactersParsed);
} // End io

// ------ Stream ------
void Stream::parseModifiers(const char* modifiersStr, size_t length)
{
	// format_spec ::= [fill](":")[align][sign]["#"][width]["." precision][type]
	auto maybeParseInfo = Parser::makeParseInfo(modifiersStr, length);
	if (!maybeParseInfo.hasValue())
	{
		throw std::runtime_error("Invalid UTF8 string passed to printf.");
	}

	ParseInfo& parseInfo = maybeParseInfo.mut_value();

	// Parse [fill](":")
	// Return early if end of string is reached with no specifiers
	uint8_t numBytesParsed;
	{
		auto c = Parser::peek(parseInfo, &numBytesParsed);
		if (!c.hasValue())
		{
			throw std::runtime_error("Invalid UTF8 string passed to printf.");
		}

		if (*c != ':' && *c != '\0')
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

		c = Parser::peek(parseInfo, &numBytesParsed);
		if (!c.hasValue())
		{
			throw std::runtime_error("Invalid UTF8 string passed to printf.");
		}

		if (*c == ':')
		{
			// Consume ':' and move on to next parsing stage
			parseInfo.cursor += numBytesParsed;
		}
		else if (*c == '\0')
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
		auto c = Parser::peek(parseInfo, &numBytesParsed);
		if (!c.hasValue())
		{
			throw std::runtime_error("Invalid UTF8 string passed to printf.");
		}

		switch (*c)
		{
		case '<':
			this->alignment = StreamAlign::Left;
			parseInfo.cursor += numBytesParsed;
			break;
		case '>':
			this->alignment = StreamAlign::Right;
			parseInfo.cursor += numBytesParsed;
			break;
		case '^':
			this->alignment = StreamAlign::Center;
			parseInfo.cursor += numBytesParsed;
			break;
		default:
			break;
		}
	}

	// Parse [sign]
	{
		auto c = Parser::peek(parseInfo, &numBytesParsed);
		if (!c.hasValue())
		{
			throw std::runtime_error("Invalid UTF8 string passed to printf.");
		}

		switch (*c)
		{
		case '+':
			this->sign = StreamSign::Positive;
			parseInfo.cursor += numBytesParsed;
			break;
		case '-':
			this->sign = StreamSign::Negative;
			parseInfo.cursor += numBytesParsed;
			break;
		case ' ':
			this->sign = StreamSign::Space;
			parseInfo.cursor += numBytesParsed;
			break;
		default:
			break;
		}
	}

	// Parse ["#"] alt form shebang
	{
		auto c = Parser::peek(parseInfo, &numBytesParsed);
		if (!c.hasValue())
		{
			throw std::runtime_error("Invalid UTF8 string passed to printf.");
		}

		if (*c == '#')
		{
			this->mods = (StreamMods)((uint32_t)this->mods | (uint32_t)StreamMods::AltFormat);
			parseInfo.cursor += numBytesParsed;
		}
	}

	// Parse ("0")[width]
	{
		auto c = Parser::peek(parseInfo, &numBytesParsed);
		if (!c.hasValue())
		{
			throw std::runtime_error("Invalid UTF8 string passed to printf.");
		}

		if (*c == '0')
		{
			this->mods = (StreamMods)((uint32_t)this->mods | (uint32_t)StreamMods::UseZeroPadding);
			parseInfo.cursor += numBytesParsed;
		}

		c = Parser::peek(parseInfo, &numBytesParsed);
		if (!c.hasValue())
		{
			throw std::runtime_error("Invalid UTF8 string passed to printf.");
		}

		if (IO::isDigit(*c))
		{
			size_t numBytesParsedForNum;
			uint32_t parsedNumber = IO::parseNextInteger(modifiersStr + parseInfo.cursor, length - parseInfo.cursor, &numBytesParsedForNum);
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
		auto c = Parser::peek(parseInfo, &numBytesParsed);
		if (!c.hasValue())
		{
			throw std::runtime_error("Invalid UTF8 string passed to printf.");
		}

		if (*c == '.')
		{
			this->mods = (StreamMods)((uint32_t)this->mods | (uint32_t)StreamMods::PrecisionSet);
			parseInfo.cursor += numBytesParsed;

			// Parse precision digits
			c = Parser::peek(parseInfo, &numBytesParsed);
			if (!c.hasValue())
			{
				throw std::runtime_error("Invalid UTF8 string passed to printf.");
			}

			if (!IO::isDigit(*c))
			{
				throw std::runtime_error("Invalid format specification. \".\" must be followed by an integer to specify a precision width.");
			}

			size_t numBytesParsedForNum;
			int32_t parsedNumber = IO::parseNextInteger(modifiersStr + parseInfo.cursor, length - parseInfo.cursor, &numBytesParsedForNum);
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
		auto c = Parser::peek(parseInfo, &numBytesParsed);
		if (!c.hasValue())
		{
			throw std::runtime_error("Invalid UTF8 string passed to printf.");
		}

		parseInfo.cursor += numBytesParsed;
		bool typeParsed = true;
		switch (*c)
		{
		case 'b':
		case 'B':
			this->type = StreamParamType::Binary;
			break;
		case 'c':
			this->type = StreamParamType::Character;
			break;
		case 'd':
			this->type = StreamParamType::Decimal;
			break;
		case 'o':
			this->type = StreamParamType::Octal;
			break;
		case 'x':
		case 'X':
			this->type = StreamParamType::Hexadecimal;
			break;
		case 'a':
		case 'A':
			this->type = StreamParamType::FloatHexadecimal;
			break;
		case 'e':
		case 'E':
			this->type = StreamParamType::ExponentNotation;
			break;
		case 'f':
		case 'F':
			this->type = StreamParamType::FixedPoint;
			break;
		case 'g':
		case 'G':
			this->type = StreamParamType::GeneralFormat;
			break;
		case 'p':
			this->type = StreamParamType::Pointer;
			break;
		default:
			typeParsed = false;
			break;
		}

		// Check if the modifier is capitalized
		if (typeParsed && *c >= 'A' && *c <= 'Z')
		{
			this->mods = (StreamMods)((uint32_t)this->mods | (uint32_t)StreamMods::CapitalModifier);
		}
	}

	// Make sure we finished parsing the whole thing, otherwise error out
	if (parseInfo.cursor < length)
	{
		throw std::runtime_error("Invalid format specifier. Expected end of format specifier \"}\" after [\"type\"], but string continued.");
	}
}

void Stream::resetModifiers()
{
	this->alignment = StreamAlign::Right;
	this->fillCharacter = ' ';
	this->mods = StreamMods::None;
	this->precision = 0;
	this->sign = StreamSign::Negative;
	this->type = StreamParamType::None;
	this->width = 0;
}

} // End CppUtils

using namespace CppUtils;

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
static size_t integerToString(char* const buffer, size_t bufferSize, T integer, Stream& io)
{
	switch (io.type)
	{
	case StreamParamType::None: // Default to decimal display
		// TODO: Should we throw a runtime error here or does it really matter?
	case StreamParamType::FixedPoint: // Treat any floating point modifiers as the default state as well
	case StreamParamType::Decimal:
		return integerToString(buffer, bufferSize, integer);
	case StreamParamType::Hexadecimal:
	case StreamParamType::Pointer:
		return integerToHexString(buffer, bufferSize, &integer, sizeof(T),
			(uint32_t)io.mods & (uint32_t)StreamMods::CapitalModifier);
	case StreamParamType::Binary:
		return dataToBinaryString(buffer, bufferSize, &integer, sizeof(T));
	}

	throw std::runtime_error("Unsupported io stream modifier in integerToString used.");
}

static size_t realNumberToString(double const& number, char* const buffer, size_t bufferSize, const Stream& io, int expCutoff)
{
	switch (io.type)
	{
		// Default to fixed point
	case StreamParamType::None:
	case StreamParamType::FixedPoint:
		break;
	case StreamParamType::ExponentNotation:
		expCutoff = 0;
		break;
	case StreamParamType::GeneralFormat:
		// TODO: Arbitrary precision here
		break;
	default:
		throw std::runtime_error("Unsupported io stream modifier in realNumberToString used.");
	}

	int numDigitsAfterDecimal = io.precision;

	// Handle special cases
	if (isinf(number))
	{
		char baseA = ((uint8_t)io.mods & (uint8_t)StreamMods::CapitalModifier) ? 'A' : 'a';
		char* bufferPtr = buffer;
		if (number > 0)
		{
			if (bufferSize >= sizeof("inf"))
			{
				*bufferPtr++ = ('i' - 'a') + baseA;
				*bufferPtr++ = ('n' - 'a') + baseA;
				*bufferPtr++ = ('f' - 'a') + baseA;
				return sizeof("inf") - 1;
			}

			return 0;
		}

		if (bufferSize >= sizeof("-inf"))
		{
			*bufferPtr++ = '-';
			*bufferPtr++ = ('i' - 'a') + baseA;
			*bufferPtr++ = ('n' - 'a') + baseA;
			*bufferPtr++ = ('f' - 'a') + baseA;
			return sizeof("-inf") - 1;
		}

		return 0;
	}
	else if (isnan(number))
	{
		char baseA = ((uint8_t)io.mods & (uint8_t)StreamMods::CapitalModifier) ? 'A' : 'a';
		if (bufferSize >= sizeof("nan"))
		{
			char* bufferPtr = buffer;
			*bufferPtr++ = ('n' - 'a') + baseA;
			*bufferPtr++ = ('a' - 'a') + baseA;
			*bufferPtr++ = ('n' - 'a') + baseA;
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

	if (magnitude < 0)
	{
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
	if (passedDecimal && IO::isDigit(*(bufferPtr - 1)) && IO::toNumber(*(bufferPtr - 1)) >= 5)
	{
		// Round all digits up by 1 until we can't
		int carry = 1;
		for (int i = (int)(bufferPtr - buffer - 2); i >= 0; i--)
		{
			if (buffer[i] == '.')
			{
				continue;
			}

			int oldNumber = IO::toNumber(buffer[i]);
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
Stream& operator<<(Stream& io, float const& number)
{
	constexpr size_t bufferSize = 128;
	char buffer[bufferSize];
	size_t length = realNumberToString((double)number, buffer, bufferSize, io, 5);
	IO::printFormattedString(buffer, length, IO::getFloatPrefix(io), IO::getFloatPrefixSize(io), io, number > 0);
	return io;
}

template<>
Stream& operator<<(Stream& io, double const& number)
{
	constexpr size_t bufferSize = 128;
	char buffer[bufferSize];
	size_t length = realNumberToString(number, buffer, bufferSize, io, 5);
	IO::printFormattedString(buffer, length, IO::getFloatPrefix(io), IO::getFloatPrefixSize(io), io, number > 0);
	return io;
}

template<>
Stream& operator<<(Stream& io, int8_t const& integer)
{
	char buffer[IO::maxIntegerBufferSize];
	size_t length = integerToString(buffer, IO::maxIntegerBufferSize, integer, io);
	IO::printFormattedString(buffer, length, IO::getIntPrefix(io), IO::getIntPrefixSize(io), io, integer > 0);
	return io;
}

template<>
Stream& operator<<(Stream& io, int16_t const& integer)
{
	char buffer[IO::maxIntegerBufferSize];
	size_t length = integerToString(buffer, IO::maxIntegerBufferSize, integer, io);
	IO::printFormattedString(buffer, length, IO::getIntPrefix(io), IO::getIntPrefixSize(io), io, integer > 0);
	return io;
}

template<>
Stream& operator<<(Stream& io, int32_t const& integer)
{
	char buffer[IO::maxIntegerBufferSize];
	size_t length = integerToString(buffer, IO::maxIntegerBufferSize, integer, io);
	IO::printFormattedString(buffer, length, IO::getIntPrefix(io), IO::getIntPrefixSize(io), io, integer > 0);
	return io;
}

template<>
CppUtils::Stream& operator<<(CppUtils::Stream& io, long const& integer)
{
	char buffer[IO::maxIntegerBufferSize];
	size_t length = integerToString(buffer, IO::maxIntegerBufferSize, integer, io);
	IO::printFormattedString(buffer, length, IO::getIntPrefix(io), IO::getIntPrefixSize(io), io, integer > 0);
	return io;
}

template<>
Stream& operator<<(Stream& io, int64_t const& integer)
{
	char buffer[IO::maxIntegerBufferSize];
	size_t length = integerToString(buffer, IO::maxIntegerBufferSize, integer, io);
	IO::printFormattedString(buffer, length, IO::getIntPrefix(io), IO::getIntPrefixSize(io), io, integer > 0);
	return io;
}

template<>
Stream& operator<<(Stream& io, uint8_t const& integer)
{
	char buffer[IO::maxIntegerBufferSize];
	size_t length = integerToString(buffer, IO::maxIntegerBufferSize, integer, io);
	IO::printFormattedString(buffer, length, IO::getIntPrefix(io), IO::getIntPrefixSize(io), io, integer > 0);
	return io;
}

template<>
Stream& operator<<(Stream& io, uint16_t const& integer)
{
	char buffer[IO::maxIntegerBufferSize];
	size_t length = integerToString(buffer, IO::maxIntegerBufferSize, integer, io);
	IO::printFormattedString(buffer, length, IO::getIntPrefix(io), IO::getIntPrefixSize(io), io, integer > 0);
	return io;
}

template<>
Stream& operator<<(Stream& io, uint32_t const& integer)
{
	char buffer[IO::maxIntegerBufferSize];
	size_t length = integerToString(buffer, IO::maxIntegerBufferSize, integer, io);
	IO::printFormattedString(buffer, length, IO::getIntPrefix(io), IO::getIntPrefixSize(io), io, integer > 0);
	return io;
}

template<>
CppUtils::Stream& operator<<(CppUtils::Stream& io, unsigned long const& integer) 
{
	char buffer[IO::maxIntegerBufferSize];
	size_t length = integerToString(buffer, IO::maxIntegerBufferSize, integer, io);
	IO::printFormattedString(buffer, length, IO::getIntPrefix(io), IO::getIntPrefixSize(io), io, integer > 0);
	return io;
}

template<>
Stream& operator<<(Stream& io, uint64_t const& integer)
{
	char buffer[IO::maxIntegerBufferSize];
	size_t length = integerToString(buffer, IO::maxIntegerBufferSize, integer, io);
	IO::printFormattedString(buffer, length, IO::getIntPrefix(io), IO::getIntPrefixSize(io), io, integer > 0);
	return io;
}

template<>
Stream& operator<<(Stream& io, BasicString const& dumbString)
{
	IO::printFormattedString((const char*)dumbString.data, dumbString.numBytes, "", 0, io);
	return io;
}

template<>
CppUtils::Stream& operator<<(CppUtils::Stream& io, CppUtils::ConsoleColor const& color)
{
	switch (color)
	{
	case BLACK:
		io << "<Black>";
		break;
	case DARKBLUE:
		io << "<DarkBlue>";
		break;
	case DARKGREEN:
		io << "<DarkGreen>";
		break;
	case DARKCYAN:
		io << "<DarkCyan>";
		break;
	case DARKRED:
		io << "<DarkRed>";
		break;
	case DARKMAGENTA:
		io << "<DarkMagenta>";
		break;
	case DARKYELLOW:
		io << "<DarkYellow>";
		break;
	case DARKGRAY:
		io << "<DarkGray>";
		break;
	case GRAY:
		io << "<Gray>";
		break;
	case BLUE:
		io << "<Blue>";
		break;
	case GREEN:
		io << "<Green>";
		break;
	case CYAN:
		io << "<Cyan>";
		break;
	case RED:
		io << "<Red>";
		break;
	case MAGENTA:
		io << "<Magenta>";
		break;
	case YELLOW:
		io << "<Yellow>";
		break;
	case WHITE:
		io << "<White>";
		break;
	default:
		io << "<UnknownConsoleColor>";
		break;
	}

	return io;
}

template<>
CppUtils::Stream& operator<<(CppUtils::Stream& io, char const& c)
{
	IO::printFormattedString((const char*)(&c), 1, "", 0, io);
	return io;
}

template<>
Stream& operator<<(Stream& io, const char* const& s)
{
	IO::printFormattedString((const char*)s, std::strlen((const char*)s), "", 0, io);
	return io;
}

template<>
Stream& operator<<(Stream& io, unsigned char* const& s)
{
	IO::printFormattedString((const char*)s, std::strlen((const char*)s), "", 0, io);
	return io;
}

template<>
Stream& operator<<(Stream& io, char* const& s)
{
	IO::printFormattedString((const char*)s, std::strlen((const char*)s), "", 0, io);
	return io;
}

template<>
Stream& operator<<(Stream& io, std::string const& str)
{
	IO::printFormattedString((const char*)str.c_str(), str.size(), "", 0, io);
	return io;
}

template<>
CppUtils::Stream& operator<<(CppUtils::Stream& io, std::filesystem::path const& str)
{
	IO::printFormattedString((const char*)str.string().c_str(), str.string().size(), "", 0, io);
	return io;
}

namespace CppUtils
{
namespace IO
{

// ------ Internal functions ------
static const char* getIntPrefix(const Stream& io)
{
	if (!((uint32_t)io.mods & (uint32_t)StreamMods::AltFormat))
	{
		return "";
	}

	bool isCaps = (uint32_t)io.mods & (uint32_t)StreamMods::CapitalModifier;
	switch (io.type)
	{
	case StreamParamType::Binary:
	{
		if (isCaps)
		{
			return "0B";
		}
		return "0b";
	}
	case StreamParamType::Octal:
	{
		if (isCaps)
		{
			return "0C";
		}
		return "0c";
	}
	case StreamParamType::Hexadecimal:
	{
		if (isCaps)
		{
			return "0X";
		}
		return "0x";
	}
	case StreamParamType::Pointer:
		return "0x";
	default:
		return "";
	}
}

static size_t getIntPrefixSize(const Stream& io)
{
	if (!((uint32_t)io.mods & (uint32_t)StreamMods::AltFormat))
	{
		return sizeof("") - 1;
	}

	switch (io.type)
	{
	case StreamParamType::Binary:
		return sizeof("0b") - 1;
	case StreamParamType::Octal:
		return sizeof("0c") - 1;
	case StreamParamType::Hexadecimal:
		return sizeof("0x") - 1;
	case StreamParamType::Pointer:
		return sizeof("0x") - 1;
	default:
		return sizeof("") - 1;
	}
}

static const char* getFloatPrefix(const Stream& io)
{
	if (!((uint32_t)io.mods & (uint32_t)StreamMods::AltFormat))
	{
		return "";
	}

	bool isCaps = (uint32_t)io.mods & (uint32_t)StreamMods::CapitalModifier;
	switch (io.type)
	{
	case StreamParamType::FloatHexadecimal:
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

static size_t getFloatPrefixSize(const Stream& io)
{
	if (!((uint32_t)io.mods & (uint32_t)StreamMods::AltFormat))
	{
		return sizeof("") - 1;
	}

	switch (io.type)
	{
	case StreamParamType::FloatHexadecimal:
		return sizeof("0x") - 1;
	default:
		return sizeof("") - 1;
	}
}

static int32_t parseNextInteger(const char* str, size_t length, size_t* numCharactersParsed)
{
	size_t cursor = 0;

	// Keep parsing until we hit length or a non-integer-digit
	while (cursor < length && isDigit(str[cursor]))
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
		result += (multiplier * toNumber(str[i]));
		multiplier *= 10;
	}

	return result;
}

}
} // End CppUtils::io

// -------------------- Platform --------------------
#ifdef _WIN32

#include <Windows.h>

namespace CppUtils
{
namespace IO
{

static HANDLE stdoutHandle = NULL;
static bool writingDirectlyToConsole = false;
static constexpr int fontExistCode = 3;
static CONSOLE_SCREEN_BUFFER_INFOEX originalConsoleInfo = {};

static WORD consoleColorToForegroundColor(ConsoleColor color)
{
	switch (color)
	{
	case BLACK:
		return 0;
	case DARKBLUE:
		return FOREGROUND_BLUE;
	case DARKGREEN:
		return FOREGROUND_GREEN;
	case DARKCYAN:
		return FOREGROUND_GREEN | FOREGROUND_BLUE;
	case DARKRED:
		return FOREGROUND_RED;
	case DARKMAGENTA:
		return FOREGROUND_RED | FOREGROUND_BLUE;
	case DARKYELLOW:
		return FOREGROUND_RED | FOREGROUND_GREEN;
	case DARKGRAY:
		return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	case GRAY:
		return FOREGROUND_INTENSITY;
	case BLUE:
		return FOREGROUND_INTENSITY | FOREGROUND_BLUE;
	case GREEN:
		return FOREGROUND_INTENSITY | FOREGROUND_GREEN;
	case CYAN:
		return FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE;
	case RED:
		return FOREGROUND_INTENSITY | FOREGROUND_RED;
	case MAGENTA:
		return FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE;
	case YELLOW:
		return FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
	case WHITE:
	default:
		return FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	}
}

static WORD consoleColorToBackgroundColor(ConsoleColor color)
{
	switch (color)
	{
	case BLACK:
		return 0;
	case DARKBLUE:
		return BACKGROUND_BLUE;
	case DARKGREEN:
		return BACKGROUND_GREEN;
	case DARKCYAN:
		return BACKGROUND_GREEN | BACKGROUND_BLUE;
	case DARKRED:
		return BACKGROUND_RED;
	case DARKMAGENTA:
		return BACKGROUND_RED | BACKGROUND_BLUE;
	case DARKYELLOW:
		return BACKGROUND_RED | BACKGROUND_GREEN;
	case DARKGRAY:
		return BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
	case GRAY:
		return BACKGROUND_INTENSITY;
	case BLUE:
		return BACKGROUND_INTENSITY | BACKGROUND_BLUE;
	case GREEN:
		return BACKGROUND_INTENSITY | BACKGROUND_GREEN;
	case CYAN:
		return BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE;
	case RED:
		return BACKGROUND_INTENSITY | BACKGROUND_RED;
	case MAGENTA:
		return BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE;
	case YELLOW:
		return BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN;
	case WHITE:
	default:
		return BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
	}
}

int CALLBACK win32FontExistsCallback(const LOGFONTW*, const TEXTMETRICW*, DWORD, LPARAM)
{
	return fontExistCode;
}

static void initializeStdoutIfNecessary()
{
	// NOTE: Check if the user may have re-assigned the stdout handle. If they have
	//       then we'll reinitialize the file/console to the defaults.
	HANDLE currentStdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (stdoutHandle != NULL && currentStdoutHandle != stdoutHandle)
	{
		stdoutHandle = NULL;
	}

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
			// Get original console info in case we need to reset colors
			{
				originalConsoleInfo = {};
				originalConsoleInfo.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
				GetConsoleScreenBufferInfoEx(stdoutHandle, &originalConsoleInfo);

				// Setup a nicer color palette
				originalConsoleInfo.ColorTable[0] = 0x00'0c'0c'0c; // Black
				originalConsoleInfo.ColorTable[1] = 0x00'da'37'00; // Dark Blue
				originalConsoleInfo.ColorTable[2] = 0x00'0e'9d'13; // Dark Green
				originalConsoleInfo.ColorTable[3] = 0x00'cf'8d'37; // Dark Cyan
				originalConsoleInfo.ColorTable[4] = 0x00'1e'0f'bd; // Dark Red
				originalConsoleInfo.ColorTable[5] = 0x00'98'17'88; // Dark Magenta (Powershell Blue)
				originalConsoleInfo.ColorTable[6] = 0x00'00'9b'bf; // Dark Yellow
				originalConsoleInfo.ColorTable[7] = 0x00'b6'b6'b6; // Dark Gray
				originalConsoleInfo.ColorTable[8] = 0x00'76'76'76; // Gray
				originalConsoleInfo.ColorTable[9] = 0x00'ff'78'3b; // Blue
				originalConsoleInfo.ColorTable[10] = 0x00'0c'c6'16; // Green
				originalConsoleInfo.ColorTable[11] = 0x00'd6'd6'61; // Cyan
				originalConsoleInfo.ColorTable[12] = 0x00'55'48'e5; // Red
				originalConsoleInfo.ColorTable[13] = 0x00'9e'00'b4; // Magenta
				originalConsoleInfo.ColorTable[14] = 0x00'a5'f1'f9; // Yellow
				originalConsoleInfo.ColorTable[15] = 0x00'f2'f2'f2; // White

				WORD backgroundColor = originalConsoleInfo.wAttributes & 0xF0;
				originalConsoleInfo.wAttributes = backgroundColor | consoleColorToForegroundColor(ConsoleColor::WHITE);

				// Reset the size to a reasonable size
				_SMALL_RECT sr;
				sr.Left = 0;
				sr.Top = 0;
				sr.Right = 120;
				sr.Bottom = 25;

				if (originalConsoleInfo.srWindow.Right < sr.Right)
					originalConsoleInfo.srWindow.Right = sr.Right + 1;
				if (originalConsoleInfo.srWindow.Bottom < sr.Bottom)
					originalConsoleInfo.srWindow.Bottom = sr.Bottom + 1;

				SetConsoleScreenBufferInfoEx(stdoutHandle, &originalConsoleInfo);
				SetConsoleWindowInfo(stdoutHandle, FALSE, &sr);
			}

			DWORD nfont = 0;

			CONSOLE_FONT_INFOEX cfi;
			cfi.cbSize = sizeof(cfi);
			cfi.nFont = nfont;
			cfi.dwFontSize.X = 0;
			cfi.dwFontSize.Y = 20;
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
				L"Lucida Console",
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

			return;
		}

		DWORD fileType = GetFileType(stdoutHandle);
		if (fileType == FILE_TYPE_PIPE)
		{
			// If we're writing to a pipe, nothing needs to be done
			return;
		}

		if (fileType == FILE_TYPE_UNKNOWN)
		{
			throw std::runtime_error("Cannot write to stdout. Stdout is directed at an unknown FILE_TYPE.");
		}

		// NOTE: We're writing to a file here, check if it's a new file or not
		//       and write the BOM if it's a new file.
		LARGE_INTEGER fileSize;
		if (GetFileSizeEx(stdoutHandle, &fileSize) == 0)
		{
			DWORD errorCode = GetLastError();
			throw std::runtime_error(("Stdout points to an invalid file." + std::to_string(errorCode)).c_str());
		}

		if (fileSize.QuadPart == 0)
		{
			const uint8_t utf8Bom[] = { 0xEF, 0xBB, 0xBF };
			DWORD numCharsWritten;
			WriteFile(stdoutHandle, utf8Bom, sizeof(utf8Bom), &numCharsWritten, NULL);

			if (numCharsWritten != sizeof(utf8Bom))
			{
				throw std::runtime_error("Failed to write file BOM for stdout.");
			}
		}
	}
}

void setColor(ConsoleColor background, ConsoleColor foreground)
{
	WORD backgroundColor = consoleColorToBackgroundColor(background);
	WORD foregroundColor = consoleColorToForegroundColor(foreground);
	SetConsoleTextAttribute(stdoutHandle, backgroundColor | foregroundColor);
}

void setBackgroundColor(ConsoleColor background)
{
	WORD foregroundColor = originalConsoleInfo.wAttributes & 0xF;
	WORD color = consoleColorToBackgroundColor(background);
	SetConsoleTextAttribute(stdoutHandle, color | foregroundColor);
}

void setForegroundColor(ConsoleColor foreground)
{
	WORD backgroundColor = originalConsoleInfo.wAttributes & 0xF0;
	WORD color = consoleColorToForegroundColor(foreground);
	SetConsoleTextAttribute(stdoutHandle, color | backgroundColor);
}

void resetColor()
{
	SetConsoleTextAttribute(stdoutHandle, originalConsoleInfo.wAttributes);
}

void printf(const char* s)
{
	_printfInternal(s, std::strlen(s));
}

void _printfInternal(const char* s, size_t length)
{
	initializeStdoutIfNecessary();
	if (length <= MAXDWORD)
	{
		DWORD numCharsInString = (DWORD)length;
		DWORD numCharsWritten;
		if (!writingDirectlyToConsole)
		{
			BOOL fileWriteRes = WriteFile(stdoutHandle, s, (DWORD)length, &numCharsWritten, NULL);
			if (fileWriteRes == 0)
			{
				DWORD error = GetLastError();
				throw std::runtime_error(("Failed to write to stdout. Error code: " + std::to_string(error)).c_str());
			}
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
			throw std::runtime_error("Failed to write to stdout.");
		}
	}
	else
	{
		std::string errorMessage = "String length is invalid '" + std::to_string(length) + "'. Must be >= 0 && <= " + std::to_string(MAXDWORD);
		throw std::runtime_error(errorMessage.c_str());
	}
}

void printFormattedString(const char* content, size_t contentLength, const char* prefix, size_t prefixLength, const Stream& io, bool isPositive)
{
	bool shouldUseZeroPadding = ((uint8_t)io.mods & (uint8_t)StreamMods::UseZeroPadding);
	bool shouldUseAltFormat = ((uint8_t)io.mods & (uint8_t)StreamMods::AltFormat);
	StreamAlign alignment = io.alignment;
	uint32_t fillChar = io.fillCharacter;
	StreamSign sign = io.sign;

	if (prefixLength > 0 && shouldUseAltFormat && shouldUseZeroPadding)
	{
		// NOTE: If we're printing alt format and want to use 0's as padding,
		//       print the prefix before anything else
		//       We also force alignment to the right
		_printfInternal(prefix, prefixLength);
	}

	if (shouldUseZeroPadding)
	{
		fillChar = '0';
		alignment = StreamAlign::Right;
	}

	auto numCharsInContent = String::utf8Length(content, contentLength);
	if (!numCharsInContent.hasValue())
	{
		throw std::runtime_error("Tried to print invalid UTF8 string in printf.");
	}
	size_t totalContentLength = prefixLength + numCharsInContent.value();
	uint32_t leftPadding = 0;
	uint32_t rightPadding = 0;
	if (io.width != 0)
	{
		switch (alignment)
		{
		case StreamAlign::Left:
		{
			if (io.width > totalContentLength)
			{
				rightPadding = (uint32_t)(io.width - totalContentLength);
			}
		}
		break;
		case StreamAlign::Right:
		{
			if (io.width > totalContentLength)
			{
				leftPadding = (uint32_t)(io.width - totalContentLength);
			}
		}
		break;
		case StreamAlign::Center:
		{
			if (io.width > totalContentLength)
			{
				rightPadding = (uint32_t)((io.width - totalContentLength) / 2);
				// Since integer division can truncate, leftPadding will just
				// take whatever's left
				leftPadding = (uint32_t)(io.width - totalContentLength - rightPadding);
			}
		}
		break;
		}
	}

	// Only allocate scratch memory if needed
	uint8_t numBytesInChar = 1;
	if (fillChar >= 0xFF'FF'FF)
	{
		numBytesInChar = 4;
	}
	else if (fillChar >= 0xFF'FF)
	{
		numBytesInChar = 3;
	}
	else if (fillChar >= 0xFF)
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
				scratchMemory[(i * numBytesInChar) + j] = (uint8_t)((fillChar >> ((numBytesInChar - j - 1) * 8)) & 0xFF);
			}
		}

		_printfInternal(scratchMemory, leftPadding * numBytesInChar);
	}

	if (prefixLength > 0 && shouldUseAltFormat && !shouldUseZeroPadding)
	{
		// NOTE: If we're not using the zero padding, then we need to print the
		//       prefix immediately before the content
		_printfInternal(prefix, prefixLength);
	}

	if (sign != StreamSign::Negative && isPositive && (
		io.type == StreamParamType::Decimal ||
		io.type == StreamParamType::ExponentNotation ||
		io.type == StreamParamType::FixedPoint ||
		io.type == StreamParamType::GeneralFormat
		))
	{
		switch (sign)
		{
		case StreamSign::Positive:
			_printfInternal("+", sizeof("+") - 1);
			break;
		case StreamSign::Space:
			_printfInternal(" ", sizeof(" ") - 1);
			break;
		default:
			break;
		}
	}

	_printfInternal(content, contentLength);

	if (rightPadding > 0)
	{
		for (size_t i = leftPadding; i < rightPadding; i++)
		{
			for (size_t j = 0; j < numBytesInChar; j++)
			{
				scratchMemory[i] = (uint8_t)((fillChar >> ((numBytesInChar - j - 1) * 8)) & 0xFF);
			}
		}

		_printfInternal(scratchMemory, rightPadding * numBytesInChar);
	}

	if ((leftPadding + rightPadding) * numBytesInChar > smallStringBufferSize)
	{
		free(scratchMemory);
	}
}

}
} // End CppUtils::io

#else // end _WIN32

namespace CppUtils
{
namespace IO
{

void printf(const char* s)
{
	std::cout << s;
}

void _printfInternal(const char* s, size_t length)
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

}
} // End CppUtils::io

#endif // end PLATFORM_IMPLS

#endif // end GABE_CPP_PRINT_IMPL