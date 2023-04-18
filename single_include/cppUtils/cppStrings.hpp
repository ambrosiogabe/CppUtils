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

g_Utf8ErrorCode g_dumbConstantString(const char* rawStringLiteral, g_DumbConstantString* output);
g_Utf8ErrorCode g_dumbString(const char* rawString, g_DumbString* outputString);
void g_dumbString_free(g_DumbString& string);

g_Utf8ErrorCode g_dumbString_utf8Length(const char* rawString, size_t* outputLength);
g_Utf8ErrorCode g_dumbString_utf8Length(const char* rawString, size_t rawStringNumBytes, size_t* outputLength);

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

g_Utf8ErrorCode g_parser_makeParser(const char* rawString, size_t numBytes, g_ParseInfo* out);
g_Utf8ErrorCode g_parser_makeParser(const char* rawString, g_ParseInfo* out);
inline g_Utf8ErrorCode g_parser_makeParser(const g_DumbString& dumbString, g_ParseInfo* out) { return g_parser_makeParser((const char*)dumbString.data, dumbString.numBytes, out); }

uint32_t g_parser_parseCharacter(g_ParseInfo& parseInfo, uint8_t* numBytesParsed, size_t offset = 0);
uint32_t g_parser_peek(g_ParseInfo& parseInfo, uint8_t* numBytesParsed, size_t peekAmount = 0);

#endif // GABE_CPP_STRINGS_H

#ifdef GABE_CPP_STRINGS_IMPL
#include <cppUtils/cppUtils.hpp>
#include <stdexcept>

// ------------- Internal variables -------------
static constexpr uint8_t numOctetMasks = 4;
static constexpr uint8_t OCTET_MASKS[numOctetMasks] = {
	0b0000'0000,
	0b1100'0000,
	0b1110'0000,
	0b1111'0000
};

static constexpr uint8_t OCTET_0_MASKS[numOctetMasks] = {
	0b1000'0000,
	0b0010'0000,
	0b0001'0000,
	0b0000'1000,
};

static constexpr uint8_t ONE_ZERO_MASK = 0b1000'0000;
static constexpr uint8_t ONE_ZERO_0_MASK = 0b0100'0000;

// ------------- Internal Functions -------------
static size_t getNumBytesTilNull(const char* rawString);
static uint8_t getNumOctets(const uint8_t* rawString, size_t numBytes, size_t cursor);

g_Utf8ErrorCode g_dumbString(const char* rawString, g_DumbString* outputString)
{
	size_t numBytes = getNumBytesTilNull(rawString);
	uint8_t* dumbString = (uint8_t*)g_memory_allocate((numBytes + 1) * sizeof(uint8_t));

	g_memory_copyMem(dumbString, (void*)rawString, numBytes * sizeof(uint8_t));
	dumbString[numBytes] = '\0';

	*outputString = {
		dumbString,
		numBytes,
		0
	};

	g_Utf8ErrorCode result = g_dumbString_utf8Length(rawString, &outputString->numCharacters);
	return result;
}

g_Utf8ErrorCode g_dumbConstantString(const char* rawStringLiteral, g_DumbConstantString* outputString)
{
	size_t numBytes = getNumBytesTilNull(rawStringLiteral);
	// Constant strings live through the lifetime of the program and don't need to be malloced
	*outputString = {
		(const uint8_t*)rawStringLiteral,
		numBytes,
		0
	};

	g_Utf8ErrorCode result = g_dumbString_utf8Length(rawStringLiteral, &outputString->numCharacters);
	return result;
}

void g_dumbString_free(g_DumbString& string)
{
	g_memory_free(string.data);
	g_memory_zeroMem(&string, sizeof(g_DumbString));
}

g_Utf8ErrorCode g_dumbString_utf8Length(const char* rawString, size_t numBytes, size_t* outputLength)
{
	// Count the number of characters in the string and also validate the UTF8 string
	// along the way
	*outputLength = 0;
	for (size_t i = 0; i < numBytes;)
	{
		uint8_t numOctets = getNumOctets((const uint8_t*)rawString, numBytes, i);
		if (numOctets == g_Utf8ErrorCode_InvalidString)
		{
			*outputLength = 0;
			return g_Utf8ErrorCode_InvalidString;
		}

		*outputLength = *outputLength + 1;
		i += numOctets;
	}

	return g_Utf8ErrorCode_Success;
}

g_Utf8ErrorCode g_dumbString_utf8Length(const char* rawString, size_t* outputLength)
{
	size_t numBytes = getNumBytesTilNull(rawString);
	return g_dumbString_utf8Length(rawString, numBytes, outputLength);
}

bool operator==(const g_DumbString& a, const g_DumbString& b)
{
	return g_memory_compareMem(a.data, a.numBytes, b.data, b.numBytes);
}

