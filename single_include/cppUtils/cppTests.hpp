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

 This is a quick and dirty lightweight C++ testing library. It has simple support for test suites and 
 unit tests. The API is meant to be as ergonomic as possible, and to do this it uses macros.

 ------ Reference ------


 ---- Macros ----
 ADD_TEST(testSuite, testName) 
   
   * Adds test with `testName` to the testSuite

 ADD_BEFORE_EACH(testSuite, functionName)

   * Will run the function described by `functionName` before each test in the test suite

 ADD_AFTER_EACH(testSuite, functionName)

   * Will run the function described by `functionName` after each test in the test suite

 ADD_BEFORE_ALL(testSuite, functionName)

   * Will run the function described by `functionName` before all tests in suite are run

 ADD_AFTER_EACH(testSuite, functionName)

   * Will run the function described by `functionName` after all tests in suite are run

 ASSERT_TRUE(val)
 ASSERT_FALSE(val)

 ASSERT_EQUAL(a, b)
 ASSERT_NOT_EQUAL(a, b)

 ASSERT_NULL(val)
 ASSERT_NOT_NULL(val)

 DEFINE_TEST(fnName)
 END_TEST

   * These two functions are used to define a test. Example usage is shown below in the Quickstart.

 DEFINE_BEFORE_EACH(fnName) const char* fnName()
 END_BEFORE_EACH return nullptr

   * These two functions are used exactly the same way as defining a test like above.

 DEFINE_AFTER_EACH(fnName) const char* fnName()
 END_AFTER_EACH return nullptr

   * Same as above, except used for defining the after each function.


 ---- Functions ----

 The namespace is CppUtils::Tests for all the following functions.

 TestSuite& addTestSuite(const char* testSuiteName);

   * Used to add a test suite to the global set of tests. Example usage below.

 void addTest(TestSuite& testSuite, const char* testName, TestFn fn);
 void setBeforeEach(TestSuite& testSuite, const char* testName, TestFn fn);
 void setAfterEach(TestSuite& testSuite, const char* testName, TestFn fn);
 void setBeforeAll(TestSuite& testSuite, const char* functionName, TestFn fn);
 void setAfterAll(TestSuite& testSuite, const char* functionName, TestFn fn);

	* Used to add a test/beforeEach/afterEach/beforeAll/afterAll to a test suite. The 
	  ADD_TEST/ADD_BEFORE_EACH/ADD_AFTER_EACH/ADD_BEFORE_ALL/ADD_AFTER_ALL helper macros 
	  are preferrable here.

 void runTests();

    * Will run all the tests and print the results to stdout.

 void free();

    * Frees any resources acquired during the test runs.

 ------ Quickstart ------

 Here's some code illustrating how to use this testing framework for two dummy test suites.

// -------- Begin myTestCode.cpp --------
#include <cppUtils/cppPrint.hpp>
#define GABE_CPP_UTILS_IMPL
#include <cppUtils/cppUtils.hpp>
#define GABE_CPP_TESTS_IMPL
#include <cppUtils/cppTests.hpp>
#include <cppUtils/cppStrings.hpp>

namespace ThreadPoolTestSuite
{

DEFINE_TEST(dummy)
{
	ASSERT_TRUE(false);
	END_TEST;
}

void setupThreadPoolTestSuite()
{
    // This name can be anything.
	Tests::TestSuite& testSuite = Tests::addTestSuite("My Thread Pool Test Suite");

	// Add as many tests as you like here
	ADD_TEST(testSuite, dummy);
}

}

namespace CppUtilsTestSuite
{

DEFINE_TEST(dummy)
{
	ASSERT_TRUE(false);
	END_TEST;
}

void setupCppUtilsTestSuite()
{
	Tests::TestSuite& testSuite = Tests::addTestSuite("My Utilities Test Suite");

	ADD_TEST(testSuite, dummy);
}

}

