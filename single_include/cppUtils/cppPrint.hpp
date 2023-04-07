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

#include <iostream>
#include <string>

// Override this define to add DLL support on your platform
#ifndef GABE_CPP_PRINT_API 
#define GABE_CPP_PRINT_API
#endif

GABE_CPP_PRINT_API void g_io_printf(const char* s);
GABE_CPP_PRINT_API void _g_io_printf_internal(const char* s, size_t length);

template<typename T, typename...Args>
GABE_CPP_PRINT_API void _g_io_printf_internal(const char* s, size_t length, const T& value, const Args&... args)
{
	size_t fmtStart = SIZE_MAX;
	for (size_t i = 0; i < length; i++)
	{
		if (s[i] == '{' && fmtStart == SIZE_MAX)
		{
			fmtStart = i;
			continue;
		}

		if (s[i] == '{' && fmtStart == i - 1)
		{
			fmtStart = SIZE_MAX;
		}

		if (fmtStart != SIZE_MAX)
		{
			// Closing the format string
			if (s[i] == '}')
			{
				std::cout << value;
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
			std::cout << s[i];
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
#endif // GABE_CPP_PRINT_IMPL