bool operator==(const g_DumbConstantString& a, const g_DumbConstantString& b)
{
	return g_memory_compareMem((void*)a.rawStringLiteral, a.numBytes, (void*)b.rawStringLiteral, b.numBytes);
}

g_Utf8ErrorCode g_parser_makeParser(const char* rawString, size_t numBytes, g_ParseInfo* out)
{
	out->cursor = 0;
	out->numBytes = numBytes;
	out->utf8String = (const uint8_t*)rawString;

	// TODO: Validate the UTF8 string up front so that we can parse the string in safety
	return g_Utf8ErrorCode_Success;
}

g_Utf8ErrorCode g_parser_makeParser(const char* rawString, g_ParseInfo* out)
{
	return g_parser_makeParser(rawString, getNumBytesTilNull(rawString), out);
}

uint32_t g_parser_parseCharacter(g_ParseInfo& parseInfo, uint8_t* numBytesParsed, size_t offset)
{
	uint8_t numOctets = getNumOctets(parseInfo.utf8String, parseInfo.numBytes, parseInfo.cursor + offset);
	if (numOctets > 4)
	{
		throw std::runtime_error("Invalid UTF8 string somehow encountered during parsing. This means the validation of the UTF8 string failed for some reason.");
	}

	uint32_t res = 0;
	{
		uint8_t bitMask = 0b0011'1111;
		for (uint8_t i = numOctets - 1; i > 0; i--)
		{
			uint8_t maskedBits = parseInfo.utf8String[parseInfo.cursor + i + offset] & bitMask;
			uint8_t shiftAmt = (numOctets - i - 1) * 6;
			res = res | ((uint32_t)maskedBits << shiftAmt);
		}
	}

	switch (numOctets)
	{
	case 1:
	{
		uint8_t bitMask = 0b0111'1111;
		uint8_t maskedBits = parseInfo.utf8String[parseInfo.cursor + offset] & bitMask;
		res = res | (uint32_t)maskedBits;
	}
	break;
	case 2:
	{
		uint8_t bitMask = 0b0001'1111;
		uint8_t maskedBits = parseInfo.utf8String[parseInfo.cursor + offset] & bitMask;
		res = res | ((uint32_t)maskedBits << 6);
	}
	break;
	case 3:
	{
		uint8_t bitMask = 0b0000'1111;
		uint8_t maskedBits = parseInfo.utf8String[parseInfo.cursor + offset] & bitMask;
		res = res | ((uint32_t)maskedBits << 12);
	}
	break;
	case 4:
	{
		uint8_t bitMask = 0b0000'0111;
		uint8_t maskedBits = parseInfo.utf8String[parseInfo.cursor + offset] & bitMask;
		res = res | ((uint32_t)maskedBits << 18);
	}
	break;
	default:
		throw std::runtime_error("Unknown number of octets hit while parsing a UTF8 string.");
	}

	*numBytesParsed = numOctets;
	return res;
}

uint32_t g_parser_peek(g_ParseInfo& parseInfo, uint8_t* numBytesParsed, size_t peekAmount)
{
	if (parseInfo.cursor >= parseInfo.numBytes)
	{
		return '\0';
	}

	uint32_t res = 0;

	size_t peekCursorPos = 0;
	size_t numCharsParsed = 0;
	while (numCharsParsed <= peekAmount)
	{
		uint8_t numBytesParsedInternal = 0;
		res = g_parser_parseCharacter(parseInfo, &numBytesParsedInternal, peekCursorPos);
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

static uint8_t getNumOctets(const uint8_t* string, size_t numBytes, size_t cursor)
{
	if (cursor >= numBytes)
	{
		return g_Utf8ErrorCode_InvalidString;
	}

	uint8_t numOctets = 0;
	for (uint8_t i = 0; i < numOctetMasks; i++)
	{
		bool octetMaskPass = (string[cursor] & OCTET_MASKS[i]) == OCTET_MASKS[i];
		bool zeroMaskPass = (string[cursor] & OCTET_0_MASKS[i]) < OCTET_0_MASKS[i];
		if (octetMaskPass && zeroMaskPass)
		{
			numOctets = (i + 1);
			cursor++;
			break;
		}
	}

	if (numOctets == 0)
	{
		return g_Utf8ErrorCode_InvalidString;
	}

	// Validate the following bytes, if any of them are invalid then
	// we return the appropriate error code
	for (uint8_t i = 1; i < numOctets; i++)
	{
		if (cursor >= numBytes)
		{
			return g_Utf8ErrorCode_InvalidString;
		}

		bool highBitPass = (string[cursor] & ONE_ZERO_MASK) == ONE_ZERO_MASK;
		bool secondHighBitPass = (string[cursor] & ONE_ZERO_0_MASK) < ONE_ZERO_0_MASK;
		if (!highBitPass || !secondHighBitPass)
		{
			return g_Utf8ErrorCode_InvalidString;
		}
	}

	return numOctets;
}

#endif // GABE_CPP_STRINGS_IMPL