void main() 
{
	g_logger_init();
	g_logger_set_level(g_logger_level_All);
	g_logger_set_log_directory("C:/dev/C++/CppUtils/logs");
	g_memory_init_padding(true, 1024);

	ThreadPoolTestSuite::setupThreadPoolTestSuite();
	CppUtilsTestSuite::setupCppUtilsTestSuite();

	Tests::runTests();
	Tests::free();

	g_memory_dumpMemoryLeaks();
	g_memory_deinit();
	g_logger_free();
}

// -------- End myTestCode.cpp --------


 -------- DLL STUFF --------

 This library is not intended to be used as a DLL. You should statically link it.

 If you have a use-case for dynamic linking, feel free to open an issue on the Github repository.
*/

#ifndef GABE_CPP_UTILS_TESTS_H
#define GABE_CPP_UTILS_TESTS_H

#define ADD_TEST(testSuite, testName) Tests::addTest(testSuite, #testName, testName)
#define ADD_BEFORE_EACH(testSuite, functionName) Tests::setBeforeEach(testSuite, #functionName, functionName)
#define ADD_AFTER_EACH(testSuite, functionName) Tests::setAfterEach(testSuite, #functionName, functionName)
#define ADD_BEFORE_ALL(testSuite, functionName) Tests::setBeforeAll(testSuite, #functionName, functionName)
#define ADD_AFTER_ALL(testSuite, functionName) Tests::setAfterAll(testSuite, #functionName, functionName)

#define ASSERT_TRUE(val) { if (!(val)) return u8"ASSERT_TRUE("#val")"; }
#define ASSERT_FALSE(val) { if (val) return u8"ASSERT_FALSE("#val")"; }

#define ASSERT_EQUAL(a, b) { if (a != b) return u8"ASSERT_EQUAL("#a", "#b")"; }
#define ASSERT_NOT_EQUAL(a, b) { if (a == b) return u8"ASSERT_NOT_EQUAL("#a", "#b")"; }

#define ASSERT_NULL(val) { if (val != nullptr) return u8"ASSERT_NULL("#val")"; }
#define ASSERT_NOT_NULL(val) { if (val == nullptr) return u8"ASSERT_NOT_NULL("#val")"; }

#define DEFINE_TEST(fnName) const char* fnName()
#define END_TEST return nullptr

#define DEFINE_BEFORE_EACH(fnName) const char* fnName()
#define END_BEFORE_EACH return nullptr

#define DEFINE_AFTER_EACH(fnName) const char* fnName()
#define END_AFTER_EACH return nullptr 

#define DEFINE_BEFORE_ALL(fnName) const char* fnName()
#define END_BEFORE_ALL return nullptr

#define DEFINE_AFTER_ALL(fnName) const char* fnName()
#define END_AFTER_ALL return nullptr 

namespace CppUtils
{
namespace Tests
{

struct TestSuite;
typedef const char* (*TestFn)();

TestSuite& addTestSuite(const char* testSuiteName);

void addTest(TestSuite& testSuite, const char* testName, TestFn fn);

void setBeforeEach(TestSuite& testSuite, const char* functionName, TestFn fn);
void setAfterEach(TestSuite& testSuite, const char* functionName, TestFn fn);

void setBeforeAll(TestSuite& testSuite, const char* functionName, TestFn fn);
void setAfterAll(TestSuite& testSuite, const char* functionName, TestFn fn);

void runTests();

void free();

}
} // End namespace CppUtils::Tests

#endif // GABE_CPP_UTILS_TESTS_H

#ifdef GABE_CPP_TESTS_IMPL

#include <stdint.h>
#include <vector>

#include <cppUtils/cppPrint.hpp>
#include <cppUtils/cppUtils.hpp>

namespace CppUtils
{
namespace Tests
{

// ----------------- Internal structures -----------------
struct TestPrototype
{
	char* testResult;
	uint8_t* name;
	size_t nameLength;
	TestFn fn;
};

struct TestSuite
{
	const char* name;
	TestPrototype* tests;
	size_t testsLength;
	size_t numTestsPassed;

	TestPrototype beforeEach = {};
	TestPrototype afterEach = {};

