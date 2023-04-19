/*
 Do this:
	 #define GABE_CPP_STRINGS_IMPL
 before you include this file in *one* C++ file to create the implementation.

 // i.e. it should look like this in *one* source file:
 #define GABE_CPP_STRINGS_IMPL
 #include "cppStrings.hpp"


 -------- LICENSE --------

 Open Source, see end of file



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

struct g_io_stream;

// ----- Basic string data structures/constructors/destructors -----
// Should always be UTF8-Encoded
struct g_DumbString
{
	uint8_t* data;
	size_t numBytes;
	size_t numCharacters;
};

struct g_DumbConstantString
{
	const uint8_t* rawStringLiteral;
	size_t numBytes;
	size_t numCharacters;
};

enum g_Utf8ErrorCode
{
	g_Utf8ErrorCode_Success = 0,
	g_Utf8ErrorCode_InvalidString = 5,
};

g_io_stream& operator<<(g_io_stream& io, g_Utf8ErrorCode error);

g_Maybe<g_DumbConstantString, g_Utf8ErrorCode> g_dumbConstantString(const char* rawStringLiteral);
g_Maybe<g_DumbString, g_Utf8ErrorCode> g_dumbString(const char* rawString);

void g_dumbString_free(g_Maybe<g_DumbString, g_Utf8ErrorCode>& string);
void g_dumbString_free(g_DumbString& string);

g_Maybe<size_t, g_Utf8ErrorCode> g_dumbString_utf8Length(const char* rawString);
g_Maybe<size_t, g_Utf8ErrorCode> g_dumbString_utf8Length(const char* rawString, size_t rawStringNumBytes);

bool operator==(const g_DumbString& a, const g_DumbString& b);
bool operator==(const g_DumbConstantString& a, const g_DumbConstantString& b);

// ----- Parsing helpers -----
// NOTE: This data structure is NON-OWNING, so it will not make a copy of the string
//       It is up to the caller to ensure that the raw string lives throughout any
//       subsequent calls to g_parser_*
struct g_ParseInfo
{
	const uint8_t* utf8String;
	size_t cursor;
	size_t numBytes;
};

g_Maybe<g_ParseInfo, g_Utf8ErrorCode> g_parser_makeParser(const char* rawString, size_t numBytes);
g_Maybe<g_ParseInfo, g_Utf8ErrorCode> g_parser_makeParser(const char* rawString);
inline g_Maybe<g_ParseInfo, g_Utf8ErrorCode> g_parser_makeParser(const g_DumbString& dumbString) { return g_parser_makeParser((const char*)dumbString.data, dumbString.numBytes); }

g_Maybe<uint32_t, g_Utf8ErrorCode> g_parser_parseCharacter(g_ParseInfo& parseInfo, uint8_t* numBytesParsed, size_t offset = 0);
g_Maybe<uint32_t, g_Utf8ErrorCode> g_parser_peek(g_ParseInfo& parseInfo, uint8_t* numBytesParsed, size_t peekAmount = 0);

#endif // GABE_CPP_STRINGS_H

#ifdef GABE_CPP_STRINGS_IMPL
#include <cppUtils/cppUtils.hpp>
#include <stdexcept>

// ------------- Internal variables -------------
static constexpr uint8_t numOctetMasks = 4;
static constexpr uint8_t OCTET_BYTE_ONE_MASKS[numOctetMasks] = {
	0b0,
	0b110,
	0b1110,
	0b1111'0
};

static constexpr uint8_t OCTET_SHIFT_AMTS[numOctetMasks] = {
	7,
	5,
	4,
	3
};

static constexpr uint8_t OCTET_EXTRA_BYTE_MASK = 0b10;
static constexpr uint8_t OCTET_EXTRA_BYTE_SHIFT_AMT = 6;

// ------------- Internal Functions -------------
static size_t getNumBytesTilNull(const char* rawString);
static g_Maybe<uint8_t, g_Utf8ErrorCode> getNumOctets(const uint8_t* rawString, size_t numBytes, size_t cursor);
static g_Maybe<uint32_t, g_Utf8ErrorCode> decodeChar(const uint8_t* rawString, size_t numBytes, uint8_t numOctets, size_t cursor);

g_io_stream& operator<<(g_io_stream& io, g_Utf8ErrorCode error)
{
	switch (error)
	{
	case g_Utf8ErrorCode_InvalidString:
		io << "Invalid UTF8 String";
		break;
	case g_Utf8ErrorCode_Success:
		io << "Success";
		break;
	}

	return io;
}

g_Maybe<g_DumbString, g_Utf8ErrorCode> g_dumbString(const char* rawString)
{
	size_t numBytes = getNumBytesTilNull(rawString);
	g_Maybe<size_t, g_Utf8ErrorCode> numCharacters = g_dumbString_utf8Length(rawString, numBytes);
	if (!numCharacters.hasValue())
	{
		return numCharacters.error();
	}

	uint8_t* dumbString = (uint8_t*)g_memory_allocate((numBytes + 1) * sizeof(uint8_t));

	g_memory_copyMem(dumbString, (void*)rawString, numBytes * sizeof(uint8_t));
	dumbString[numBytes] = '\0';

	return g_DumbString{
		dumbString,
		numBytes,
		numCharacters.value()
	};
}

g_Maybe<g_DumbConstantString, g_Utf8ErrorCode> g_dumbConstantString(const char* rawStringLiteral)
{
	size_t numBytes = getNumBytesTilNull(rawStringLiteral);
	// Constant strings live through the lifetime of the program and don't need to be malloced
	g_DumbConstantString res = {
		(const uint8_t*)rawStringLiteral,
		numBytes,
		0
	};

	g_Maybe<size_t, g_Utf8ErrorCode> numCharacters = g_dumbString_utf8Length(rawStringLiteral);
	if (numCharacters.hasValue())
	{
		res.numCharacters = numCharacters.value();
		return res;
	}

	return numCharacters.error();
}

void g_dumbString_free(g_Maybe<g_DumbString, g_Utf8ErrorCode>& string)
{
	if (string.hasValue())
	{
		g_dumbString_free(string.mut_value());
	}
}

void g_dumbString_free(g_DumbString& string)
{
	g_memory_free(string.data);
	g_memory_zeroMem(&string, sizeof(g_DumbString));
}

g_Maybe<size_t, g_Utf8ErrorCode> g_dumbString_utf8Length(const char* rawString, size_t numBytes)
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

g_Maybe<size_t, g_Utf8ErrorCode> g_dumbString_utf8Length(const char* rawString)
{
	size_t numBytes = getNumBytesTilNull(rawString);
	return g_dumbString_utf8Length(rawString, numBytes);
}

bool operator==(const g_DumbString& a, const g_DumbString& b)
{
	return g_memory_compareMem(a.data, a.numBytes, b.data, b.numBytes);
}

bool operator==(const g_DumbConstantString& a, const g_DumbConstantString& b)
{
	return g_memory_compareMem((void*)a.rawStringLiteral, a.numBytes, (void*)b.rawStringLiteral, b.numBytes);
}

g_Maybe<g_ParseInfo, g_Utf8ErrorCode> g_parser_makeParser(const char* rawString, size_t numBytes)
{
	g_ParseInfo res;
	res.cursor = 0;
	res.numBytes = numBytes;
	res.utf8String = (const uint8_t*)rawString;
	return res;
}

g_Maybe<g_ParseInfo, g_Utf8ErrorCode> g_parser_makeParser(const char* rawString)
{
	return g_parser_makeParser(rawString, getNumBytesTilNull(rawString));
}

g_Maybe<uint32_t, g_Utf8ErrorCode> g_parser_parseCharacter(g_ParseInfo& parseInfo, uint8_t* numBytesParsed, size_t offset)
{
	auto numOctets = getNumOctets(parseInfo.utf8String, parseInfo.numBytes, parseInfo.cursor + offset);
	if (!numOctets.hasValue())
	{
		return numOctets.error();
	}

	auto character = decodeChar(parseInfo.utf8String, parseInfo.numBytes, numOctets.value(), offset + parseInfo.cursor);
	if (!character.hasValue())
	{
		return character.error();
	}

	*numBytesParsed = *numOctets;
	return character.value();
}

g_Maybe<uint32_t, g_Utf8ErrorCode> g_parser_peek(g_ParseInfo& parseInfo, uint8_t* numBytesParsed, size_t peekAmount)
{
	if (parseInfo.cursor >= parseInfo.numBytes)
	{
		return '\0';
	}

	g_Maybe<uint32_t, g_Utf8ErrorCode> res = 0;

	size_t peekCursorPos = 0;
	size_t numCharsParsed = 0;
	while (numCharsParsed <= peekAmount)
	{
		uint8_t numBytesParsedInternal = 0;
		res = g_parser_parseCharacter(parseInfo, &numBytesParsedInternal, peekCursorPos);
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

static g_Maybe<uint8_t, g_Utf8ErrorCode> getNumOctets(const uint8_t* string, size_t numBytes, size_t cursor)
{
	if (cursor >= numBytes)
	{
		return g_Utf8ErrorCode_InvalidString;
	}

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
			return g_Utf8ErrorCode_InvalidString;
		}

		bool secondByteValid = (string[cursor + 1] >> OCTET_EXTRA_BYTE_SHIFT_AMT) == OCTET_EXTRA_BYTE_MASK;
		numOctets = 2;
		pass = secondByteValid;
	}
	else if (threeBytePass)
	{
		if (cursor + 2 >= numBytes)
		{
			return g_Utf8ErrorCode_InvalidString;
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
			return g_Utf8ErrorCode_InvalidString;
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

	return g_Utf8ErrorCode_InvalidString;
}

static g_Maybe<uint32_t, g_Utf8ErrorCode> decodeChar(const uint8_t* rawString, size_t numBytes, uint8_t numOctets, size_t cursor)
{
	if (cursor + numOctets > numBytes)
	{
		return g_Utf8ErrorCode_InvalidString;
	}

	uint32_t res = 0;
	{
		uint8_t bitMask = 0b0011'1111;
		for (uint8_t i = numOctets - 1; i > 0; i--)
		{
			uint8_t maskedBits = rawString[cursor + i] & bitMask;
			uint8_t shiftAmt = (numOctets - i - 1) * 6;
			res = res | ((uint32_t)maskedBits << shiftAmt);
		}
	}

	switch (numOctets)
	{
	case 1:
	{
		uint8_t bitMask = 0b0111'1111;
		uint8_t maskedBits = rawString[cursor] & bitMask;
		res = res | (uint32_t)maskedBits;

		// Invalid range
		if (res > 0x007f)
		{
			return g_Utf8ErrorCode_InvalidString;
		}
	}
	break;
	case 2:
	{
		uint8_t bitMask = 0b0001'1111;
		uint8_t maskedBits = rawString[cursor] & bitMask;
		res = res | ((uint32_t)maskedBits << 6);

		// Invalid range
		if (res < 0x0080 || res > 0x07ff)
		{
			return g_Utf8ErrorCode_InvalidString;
		}
	}
	break;
	case 3:
	{
		uint8_t bitMask = 0b0000'1111;
		uint8_t maskedBits = rawString[cursor] & bitMask;
		res = res | ((uint32_t)maskedBits << 12);

		// Invalid range
		if (res < 0x0800 || res > 0xffff)
		{
			return g_Utf8ErrorCode_InvalidString;
		}
	}
	break;
	case 4:
	{
		uint8_t bitMask = 0b0000'0111;
		uint8_t maskedBits = rawString[cursor] & bitMask;
		res = res | ((uint32_t)maskedBits << 18);

		// Invalid range
		if (res < 0x001'0000 || res > 0x0010'ffff)
		{
			return g_Utf8ErrorCode_InvalidString;
		}
	}
	break;
	default:
		return g_Utf8ErrorCode_InvalidString;
	}

	return res;
}

#endif // GABE_CPP_STRINGS_IMPL