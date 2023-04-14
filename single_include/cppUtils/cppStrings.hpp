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
*/
#ifndef GABE_CPP_STRINGS_H
#define GABE_CPP_STRINGS_H

#include <stdint.h>

// Should always be UTF8-Encoded
struct g_DumbString
{
	uint8_t* str;
	size_t numBytes;
};

g_DumbString g_dumbString_new(const char* rawString);
void g_dumbString_free(g_DumbString& string);

inline size_t g_dumbString_asciiLength(const g_DumbString& string) { return string.numBytes; }
size_t g_dumbString_utf8Length(const g_DumbString& string);

bool operator==(const g_DumbString& a, const g_DumbString& b);

#endif // GABE_CPP_STRINGS_H

#ifdef GABE_CPP_STRINGS_IMPL
#define GABE_CPP_STRINGS_IMPL

#include <stdlib.h>
#include <algorithm>

#include <cppUtils/cppUtils.hpp>

// Forward declarations
static size_t getStringLength(const char* rawString)
{
	size_t i = 0;
	while (rawString[0] != '\0')
	{
		i++;
		rawString++;
	}

	return i;
}

g_DumbString g_dumbString_new(const char* rawString)
{
	size_t strLength = getStringLength(rawString);
	uint8* dumbString = (uint8*)g_memory_allocate((strLength + 1) * sizeof(uint8));

	g_memory_copyMem(dumbString, (void*)rawString, strLength * sizeof(uint8));
	dumbString[strLength] = '\0';

	return {
		dumbString,
		strLength
	};
}

void g_dumbString_free(g_DumbString& string)
{
	g_memory_free(string.str);
	g_memory_zeroMem(&string, sizeof(g_DumbString));
}


size_t g_dumbString_utf8Length(const g_DumbString&)
{
	// TODO: Implement me
	return 0;
}

bool operator==(const g_DumbString& a, const g_DumbString& b)
{
	return g_memory_compareMem(a.str, a.numBytes, b.str, b.numBytes);
}

#endif // GABE_CPP_STRINGS_IMPL