	TestPrototype beforeAll = {};
	TestPrototype afterAll = {};
};

// ----------------- Internal variables -----------------
static std::vector<TestSuite> testSuites = {};

// ----------------- Internal functions -----------------
static void runTestSuite(void* testSuiteRaw, size_t testSuiteSize);
static void printTestSuiteResCallback(void* testSuiteRaw, size_t testSuiteSize);
static TestPrototype createTestPrototype(const char* testName, TestFn fn);
static void freeTestPrototype(TestPrototype& test);

TestSuite& addTestSuite(const char* testSuiteName)
{
	TestSuite res = {};
	res.name = testSuiteName;
	res.numTestsPassed = 0;
	res.tests = nullptr;
	res.testsLength = 0;
	testSuites.push_back(res);

	return testSuites[testSuites.size() - 1];
}

void addTest(TestSuite& testSuite, const char* testName, TestFn fn)
{
	testSuite.testsLength++;
	testSuite.tests = (TestPrototype*)g_memory_realloc(
		testSuite.tests,
		sizeof(TestPrototype) * testSuite.testsLength
	);

	TestPrototype test = createTestPrototype(testName, fn);
	testSuite.tests[testSuite.testsLength - 1] = test;
}

void setBeforeEach(TestSuite& testSuite, const char* functionName, TestFn fn)
{
	testSuite.beforeEach = createTestPrototype(functionName, fn);
}

void setAfterEach(TestSuite& testSuite, const char* functionName, TestFn fn)
{
	testSuite.afterEach = createTestPrototype(functionName, fn);
}

void setBeforeAll(TestSuite& testSuite, const char* functionName, TestFn fn)
{
	testSuite.beforeAll = createTestPrototype(functionName, fn);
}

void setAfterAll(TestSuite& testSuite, const char* functionName, TestFn fn)
{
	testSuite.afterAll = createTestPrototype(functionName, fn);
}

void runTests()
{
	g_logger_info("Running {} test {}.\n", testSuites.size(), testSuites.size() > 1 ? "suites" : "suite");

	// TODO: Add thread pool implementation
	//GlobalThreadPool threadPool = GlobalThreadPool(std::thread::hardware_concurrency());

	//for (const auto& testSuite : testSuites)
	//{
	//	threadPool.queueTask(
	//		runTestSuite,
	//		testSuite.name,
	//		(void*)&testSuite,
	//		sizeof(TestSuite),
	//		MathAnim::Priority::None,
	//		printTestSuiteResCallback
	//	);
	//}

	//threadPool.beginWork();

	//// Should join all threads and wait for tasks to finish
	//threadPool.free();

	for (const auto& testSuite : testSuites)
	{
		runTestSuite((void*)&testSuite, sizeof(TestSuite));
		printTestSuiteResCallback((void*)&testSuite, sizeof(TestSuite));
	}

	size_t numTestSuitesPassed = 0;
	for (const auto& testSuite : testSuites)
	{
		if (testSuite.numTestsPassed >= testSuite.testsLength)
		{
			numTestSuitesPassed++;
		}
	}


	IO::printf("\n  Number of Test Suites Passed ");

	if (numTestSuitesPassed == testSuites.size())
	{
		IO::setForegroundColor(ConsoleColor::GREEN);
	}
	else
	{
		IO::setForegroundColor(ConsoleColor::RED);
	}

	IO::printf("{}/{}\n\n",
		numTestSuitesPassed,
		testSuites.size()
	);
	IO::resetColor();
}

void free()
{
	for (auto& testSuite : testSuites)
	{
		for (size_t i = 0; i < testSuite.testsLength; i++)
		{
			freeTestPrototype(testSuite.tests[i]);
		}

		g_memory_free(testSuite.tests);

		freeTestPrototype(testSuite.beforeEach);
		freeTestPrototype(testSuite.afterEach);
		freeTestPrototype(testSuite.beforeAll);
		freeTestPrototype(testSuite.afterAll);

		testSuite.tests = nullptr;
		testSuite.numTestsPassed = 0;
		testSuite.name = nullptr;
		testSuite.testsLength = 0;
	}

	testSuites.clear();
}

// ----------------- Internal functions -----------------
static void runTestSuite(void* testSuiteRaw, size_t testSuiteSize)
{
	g_logger_assert(testSuiteSize == sizeof(TestSuite), "Invalid data passed to runTestSuite");
	TestSuite* testSuite = (TestSuite*)testSuiteRaw;

	if (testSuite->beforeAll.fn)
	{
		testSuite->beforeAll.fn();
	}

	for (size_t i = 0; i < testSuite->testsLength; i++)
	{
		if (testSuite->beforeEach.fn)
		{
			testSuite->beforeEach.fn();
		}

		const char* result = testSuite->tests[i].fn();
		if (result == nullptr)
		{
			testSuite->numTestsPassed++;
			testSuite->tests[i].testResult = nullptr;
		}
		else
		{
			size_t strLengthWithNullByte = std::strlen(result) + 1;
			testSuite->tests[i].testResult = (char*)g_memory_allocate(sizeof(char) * strLengthWithNullByte);
			testSuite->tests[i].nameLength = strLengthWithNullByte - 1;
			g_memory_copyMem(testSuite->tests[i].testResult, testSuite->tests[i].nameLength + 1, (void*)result, strLengthWithNullByte);
		}

		if (testSuite->afterEach.fn)
		{
			testSuite->afterEach.fn();
		}
	}

	if (testSuite->afterAll.fn)
	{
		testSuite->afterAll.fn();
	}
}

static void printTestSuiteResCallback(void* testSuiteRaw, size_t testSuiteSize)
{
	g_logger_assert(testSuiteSize == sizeof(TestSuite), "Invalid data passed to printTestSuiteResCallback");
	TestSuite* testSuite = (TestSuite*)testSuiteRaw;

	IO::printf("  Test Suite '{}' Results...\n\n", testSuite->name);

	for (size_t i = 0; i < testSuite->testsLength; i++)
	{
		if (testSuite->tests[i].testResult == nullptr)
		{
			IO::setForegroundColor(ConsoleColor::GREEN);
			IO::printf(u8"      \u2713 Success ");
			IO::resetColor();
			IO::printf("'{}::{}'\n", testSuite->name, testSuite->tests[i].name);
		}
		else
		{
			IO::setForegroundColor(ConsoleColor::RED);
			IO::printf(u8"      \u0078 Fail    ");
			IO::resetColor();
			IO::printf("'{}::{}'\n"
				"        Failed at: ",
				testSuite->name,
				testSuite->tests[i].name
			);
			IO::setForegroundColor(ConsoleColor::RED);
			IO::printf("{}\n", testSuite->tests[i].testResult);
			IO::resetColor();
		}
	}

	if (testSuite->numTestsPassed < testSuite->testsLength)
	{
		IO::setForegroundColor(ConsoleColor::RED);
		IO::printf("\n    Suite Fail ");
		IO::resetColor();
		IO::printf("'{}'", testSuite->name);
	}
	else
	{
		IO::setForegroundColor(ConsoleColor::GREEN);
		IO::printf("\n    Suite Success ");
		IO::resetColor();
		IO::printf("'{}'", testSuite->name);
	}

	IO::printf("\n    Number of Tests Passed ");
	IO::setForegroundColor(ConsoleColor::YELLOW);
	IO::printf("{}/{}\n\n", testSuite->numTestsPassed, testSuite->testsLength);
	IO::resetColor();
}

static TestPrototype createTestPrototype(const char* testName, TestFn fn)
{
	TestPrototype test = {};
	test.fn = fn;
	test.nameLength = std::strlen(testName);
	test.name = (uint8*)g_memory_allocate(sizeof(uint8) * (test.nameLength + 1));
	g_memory_copyMem(test.name, test.nameLength, (void*)testName, sizeof(uint8) * test.nameLength);
	test.name[test.nameLength] = '\0';
	return test;
}

static void freeTestPrototype(TestPrototype& test)
{
	g_memory_free(test.name);
	g_memory_free(test.testResult);

	test.name = nullptr;
	test.nameLength = 0;
	test.fn = nullptr;
}

}
} // End namespace CppUtils::Tests

#endif // end GABE_CPP_TESTS_IMPL