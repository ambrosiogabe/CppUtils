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
  |	   (hexadecimal)    |                   (binary)                |
  +---------------------+----------+----------+----------+----------+
  |                     | byte 0   | byte 1   | byte 2   | byte 3   |
  +---------------------+----------+----------+----------+----------+
  | 0000 0000-0000 007F | 0xxxxxxx | 		  |          |          |
  | 0000 0080-0000 07FF | 110xxxxx | 10xxxxxx |          |          |
  | 0000 0800-0000 FFFF | 1110xxxx | 10xxxxxx | 10xxxxxx |          |
  | 0001 0000-0010 FFFF | 11110xxx | 10xxxxxx | 10xxxxxx | 10xxxxxx |
  +---------------------+----------+----------+----------+----------+

  - Encoding a character -
  1. Determine the number of octets required per column [0]
  2. Prepare the higher order bits of the octets per columns [1-4]
 
*/
#ifndef GABE_CPP_STRINGS_H
#define GABE_CPP_STRINGS_H

#include <stdint.h>

// Should always be UTF8-Encoded
struct g_DumbString
{
	uint8_t* data;
	size_t numBytes;
	size_t numCharacters;
};

enum g_Utf8ErrorCode
{
	g_Utf8ErrorCode_Success = 0,
	g_Utf8ErrorCode_InvalidString = 5,
};

g_Utf8ErrorCode g_dumbString_new(const char* rawString, g_DumbString* outputString);
void g_dumbString_free(g_DumbString& string);

g_Utf8ErrorCode g_dumbString_utf8Length(const char* rawString, size_t* outputLength);
g_Utf8ErrorCode g_dumbString_utf8Length(const char* rawString, size_t rawStringNumBytes, size_t* outputLength);

bool operator==(const g_DumbString& a, const g_DumbString& b);

#endif // GABE_CPP_STRINGS_H

#ifdef GABE_CPP_STRINGS_IMPL
#include <cppUtils/cppUtils.hpp>

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
static inline uint8_t getNumOctets(const g_DumbString& dumbString, size_t cursor) { return getNumOctets(dumbString.data, dumbString.numBytes, cursor); }

g_Utf8ErrorCode g_dumbString_new(const char* rawString, g_DumbString* outputString)
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

	// Count the number of characters in the string and also validate the UTF8 string
	// along the way
	size_t numChars = 0;
	for (size_t i = 0; i < numBytes;)
	{
		uint8_t numOctets = getNumOctets(*outputString, i);
		if (numOctets == (uint8_t)g_Utf8ErrorCode_InvalidString)
		{
			g_dumbString_free(*outputString);
			return g_Utf8ErrorCode_InvalidString;
		}

		numChars++;
		i += numOctets;
	}

	outputString->numCharacters = numChars;
	return g_Utf8ErrorCode_Success;
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
	if (a.numBytes != b.numBytes)
	{
		return false;
	}

	return g_memory_compareMem(a.data, a.numBytes, b.data, b.numBytes);
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