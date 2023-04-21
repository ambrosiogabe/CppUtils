#ifndef GABE_CPP_UTILS_TESTS_H
#define GABE_CPP_UTILS_TESTS_H

#define ADD_TEST(testSuite, testName) Tests::addTest(testSuite, #testName, testName)
#define ADD_BEFORE_EACH(testSuite, testName) Tests::setBeforeEach(testSuite, #testName, testName)
#define ADD_AFTER_EACH(testSuite, testName) Tests::setAfterEach(testSuite, #testName, testName)

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

namespace CppUtils
{
namespace Tests
{

struct TestSuite;
typedef const char* (*TestFn)();

TestSuite& addTestSuite(const char* testSuiteName);

void addTest(TestSuite& testSuite, const char* testName, TestFn fn);

void setBeforeEach(TestSuite& testSuite, const char* testName, TestFn fn);
void setAfterEach(TestSuite& testSuite, const char* testName, TestFn fn);

void runTests();

void free();

}
} // End namespace CppUtils::Tests

#endif // GABE_CPP_UTILS_TESTS_H

#ifdef GABE_CPP_UTILS_IMPL

#include <stdint.h>
#include <vector>

#undef GABE_CPP_UTILS_IMPL
#include "cppUtils/cppUtils.hpp"
#include "cppUtils/cppPrint.hpp"
#define GABE_CPP_UTILS_IMPL

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

void setBeforeEach(TestSuite& testSuite, const char* testName, TestFn fn)
{
	testSuite.beforeEach = createTestPrototype(testName, fn);
}

void setAfterEach(TestSuite& testSuite, const char* testName, TestFn fn)
{
	testSuite.afterEach = createTestPrototype(testName, fn);
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

		if (testSuite.tests)
		{
			g_memory_free(testSuite.tests);
		}

		freeTestPrototype(testSuite.beforeEach);
		freeTestPrototype(testSuite.afterEach);

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
			size_t strLength = std::strlen(result) + 1;
			testSuite->tests[i].testResult = (char*)g_memory_allocate(sizeof(char) * strLength);
			g_memory_copyMem(testSuite->tests[i].testResult, (void*)result, strLength);
		}

		if (testSuite->afterEach.fn)
		{
			testSuite->afterEach.fn();
		}
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
	g_memory_copyMem(test.name, (void*)testName, sizeof(uint8) * test.nameLength);
	test.name[test.nameLength] = '\0';
	return test;
}

static void freeTestPrototype(TestPrototype& test)
{
	if (test.name)
	{
		g_memory_free(test.name);
	}

	if (test.testResult)
	{
		g_memory_free(test.testResult);
	}

	test.name = nullptr;
	test.nameLength = 0;
	test.fn = nullptr;
}

}
} // End namespace CppUtils::Tests

#endif 