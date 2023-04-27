# CppUtils

This is a collection of utilities for C and C++ that I've created to help out in my own projects. Each file has extensive documentation about how to use it at the top of the file, and each file is meant to used as a single included file. Here's a brief description of the utilities.

## Overview

### `cppUtils/cppUtils.hpp`

This file is meant to be a general purpose memory tracker, buffer corruption detector, and logging utility. It has support for logging warnings/errors/info/assertions. It breaks into the debugger on assertion failure on MSVC (Linux support coming soon).

When the program completes execution, you can print out any memory leaks or buffer corruption and it will tell you the source file and line of code that allocated the memory.

This file is also meant to be used with real-time applications. I use it in my games and have it on in debug/release mode with negligible impacts on the performance. However you can turn off memory tracking and if release optimizations are turned on it should remove the dead branches and provide bare-bones functionality with no safety checks.

Tested with C11 and C++17.

May be compatible with earlier versions.

This file is standalone. Meaning you can use it without using any of the other utilities. So feel free to copy/paste the file into a project, or to clone this repository as a submodule and include it that way.

Example usage:

```cpp
// Initialize the stuff
g_logger_init();
g_logger_set_level(g_logger_level_All);

// Not necessary to have such big buffers, but it's useful for debugging memory corruption
g_memory_init_padding(true, 1024);
// g_memory_init_padding(true, 5); // Better default that still catches off by ones etc.

void* memoryLeak = g_memory_allocate(sizeof(uint8) * 255);

void* nonLeakedMemory = g_memory_allocate(sizeof(uint8) * 255);
// Later
g_memory_free(nonLeakedMemory);

// I don't use C++, but you can use this with C++ objects, it's just kind of gross
void* cppObject = g_memory_allocate(sizeof(Foo));
new(cppObject)Foo;
// Later
Foo->~();
g_memory_free(Foo);

uint8* bufferCorruption = g_memory_allocate(sizeof(uint8) * 10);
bufferCorruption[12] = 'a';
g_memory_free(bufferCorruption);

g_logger_error("This is an error! Here's some PI {*:^10.5f}", 3.14f);

// This will print buffer corruption and memory leaks
g_memory_dumpMemoryLeaks();

// Free our resources like good boi programmers
g_memory_deinit();
g_logger_free();
```

Setup:

```cpp
/*
 Do this:
    #define GABE_CPP_UTILS_IMPL
 before you include this file in *one* C/C++ file to create the implementation.

 // i.e. it should look like this in *one* source file:
 #define GABE_CPP_UTILS_IMPL
 #include "cppUtils.hpp"
 */
 ```

 More extensive documentation available at the top of the file.

### `cppUtils/cppPrint.hpp`

### `cppUtils/cppStrings.hpp`

### `cppUtils/cppTests.hpp`

 Requires C++17 or greater.

 These are intended to be used with the rest of the source files in this directory.

 Printing has UTF8 support.

 Printing also matches C's printf formatting where applicable (with a few additions that strive to be similar to the popular fmt library).

 Example usage:

```cpp
// Prints
// ╔════════════════════╗
// ║    Hello World!    ║
// ╚════════════════════╝
g_logger_info("\n"
    u8"\u2554{\u2550:^20}\u2557\n"
    u8"\u2551{ :^20}\u2551\n"
    u8"\u255a{\u2550:^20}\u255d",
    "",
    u8"Hello World!",
    ""
);
```

 Setup:

 ```cpp
 /*
 This is to be used in conjunction with the other source files, to use it do this:
    #define GABE_CPP_UTILS_IMPL
 before you include this file (and the others) in *one* C++ file to create the implementation.

 // i.e. it should look like this in *one* source file:
 // The order of the includes is important, keep it in this order and you'll be fine
 #define GABE_CPP_UTILS_IMPL
 #include <cppUtils/cppPrint.hpp>
 #include <cppUtils/cppUtils.hpp>
 #include <cppUtils/cppTests.hpp>
 #include <cppUtils/cppStrings.hpp>

 Then you can include it anywhere else you please.

 This works nicely with cppUtils.hpp. All calls to g_logger_* will use this print library,
 allowing you to make print statements like `g_logger_info("{:.2f}", 3.1416f)`.
 */
```

More extensive documentation available at the top of the files.

### `cppUtils/cppMaybe.hpp`

Requires C++17 or greater.

This is a simple library meant to aid in returning `maybe` results with type-safe error codes. It shouldn't take any additional space for the error code by using a `std::variant` under the hood to create a type-safe union.

This shouldn't invoke any copies and should move the value or error directly into the structure.

Example usage:

```cpp
// Using an API that returns a maybe
Maybe<std::string, ParseError> parseInfo = parseString(sourceStr, sourceStrLength);
if (!parseInfo.hasValue())
{
    g_logger_error("Failed to parse string with error {}", parseInfo.error());
    return;
}

std::string& mutStr = parseInfo.mut_value();
// Or
const std::string& str = parseInfo.value();
// Or
const std::string& str = *parseInfo;

// Creating an API that returns a maybe
Maybe<std::string, ParseError> parseString(const char* source, size_t sourceLength) 
{
  // Do some sort of validation
  if (!isValid) 
  {
    return ParseError::InvalidString;
  }
  
  return result;
}
```
