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


 -------- LICENSE --------

 Open Source



 -------- DOCUMENTATION --------

 TODO: Add me


 -------- DLL STUFF --------

 If you want to use this library as part of a DLL, I have created a macro:

	GABE_CPP_STRINGS_API

 that does nothing, but prefaces each forward declaration. Simply redefine this
 to your preferred __declspec(dllimport) or __declspec(dllexport) before including this
 file anywhere and it should just work out.

 -------- UTF8 Encoding Details --------
 (These are details for my own benefit while implementing UTF8  aware parsing,
 but I'll leave them in here for anyone else who may be interested)

 Adapted from https://www.rfc-editor.org/rfc/rfc3629

 - General Details 1 -
 * [0x0000-0x007F] (correspond to 7-bit US-ASCII values)
 * The first octect of a multi-octet sequence indicates the number of upcoming octets
 * Octets 0xC0, 0xC1, [0xF5-0xFF] should never appear
 * Character boundaries are easily findable from anywhere in the stream

 - General Details 2 -
 * Characters from U+0000-U+10FFFF range are encoded using 1-4 octets
 * The following table illustrates how the bytes are encoded:
   * The first byte contains n bits set, followed by a 0 where n is the number of octets
	 in the character.
   * The next (n - 1) bytes all start with a 10 (2^8) in binary

  +---------------------+-------------------------------------------+
  |  Char. number range |             UTF-8 octet sequence          |
  |    (hexadecimal)    |                   (binary)                |
  +---------------------+----------+----------+----------+----------+
  |                     | byte 0   | byte 1   | byte 2   | byte 3   |
  +---------------------+----------+----------+----------+----------+
  | 0000 0000-0000 007F | 0xxxxxxx |          |          |          |
  | 0000 0080-0000 07FF | 110xxxxx | 10xxxxxx |          |          |
  | 0000 0800-0000 FFFF | 1110xxxx | 10xxxxxx | 10xxxxxx |          |
  | 0001 0000-0010 FFFF | 11110xxx | 10xxxxxx | 10xxxxxx | 10xxxxxx |
  +---------------------+----------+----------+----------+----------+

  - Decoding a character -
  1. Initialize a 32 bit binary number with all bits set to 0 (up to 21 bits may be needed)
  2. Figure out which bits encode the number (the x's in the table above)
  3. Distribute the bits into the binary number starting with the lowest order bits and moving up

*/
#ifndef GABE_CPP_STRINGS_H
#define GABE_CPP_STRINGS_H

#include <stdint.h>
#include <cppUtils/cppMaybe.hpp>

namespace CppUtils {

struct Stream;

// ----- Basic string data structures/constructors/destructors -----
// Should always be UTF8-Encoded
struct BasicString
{
	uint8_t* data;
	size_t numBytes;
	size_t numCharacters;
};

struct ConstantString
{
	const uint8_t* rawStringLiteral;
	size_t numBytes;
	size_t numCharacters;
};

enum class Utf8ErrorCode : uint8_t
{
	InvalidString = 0,
};

namespace String {

Maybe<ConstantString, Utf8ErrorCode> makeConstantString(const char* rawStringLiteral);
Maybe<BasicString, Utf8ErrorCode> makeString(const char* rawString);

inline Maybe<ConstantString, Utf8ErrorCode> makeConstantString(const uint8_t* rawString) { return makeConstantString((const char*)rawString); }
inline Maybe<BasicString, Utf8ErrorCode> makeString(const uint8_t* rawString) { return makeString((const char*)rawString); }

void free(Maybe<BasicString, Utf8ErrorCode>& string);
void free(BasicString& string);

Maybe<size_t, Utf8ErrorCode> utf8Length(const char* rawString);
Maybe<size_t, Utf8ErrorCode> utf8Length(const char* rawString, size_t rawStringNumBytes);

} } // End CppUtils::String

bool operator==(const CppUtils::BasicString& a, const CppUtils::BasicString& b);
bool operator==(const CppUtils::ConstantString& a, const CppUtils::ConstantString& b);
bool operator==(const CppUtils::BasicString& a, const CppUtils::ConstantString& b);
inline bool operator==(const CppUtils::ConstantString& a, const CppUtils::BasicString& b) { return b == a; }

inline bool operator!=(const CppUtils::BasicString& a, const CppUtils::BasicString& b) { return !(a == b); }
inline bool operator!=(const CppUtils::ConstantString& a, const CppUtils::ConstantString& b) { return !(a == b); }
inline bool operator!=(const CppUtils::BasicString& a, const CppUtils::ConstantString& b) { return !(a == b); }
inline bool operator!=(const CppUtils::ConstantString& a, const CppUtils::BasicString& b) { return !(b == a); }

namespace CppUtils {

// ----- Parsing helpers -----
// NOTE: This data structure is NON-OWNING, so it will not make a copy of the string
//       It is up to the caller to ensure that the raw string lives throughout any
//       subsequent calls to Parser::*
struct ParseInfo
{
	const uint8_t* utf8String;
	size_t cursor;
	size_t numBytes;
};

namespace Parser {

Maybe<ParseInfo, Utf8ErrorCode> makeParseInfo(const char* rawString, size_t numBytes);
Maybe<ParseInfo, Utf8ErrorCode> makeParseInfo(const char* rawString);
inline Maybe<ParseInfo, Utf8ErrorCode> makeParseInfo(const BasicString& dumbString) { return makeParseInfo((const char*)dumbString.data, dumbString.numBytes); }

Maybe<uint32_t, Utf8ErrorCode> parseCharacter(ParseInfo& parseInfo, uint8_t* numBytesParsed, size_t offset = 0);
Maybe<uint32_t, Utf8ErrorCode> peek(ParseInfo& parseInfo, uint8_t* numBytesParsed, size_t peekAmount = 0);

} } // End CppUtils::Parser

// Stupid stuff. We have to have this operator in the global namespace to make sure it can be resolved appropriately
CppUtils::Stream& operator<<(CppUtils::Stream& io, CppUtils::Utf8ErrorCode error);

#endif // GABE_CPP_STRINGS_H

#ifdef GABE_CPP_STRING_IMPL
#define USE_GABE_CPP_PRINT
#include <cppUtils/cppUtils.hpp>

CppUtils::Stream& operator<<(CppUtils::Stream& io, CppUtils::Utf8ErrorCode error)
{
	switch (error)
	{
	case CppUtils::Utf8ErrorCode::InvalidString:
		io << "Invalid UTF8 BasicString";
		break;
	}

	return io;
}

namespace CppUtils { namespace String {

// ------------- Internal Functions -------------
static size_t getNumBytesTilNull(const char* rawString);
static Maybe<uint8_t, Utf8ErrorCode> getNumOctets(const uint8_t* rawString, size_t numBytes, size_t cursor);
static Maybe<uint32_t, Utf8ErrorCode> decodeChar(const uint8_t* rawString, size_t numBytes, uint8_t numOctets, size_t cursor);

Maybe<BasicString, Utf8ErrorCode> makeString(const char* rawString)
{
	size_t numBytes = getNumBytesTilNull(rawString);
	Maybe<size_t, Utf8ErrorCode> numCharacters = utf8Length(rawString, numBytes);
	if (!numCharacters.hasValue())
	{
		return numCharacters.error();
	}

	uint8_t* dumbString = (uint8_t*)g_memory_allocate((numBytes + 1) * sizeof(uint8_t));

	g_memory_copyMem(dumbString, (numBytes + 1) * sizeof(uint8_t), (void*)rawString, numBytes * sizeof(uint8_t));
	dumbString[numBytes] = '\0';

	return BasicString{
		dumbString,
		numBytes,
		numCharacters.value()
	};
}

Maybe<ConstantString, Utf8ErrorCode> makeConstantString(const char* rawStringLiteral)
{
	size_t numBytes = getNumBytesTilNull(rawStringLiteral);
	// Constant strings live through the lifetime of the program and don't need to be malloced
	ConstantString res = {
		(const uint8_t*)rawStringLiteral,
		numBytes,
		0
	};

	Maybe<size_t, Utf8ErrorCode> numCharacters = utf8Length(rawStringLiteral);
	if (numCharacters.hasValue())
	{
		res.numCharacters = numCharacters.value();
		return res;
	}

	return numCharacters.error();
}

void free(Maybe<BasicString, Utf8ErrorCode>& string)
{
	if (string.hasValue())
	{
		free(string.mut_value());
	}
}

void free(BasicString& string)
{
	g_memory_free(string.data);
	g_memory_zeroMem(&string, sizeof(BasicString));
}

Maybe<size_t, Utf8ErrorCode> utf8Length(const char* rawString, size_t numBytes)
{
	// Count the number of characters in the string and also validate the UTF8 string
	// along the way
	size_t numCharacters = 0;
	for (size_t i = 0; i < numBytes;)
	{
		auto numOctets = getNumOctets((const uint8_t*)rawString, numBytes, i);
		if (!numOctets.hasValue())
		{
			return numOctets.error();
		}

		auto decodedChar = decodeChar((const uint8_t*)rawString, numBytes, numOctets.value(), i);
		if (!decodedChar.hasValue())
		{
			return decodedChar.error();
		}

		numCharacters++;
		i += numOctets.value();
	}

	return numCharacters;
}

Maybe<size_t, Utf8ErrorCode> utf8Length(const char* rawString)
{
	size_t numBytes = getNumBytesTilNull(rawString);
	return utf8Length(rawString, numBytes);
}

} } // End CppUtils::String

bool operator==(const BasicString& a, const BasicString& b)
{
	return g_memory_compareMem(a.data, a.numBytes, b.data, b.numBytes);
}

bool operator==(const ConstantString& a, const ConstantString& b)
{
	return g_memory_compareMem((void*)a.rawStringLiteral, a.numBytes, (void*)b.rawStringLiteral, b.numBytes);
}

bool operator==(const BasicString& a, const ConstantString& b)
{
	return g_memory_compareMem((void*)a.data, a.numBytes, (void*)b.rawStringLiteral, b.numBytes);
}

namespace CppUtils { namespace String {

// ------------- Internal Functions -------------
static size_t getNumBytesTilNull(const char* rawString)
{
	size_t i = 0;
	while (rawString[0] != '\0')
	{
		i++;
		rawString++;
	}

	return i;
}

static Maybe<uint8_t, Utf8ErrorCode> getNumOctets(const uint8_t* string, size_t numBytes, size_t cursor)
{
	if (cursor >= numBytes)
	{
		return Utf8ErrorCode::InvalidString;
	}

	// ------- Useful Constants -------
	constexpr uint8_t numOctetMasks = 4;
	constexpr uint8_t OCTET_BYTE_ONE_MASKS[numOctetMasks] = {
		0b0,
		0b110,
		0b1110,
		0b1111'0
	};

	constexpr uint8_t OCTET_SHIFT_AMTS[numOctetMasks] = {
		7,
		5,
		4,
		3
	};

	constexpr uint8_t OCTET_EXTRA_BYTE_MASK = 0b10;
	constexpr uint8_t OCTET_EXTRA_BYTE_SHIFT_AMT = 6;

	bool oneBytePass = (string[cursor] >> OCTET_SHIFT_AMTS[0]) == OCTET_BYTE_ONE_MASKS[0];
	bool twoBytePass = (string[cursor] >> OCTET_SHIFT_AMTS[1]) == OCTET_BYTE_ONE_MASKS[1];
	bool threeBytePass = (string[cursor] >> OCTET_SHIFT_AMTS[2]) == OCTET_BYTE_ONE_MASKS[2];
	bool fourBytePass = (string[cursor] >> OCTET_SHIFT_AMTS[3]) == OCTET_BYTE_ONE_MASKS[3];

	uint8_t numOctets = 0;
	bool pass = false;
	if (oneBytePass)
	{
		numOctets = 1;
		pass = true;
	}
	else if (twoBytePass)
	{
		if (cursor + 1 >= numBytes)
		{
			return Utf8ErrorCode::InvalidString;
		}

		bool secondByteValid = (string[cursor + 1] >> OCTET_EXTRA_BYTE_SHIFT_AMT) == OCTET_EXTRA_BYTE_MASK;
		numOctets = 2;
		pass = secondByteValid;
	}
	else if (threeBytePass)
	{
		if (cursor + 2 >= numBytes)
		{
			return Utf8ErrorCode::InvalidString;
		}

		bool secondByteValid = (string[cursor + 1] >> OCTET_EXTRA_BYTE_SHIFT_AMT) == OCTET_EXTRA_BYTE_MASK;
		bool thirdByteValid = (string[cursor + 2] >> OCTET_EXTRA_BYTE_SHIFT_AMT) == OCTET_EXTRA_BYTE_MASK;
		numOctets = 3;
		pass = secondByteValid && thirdByteValid;
	}
	else if (fourBytePass)
	{
		if (cursor + 3 >= numBytes)
		{
			return Utf8ErrorCode::InvalidString;
		}

		bool secondByteValid = (string[cursor + 1] >> OCTET_EXTRA_BYTE_SHIFT_AMT) == OCTET_EXTRA_BYTE_MASK;
		bool thirdByteValid = (string[cursor + 2] >> OCTET_EXTRA_BYTE_SHIFT_AMT) == OCTET_EXTRA_BYTE_MASK;
		bool fourthByteValid = (string[cursor + 3] >> OCTET_EXTRA_BYTE_SHIFT_AMT) == OCTET_EXTRA_BYTE_MASK;
		numOctets = 4;
		pass = secondByteValid && thirdByteValid && fourthByteValid;
	}

	if (pass)
	{
		return numOctets;
	}

	return Utf8ErrorCode::InvalidString;
}

static Maybe<uint32_t, Utf8ErrorCode> decodeChar(const uint8_t* rawString, size_t numBytes, uint8_t numOctets, size_t cursor)
{
	if (cursor + numOctets > numBytes)
	{
		return Utf8ErrorCode::InvalidString;
	}

	// Simple case, just return the masked byte
	if (numOctets == 1)
	{
		uint8_t bitMask = 0b0111'1111;
		uint8_t maskedBits = rawString[cursor] & bitMask;
		uint32_t res = (uint32_t)maskedBits;

		// Invalid range
		if (res > 0x007f)
		{
			return Utf8ErrorCode::InvalidString;
		}

		return res;
	}

	uint32_t res = 0;
	uint8_t lowOctetBitmask = 0b0011'1111;
	uint8_t maskedByte1 = rawString[(cursor + 1) % numOctets] & lowOctetBitmask;
	uint8_t maskedByte2 = rawString[(cursor + 2) % numOctets] & lowOctetBitmask;
	uint8_t maskedByte3 = rawString[(cursor + 3) % numOctets] & lowOctetBitmask;

	switch (numOctets)
	{
	case 2:
	{
		uint8_t byte0Bitmask = 0b0001'1111;
		uint8_t maskedByte0 = rawString[cursor] & byte0Bitmask;

		res = res | ((uint32_t)maskedByte0 << 6);
		res = res | ((uint32_t)maskedByte1);

		// Invalid range
		if (res < 0x0080 || res > 0x07ff)
		{
			return Utf8ErrorCode::InvalidString;
		}

		return res;
	}
	break;
	case 3:
	{
		uint8_t byte0Bitmask = 0b0000'1111;
		uint8_t maskedByte0 = rawString[cursor] & byte0Bitmask;

		res = res | ((uint32_t)maskedByte2);
		res = res | ((uint32_t)maskedByte1 << 6);
		res = res | ((uint32_t)maskedByte0 << 12);

		// Invalid range
		if (res < 0x0800 || res > 0xffff)
		{
			return Utf8ErrorCode::InvalidString;
		}

		return res;
	}
	break;
	case 4:
	{
		uint8_t byte0Bitmask = 0b0000'0111;
		uint8_t maskedByte0 = rawString[cursor] & byte0Bitmask;

		res = res | ((uint32_t)maskedByte3);
		res = res | ((uint32_t)maskedByte2 << 6);
		res = res | ((uint32_t)maskedByte1 << 12);
		res = res | ((uint32_t)maskedByte0 << 18);

		// Invalid range
		if (res < 0x001'0000 || res > 0x0010'ffff)
		{
			return Utf8ErrorCode::InvalidString;
		}

		return res;
	}
	break;
	}

	return Utf8ErrorCode::InvalidString;
}

} // End String

namespace Parser {

Maybe<ParseInfo, Utf8ErrorCode> makeParseInfo(const char* rawString, size_t numBytes)
{
	ParseInfo res;
	res.cursor = 0;
	res.numBytes = numBytes;
	res.utf8String = (const uint8_t*)rawString;
	return res;
}

Maybe<ParseInfo, Utf8ErrorCode> makeParseInfo(const char* rawString)
{
	return makeParseInfo(rawString, String::getNumBytesTilNull(rawString));
}

Maybe<uint32_t, Utf8ErrorCode> parseCharacter(ParseInfo& parseInfo, uint8_t* numBytesParsed, size_t offset)
{
	auto numOctets = String::getNumOctets(parseInfo.utf8String, parseInfo.numBytes, parseInfo.cursor + offset);
	if (!numOctets.hasValue())
	{
		return numOctets.error();
	}

	auto character = String::decodeChar(parseInfo.utf8String, parseInfo.numBytes, numOctets.value(), offset + parseInfo.cursor);
	if (!character.hasValue())
	{
		return character.error();
	}

	*numBytesParsed = *numOctets;
	return character.value();
}

Maybe<uint32_t, Utf8ErrorCode> peek(ParseInfo& parseInfo, uint8_t* numBytesParsed, size_t peekAmount)
{
	if (parseInfo.cursor >= parseInfo.numBytes)
	{
		return '\0';
	}

	Maybe<uint32_t, Utf8ErrorCode> res = 0;

	size_t peekCursorPos = 0;
	size_t numCharsParsed = 0;
	while (numCharsParsed <= peekAmount)
	{
		uint8_t numBytesParsedInternal = 0;
		res = parseCharacter(parseInfo, &numBytesParsedInternal, peekCursorPos);
		if (!res.hasValue())
		{
			return res.error();
		}

		peekCursorPos += numBytesParsedInternal;
		numCharsParsed++;
	}

	*numBytesParsed = (uint8_t)peekCursorPos;
	return res;
}

} } // End CppUtils::Parser

#endif // GABE_CPP_STRING_IMPL